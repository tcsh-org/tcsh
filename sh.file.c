/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.file.c,v 2.0 1991/03/26 03:46:14 christos Exp $ */
/*
 * sh.file.c: File completion of the old csh. This file is not used.
 *	      Tcsh now uses the routines in tw.*.c
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifdef FILEC
#include "config.h"
#ifdef notdef
static Char *sccsid = "@(#)sh.file.c	5.6 (Berkeley) 5/18/86";
#endif
#ifndef lint
static Char *rcsid = "$Id: sh.file.c,v 2.0 1991/03/26 03:46:14 christos Exp $";
#endif

/*
 * Tenex style file name recognition, .. and more.
 * History:
 *	Author: Ken Greer, Sept. 1975, CMU.
 *	Finally got around to adding to the Cshell., Ken Greer, Dec. 1981.
 */

#include "sh.h"

#define ON	1
#define OFF	0

#define ESC	'\033'

typedef enum {LIST, RECOGNIZE} COMMAND;

static void setup_tty();
static void back_to_col_1();
static void pushback();
static void catn();
static void copyn();
static Char filetype();
static void extract_dir_and_name();
static Char * getentry();
static void free_items();
static void search();
static void recognize();
static void is_prefix();
static void is_suffix();
static void ignored();

extern int	sortscmp();			/* defined in sh.glob.c */
/*
 * Put this here so the binary can be patched with adb to enable file
 * completion by default.  Filec controls completion, nobeep controls
 * ringing the terminal bell on incomplete expansions.
 */
static bool filec = 0;

static void
setup_tty(on)
	int on;
{
	struct sgttyb sgtty;
	static struct tChars tChars;	/* INT, QUIT, XON, XOFF, EOF, BRK */

	if (on) {
		(void) ioctl(SHIN, TIOCGETC, (ioctl_t)&tChars);
		tChars.t_brkc = ESC;
		(void) ioctl(SHIN, TIOCSETC, (ioctl_t)&tChars);
		/*
		 * This must be done after every command: if
		 * the tty gets into raw or cbreak mode the user
		 * can't even type 'reset'.
		 */
		(void) ioctl(SHIN, TIOCGETP, (ioctl_t)&sgtty);
		if (sgtty.sg_flags & (RAW|CBREAK)) {
			 sgtty.sg_flags &= ~(RAW|CBREAK);
			 (void) ioctl(SHIN, TIOCSETP, (ioctl_t)&sgtty);
		}
	} else {
		tChars.t_brkc = -1;
		(void) ioctl(SHIN, TIOCSETC, (ioctl_t)&tChars);
	}
}

/*
 * Move back to beginning of current line
 */
static void
back_to_col_1()
{
	struct sgttyb tty, tty_normal;
	int omask;

	omask = sigblock(sigmask(SIGINT));
	(void) ioctl(SHIN, TIOCGETP, (ioctl_t)&tty);
	tty_normal = tty;
	tty.sg_flags &= ~CRMOD;
	(void) ioctl(SHIN, TIOCSETN, (ioctl_t)&tty);
	(void) write(SHOUT, "\r", 1);
	(void) ioctl(SHIN, TIOCSETN, (ioctl_t)&tty_normal);
	(void) sigsetmask(omask);
}

/*
 * Push string contents back into tty queue
 */
static void
pushback(string)
	Char *string;
{
	register Char *p;
	struct sgttyb tty, tty_normal;
	int omask;

	omask = sigblock(sigmask(SIGINT));
	(void) ioctl(SHOUT, TIOCGETP, (ioctl_t)&tty);
	tty_normal = tty;
	tty.sg_flags &= ~ECHO;
	(void) ioctl(SHOUT, TIOCSETN, (ioctl_t)&tty);

	for (p = string; *p; p++)
		(void) ioctl(SHOUT, TIOCSTI, (ioctl_t) p);
	(void) ioctl(SHOUT, TIOCSETN, (ioctl_t)&tty_normal);
	(void) sigsetmask(omask);
}

/*
 * Concatenate src onto tail of des.
 * Des is a string whose maximum length is count.
 * Always null terminate.
 */
static void
catn(des, src, count)
	register Char *des, *src;
	register count;
{

	while (--count >= 0 && *des)
		des++;
	while (--count >= 0)
		if ((*des++ = *src++) == 0)
			 return;
	*des = '\0';
}

/*
 * Like strncpy but always leave room for trailing \0
 * and always null terminate.
 */
static void
copyn(des, src, count)
	register Char *des, *src;
	register count;
{

	while (--count >= 0)
		if ((*des++ = *src++) == 0)
			return;
	*des = '\0';
}

static Char
filetype(dir, file)
	Char *dir, *file;
{
	Char path[MAXPATHLEN];
	struct stat statb;

	catn(Strcpy(path, dir), file, sizeof(path) / sizeof(Char));
	if (lstat(path, &statb) == 0) {
		switch(statb.st_mode & S_IFMT) {
		    case S_IFDIR:
			return ('/');

		    case S_IFLNK:
			if (stat(path, &statb) == 0 &&	    /* follow it out */
			   (statb.st_mode & S_IFMT) == S_IFDIR)
				return ('>');
			else
				return ('@');

		    case S_IFSOCK:
			return ('=');

		    default:
			if (statb.st_mode & 0111)
				return ('*');
		}
	}
	return (' ');
}

static struct winsize win;

/*
 * Print sorted down columns
 */
static void
print_by_column(dir, items, count)
	Char *dir, *items[];
{
	register int i, rows, r, c, maxwidth = 0, columns;

	if (ioctl(SHOUT, TIOCGWINSZ, (ioctl_t)&win) < 0 || win.ws_col == 0)
		win.ws_col = 80;
	for (i = 0; i < count; i++)
		maxwidth = maxwidth > (r = Strlen(items[i])) ? maxwidth : r;
	maxwidth += 2;			/* for the file tag and space */
	columns = win.ws_col / maxwidth;
	if (columns == 0)
		columns = 1;
	rows = (count + (columns - 1)) / columns;
	for (r = 0; r < rows; r++) {
		for (c = 0; c < columns; c++) {
			i = c * rows + r;
			if (i < count) {
				register int w;

				CSHprintf("%s", items[i]);
				CSHputChar(dir ? filetype(dir, items[i]) : ' ');
				if (c < columns - 1) {	/* last column? */
					w = Strlen(items[i]) + 1;
					for (; w < maxwidth; w++)
						CSHputChar(' ');
				}
			}
		}
		CSHputChar('\r');
		CSHputChar('\n');
	}
}

/*
 * Expand file name with possible tilde usage
 *	~person/mumble
 * expands to
 *	home_directory_of_person/mumble
 */
static Char *
tilde(new, old)
	Char *new, *old;
{
	register Char *o, *p;
	register struct passwd *pw;
	static Char person[40];

	if (old[0] != '~')
		return (Strcpy(new, old));

	for (p = person, o = &old[1]; *o && *o != '/'; *p++ = *o++)
		;
	*p = '\0';
	if (person[0] == '\0')
		(void) Strcpy(new, value(STRhome));
	else {
		pw = getpwnam(short2str(person));
		if (pw == NULL)
			return (NULL);
		(void) Strcpy(new, str2short(pw->pw_dir));
	}
	(void) Strcat(new, o);
	return (new);
}

/*
 * Cause pending line to be printed
 */
static void
retype()
{
	int pending_input = LPENDIN;

	(void) ioctl(SHOUT, TIOCLBIS, (ioctl_t)&pending_input);
}

static void
beep()
{

	if (adrof(STRnobeep) == 0)
		(void) write(SHOUT, "\007", 1);
}

/*
 * Erase that silly ^[ and
 * print the recognized part of the string
 */
static void
print_recognized_stuff(recognized_part)
	Char *recognized_part;
{

	/* An optimized erasing of that silly ^[ */
	switch (Strlen(recognized_part)) {

	case 0:				/* erase two Characters: ^[ */
		CSHprintf("\210\210  \210\210");
		break;

	case 1:				/* overstrike the ^, erase the [ */
		CSHprintf("\210\210%s \210", recognized_part);
		break;

	default:			/* overstrike both Characters ^[ */
		CSHprintf("\210\210%s", recognized_part);
		break;
	}
	flush();
}

/*
 * Parse full path in file into 2 parts: directory and file names
 * Should leave final slash (/) at end of dir.
 */
static void
extract_dir_and_name(path, dir, name)
	Char *path, *dir, *name;
{
	register Char  *p;

	p = rStrchr(path, '/');
	if (p == NULL) {
		copyn(name, path, MAXNAMLEN);
		dir[0] = '\0';
	} else {
		copyn(name, ++p, MAXNAMLEN);
		copyn(dir, path, p - path);
	}
}

static Char *
getentry(dir_fd, looking_for_lognames)
	DIR *dir_fd;
{
	register struct passwd *pw;
	register struct direct *dirp;

	if (looking_for_lognames) {
		if ((pw = getpwent()) == NULL)
			return (NULL);
		return (str2short(pw->pw_name));
	}
	if (dirp = readdir(dir_fd))
		return (str2short(dirp->d_name));
	return (NULL);
}

static void
free_items(items)
	register Char **items;
{
	register int i;

	for (i = 0; items[i]; i++)
		free(items[i]);
	free((Char *)items);
}

#define FREE_ITEMS(items) { \
	int omask;\
\
	omask = sigblock(sigmask(SIGINT));\
	free_items(items);\
	items = NULL;\
	(void) sigsetmask(omask);\
}

/*
 * Perform a RECOGNIZE or LIST command on string "word".
 */
static void
search(word, command, max_word_length)
	Char *word;
	COMMAND command;
{
	static Char **items = NULL;
	register DIR *dir_fd;
	register numitems = 0, ignoring = TRUE, nignored = 0;
	register name_length, looking_for_lognames;
	Char tilded_dir[MAXPATHLEN + 1], dir[MAXPATHLEN + 1];
	Char name[MAXNAMLEN + 1], extended_name[MAXNAMLEN+1];
	Char *entry;
#define MAXITEMS 1024

	if (items != NULL)
		FREE_ITEMS(items);

	looking_for_lognames = (*word == '~') && (Strchr(word, '/') == NULL);
	if (looking_for_lognames) {
		(void) setpwent();
		copyn(name, &word[1], MAXNAMLEN);	/* name sans ~ */
	} else {
		extract_dir_and_name(word, dir, name);
		if (tilde(tilded_dir, dir) == 0)
			return (0);
		dir_fd = opendir(*tilded_dir ? short2str(tilded_dir) : ".");
		if (dir_fd == NULL)
			return (0);
	}

again:	/* search for matches */
	name_length = Strlen(name);
	for (numitems = 0; entry = getentry(dir_fd, looking_for_lognames); ) {
		if (!is_prefix(name, entry))
			continue;
		/* Don't match . files on null prefix match */
		if (name_length == 0 && entry[0] == '.' &&
		    !looking_for_lognames)
			continue;
		if (command == LIST) {
			if (numitems >= MAXITEMS) {
				CSHprintf ("\nYikes!! Too many %s!!\n",
				    looking_for_lognames ?
					"names in password file":"files");
				break;
			}
			if (items == NULL)
				items = (Char **) calloc(sizeof (items[1]),
				    MAXITEMS);
			items[numitems] = xalloc((unsigned)Strlen(entry) + 1);
			copyn(items[numitems], entry, MAXNAMLEN);
			numitems++;
		} else {			/* RECOGNIZE command */
			if (ignoring && ignored(entry))
				nignored++;
			else if (recognize(extended_name,
			    entry, name_length, ++numitems))
				break;
		}
	}
	if (ignoring && numitems == 0 && nignored > 0) {
		ignoring = FALSE;
		nignored = 0;
		if (looking_for_lognames)
			(void) setpwent();
		else
			rewinddir(dir_fd);
		goto again;
	}

	if (looking_for_lognames)
		(void) endpwent();
	else
		(void) closedir(dir_fd);
	if (numitems == 0)
		return (0);
	if (command == RECOGNIZE) {
		if (looking_for_lognames)
			 copyn(word, "~", 1);
		else
			/* put back dir part */
			copyn(word, dir, max_word_length);
		/* add extended name */
		catn(word, extended_name, max_word_length);
		return (numitems);
	}
	else { 				/* LIST */
		qsort((Char *)items, numitems, sizeof(items[1]), sortscmp);
		print_by_column(looking_for_lognames ? NULL : tilded_dir,
		    items, numitems);
		if (items != NULL)
			FREE_ITEMS(items);
	}
	return (0);
}

/*
 * Object: extend what user typed up to an ambiguity.
 * Algorithm:
 * On first match, copy full entry (assume it'll be the only match) 
 * On subsequent matches, shorten extended_name to the first
 * Character mismatch between extended_name and entry.
 * If we shorten it back to the prefix length, stop searching.
 */
static void
recognize(extended_name, entry, name_length, numitems)
	Char *extended_name, *entry;
{

	if (numitems == 1)			/* 1st match */
		copyn(extended_name, entry, MAXNAMLEN);
	else {					/* 2nd & subsequent matches */
		register Char *x, *ent;
		register int len = 0;

		x = extended_name;
		for (ent = entry; *x && *x == *ent++; x++, len++)
			;
		*x = '\0';			/* Shorten at 1st Char diff */
		if (len == name_length)		/* Ambiguous to prefix? */
			return (-1);		/* So stop now and save time */
	}
	return (0);
}

/*
 * Return true if check matches initial Chars in template.
 * This differs from PWB imatch in that if check is null
 * it matches anything.
 */
static void
is_prefix(check, template)
	register Char *check, *template;
{

	do
		if (*check == 0)
			return (TRUE);
	while (*check++ == *template++);
	return (FALSE);
}

/*
 *  Return true if the Chars in template appear at the
 *  end of check, I.e., are it's suffix.
 */
static void
is_suffix(check, template)
	Char *check, *template;
{
	register Char *c, *t;

	for (c = check; *c++;)
		;
	for (t = template; *t++;)
		;
	for (;;) {
		if (t == template)
			return 1;
		if (c == check || *--t != *--c)
			return 0;
	}
}

int
tenex(inputline, inputline_size)
	Char *inputline;
	int inputline_size;
{
	register int numitems, num_read;
	char tinputline[BUFSIZ];


	setup_tty(ON);

	while ((num_read = read(SHIN, tinputline, BUFSIZ)) > 0) {
		int i;
		static Char *delims = " '\"\t;&<>()|^%";
		register Char *str_end, *word_start, last_Char, should_retype;
		register int space_left;
		COMMAND command;

		for (i = 0; i < num_read; i++)
			inputline[i] = tinputline[i];
		last_Char = inputline[num_read - 1] & ASCII;

		if (last_Char == '\n' || num_read == inputline_size)
			break;
		command = (last_Char == ESC) ? RECOGNIZE : LIST;
		if (command == LIST)
			CSHputChar('\n');
		str_end = &inputline[num_read];
		if (last_Char == ESC)
			--str_end;		/* wipeout trailing cmd Char */
		*str_end = '\0';
		/*
		 * Find LAST occurence of a delimiter in the inputline.
		 * The word start is one Character past it.
		 */
		for (word_start = str_end; word_start > inputline; --word_start)
			if (Strchr(delims, word_start[-1]))
				break;
		space_left = inputline_size - (word_start - inputline) - 1;
		numitems = search(word_start, command, space_left);

		if (command == RECOGNIZE) {
			/* print from str_end on */
			print_recognized_stuff(str_end);
			if (numitems != 1)	/* Beep = No match/ambiguous */
				beep();
		}

		/*
		 * Tabs in the input line cause trouble after a pushback.
		 * tty driver won't backspace over them because column
		 * positions are now incorrect. This is solved by retyping
		 * over current line.
		 */
		should_retype = FALSE;
		if (Strchr(inputline, '\t')) {	/* tab Char in input line? */
			back_to_col_1();
			should_retype = TRUE;
		}
		if (command == LIST)		/* Always retype after a LIST */
			should_retype = TRUE;
		if (should_retype)
			printprompt(0, (Char *) 0);
		pushback(inputline);
		if (should_retype)
			retype();
	}
	setup_tty(OFF);
	return (num_read);
}

static void
ignored(entry)
	register Char *entry;
{
	struct varent *vp;
	register Char **cp;

	if ((vp = adrof(STRfignore)) == NULL || (cp = vp->vec) == NULL)
		return (FALSE);
	for (; *cp != NULL; cp++)
		if (is_suffix(entry, *cp))
			return (TRUE);
	return (FALSE);
}
#endif /* FILEC */

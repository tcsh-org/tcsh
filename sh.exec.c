/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.exec.c,v 2.0 1991/03/26 02:59:29 christos Exp $ */
/*
 * sh.exec.c: Search, find, and execute a command!
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

#include "config.h"

#ifdef notdef
static char *sccsid = "@(#)sh.exec.c	5.2 (Berkeley) 6/6/85";
#endif
#ifndef lint
static char *rcsid = "$Id: sh.exec.c,v 2.0 1991/03/26 02:59:29 christos Exp $";
#endif

#include "sh.h"

/*
 * C shell
 */

/*
 * System level search and execute of a command.
 * We look in each directory for the specified command name.
 * If the name contains a '/' then we execute only the full path name.
 * If there is no search path then we execute only full path names.
 */

/* 
 * As we search for the command we note the first non-trivial error
 * message for presentation to the user.  This allows us often
 * to show that a file has the wrong mode/no access when the file
 * is not in the last component of the search path, so we must
 * go on after first detecting the error.
 */
static char	*exerr;			/* Execution error message */
static Char	*expath;		/* Path for exerr */

/*
 * Xhash is an array of HSHSIZ bits (HSHSIZ / 8 chars), which are used
 * to hash execs.  If it is allocated (havhash true), then to tell
 * whether ``name'' is (possibly) present in the i'th component
 * of the variable path, you look at the bit in xhash indexed by
 * hash(hashname("name"), i).  This is setup automatically
 * after .login is executed, and recomputed whenever ``path'' is
 * changed.
 * The two part hash function is designed to let texec() call the
 * more expensive hashname() only once and the simple hash() several
 * times (once for each path component checked).
 * Byte size is assumed to be 8.
 */
#define	HSHSIZ		8192			/* 1k bytes */
#define HSHMASK		(HSHSIZ - 1)
#define HSHMUL		243
static char	xhash[HSHSIZ / 8];
#define hash(a, b)	((a) * HSHMUL + (b) & HSHMASK)
#define bit(h, b)	((h)[(b) >> 3] & 1 << ((b) & 7))	/* bit test */
#define bis(h, b)	((h)[(b) >> 3] |= 1 << ((b) & 7))	/* bit set */
#ifdef VFORK
static int	hits, misses;
#endif

/* Dummy search path for just absolute search when no path */
static Char	*justabs[] =	{ STRNULL, 0 };

static void pexerr();
static void texec();
static int hashname();

void
doexec(t)
	register struct command *t;
{
	register Char *dp, **pv, **av, *sav;
	register struct varent *v;
	register bool slash;
	register int hashval = 0, hashval1, i;
	Char *blk[2];

	/*
	 * Glob the command name. We will search $path even if this
	 * does something, as in sh but not in csh.  One special
	 * case: if there is no PATH, then we execute only commands
	 * which start with '/'.
	 */
	blk[0] = t->t_dcom[0];
	blk[1] = 0;
	gflag = 0, tglob(blk);
	if (gflag) {
		pv = globall(blk);
		if (pv == 0) {
			setname(short2str(blk[0]));
			stdbferr(ERR_NOMATCH);
		}
		gargv = 0;
	} 
	else 
		pv = saveblk(blk);

	trim(pv);

	exerr = 0; expath = Strsave(pv[0]);
#ifdef VFORK
	Vexpath = expath;
#endif

	v = adrof(STRpath);
	if (v == 0 && expath[0] != '/') {
		blkfree(pv);
		pexerr();
	}
	slash = any(short2str(expath), '/');

	/*
	 * Glob the argument list, if necessary.
	 * Otherwise trim off the quote bits.
	 */
	gflag = 0; av = &t->t_dcom[1];
	tglob(av);
	if (gflag) {
		av = globall(av);
		if (av == 0) {
			blkfree(pv);
			setname(short2str(expath));
			stdbferr(ERR_NOMATCH);
		}
		gargv = 0;
	} else
		av = saveblk(av);

	blkfree(t->t_dcom);
	t->t_dcom = blkspl(pv, av);
	xfree((ptr_t)pv);
	xfree((ptr_t)av);
	av = t->t_dcom;
	trim(av);

	if (*av == (Char *) 0 || **av == '\0')
		pexerr();

	xechoit(av);		/* Echo command if -x */
#ifdef FIOCLEX
	/*
	 * Since all internal file descriptors are set to close on exec,
	 * we don't need to close them explicitly here.  Just reorient
	 * ourselves for error messages.
	 */
	SHIN = 0; SHOUT = 1; SHDIAG = 2; OLDSTD = 0;
	isoutatty = isatty(SHOUT);
        isdiagatty = isatty(SHDIAG);
#else
	closech();		/* Close random fd's */
#endif
	/*
	 * We must do this AFTER any possible forking (like `foo`
	 * in glob) so that this shell can still do subprocesses.
	 */
#ifdef BSDSIGS
	(void) sigsetmask(0);
#else /* BSDSIGS */
	(void) sigrelse(SIGINT);
	(void) sigrelse(SIGCHLD);
#endif /* BSDSIGS */

	/*
	 * If no path, no words in path, or a / in the filename
	 * then restrict the command search.
	 */
	if (v == 0 || v->vec[0] == 0 || slash)
		pv = justabs;
	else
		pv = v->vec;
	sav = Strspl(STRslash, *av);		/* / command name for postpending */
#ifdef VFORK
	Vsav = sav;
#endif
	if (havhash)
		hashval = hashname(*av);
	i = 0;
#ifdef VFORK
	hits++;
#endif
	do {
		/* Try to save time by looking at the hash table for where
		 * this command could be.  If we are doing delayed hashing,
		 * then we put the names in one at a time, as the user
		 * enters them.  This is kinda like Korn Shell's "tracked
		 * aliases".
		 */
		if (!slash && pv[0][0] == '/' && havhash) {
			hashval1 = hash(hashval, i);
			if (!bit(xhash, hashval1))
				goto cont;
		}
		if (pv[0][0] == 0 || eq(pv[0], STRdot))	/* don't make ./xxx */
			texec(*av, av);
		else {
			dp = Strspl(*pv, sav);
#ifdef VFORK
			Vdp = dp;
#endif
			texec(dp, av);
#ifdef VFORK
			Vdp = 0;
#endif
			xfree((ptr_t) dp);
		}
#ifdef VFORK
		misses++;
#endif
cont:
		pv++;
		i++;
	} while (*pv);
#ifdef VFORK
	hits--;
#endif
#ifdef VFORK
	Vsav = 0;
#endif
	xfree((ptr_t) sav);
	pexerr();
}

static void
pexerr()
{

	/* Couldn't find the damn thing */
	if (expath) {
	    setname(short2str(expath));
#ifdef VFORK
	    Vexpath = 0;
#endif
	    xfree((ptr_t)expath);
	    expath = 0;
	} else
	    setname("");
	if (exerr)
		bferr(exerr);
	bferr("Command not found");
}

/*
 * Execute command f, arg list t.
 * Record error message if not found.
 * Also do shell scripts here.
 */
static void
texec(sf, st)
	Char *sf;
	register Char **st;
{
	register char **t;
	register char *f;
	register struct varent *v;
	register Char **vp;
	Char *lastsh[2];
	int fd;
	unsigned char c;
	Char *st0, **ost;

	/* The order for the conversions is significant */
	t = short2blk(st);
	f = short2str(sf);
#ifdef VFORK
	Vt = t;
#endif
	errno = 0;	/* don't use a previous error */
	(void) execv(f, t);
#ifdef VFORK
	Vt = 0;
#endif
	blkfree((Char **) t);
	switch (errno) {

	case ENOEXEC:
		/*
		 * From: casper@fwi.uva.nl (Casper H.S. Dik)
		 * If we could not execute it, don't feed it
		 * to the shell if it looks like a binary!
		 */
		if ((fd = open(f, O_RDONLY)) != -1) {
			if (read(fd, (char *) &c, 1) == 1) {
				if (!isprint(c) && (c != '\n' && c != '\t')) {
					(void) close(fd);
					/*
					 * We *know* what ENOEXEC means.
					 */
					error(
			      "%s: Exec format error. Wrong Architecture", f);
				}
			}
#ifdef _PATH_BSHELL
			else
				c = '#';
#endif
			(void) close(fd);
		}
		/*
		 * If there is an alias for shell, then
		 * put the words of the alias in front of the
		 * argument list replacing the command name.
		 * Note no interpretation of the words at this point.
		 */
		v = adrof1(STRshell, &aliases);
		if (v == 0) {
			vp = lastsh;
			vp[0] = adrof(STRshell) ? value(STRshell) : STR_SHELLPATH;
			vp[1] = NOSTR;
#ifdef _PATH_BSHELL
			if (fd != -1 && c != '#')
				vp[0] = STR_BSHELL;
#endif
		} else
			vp = v->vec;
		st0 = st[0];
		st[0] = sf;
		ost = st;
		st = blkspl(vp, st);		/* Splice up the new arglst */
		ost[0] = st0;
		sf = *st;
		/* The order for the conversions is significant */
		t = short2blk(st);
		f = short2str(sf);
		xfree((ptr_t)st);
#ifdef VFORK
		Vt = t;
#endif
		(void) execv(f, t);
#ifdef VFORK
		Vt = 0;
#endif
		blkfree((Char **) t);
		/* The sky is falling, the sky is falling! */

	case ENOMEM:
		Perror(f);

#ifdef _IBMR2
	case 0:		/* execv fails and returns 0! */
#endif /* _IBMR2 */
	case ENOENT:
		break;

	default:
		if (exerr == 0) {
			exerr = strerror();
			if (expath)
				xfree((ptr_t)expath);
			expath = Strsave(sf);
#ifdef VFORK
			Vexpath = expath;
#endif
		}
	}
}

/*ARGSUSED*/
void
execash(t, kp)
	char **t;
	register struct command *kp;
{
	if (chkstop == 0 && setintr)
		panystop(0);
#ifndef FIOCLEX
	didcch = 1;
#endif
	rechist();
	(void) signal(SIGINT, parintr);
	(void) signal(SIGQUIT, parintr);
	(void) signal(SIGTERM, parterm);	/* if doexec loses, screw */
	lshift(kp->t_dcom, 1);
	exiterr = 1;
	doexec(kp);
	/*NOTREACHED*/
}

void
xechoit(t)
	Char **t;
{

	if (adrof(STRecho)) {
		flush();
		haderr = 1;
		blkpr(t), CSHputchar('\n');
		haderr = 0;
	}
}

/*VARARGS0*/
void
dohash()
{
#ifdef COMMENT 
	struct stat stb;
#endif
	DIR *dirp;
	register struct dirent *dp;
	register int cnt;
	int i = 0;
	struct varent *v = adrof(STRpath);
	Char **pv;
	int hashval;

	tw_clear_comm_list();
	havhash = 1;
	for (cnt = 0; cnt < sizeof xhash; cnt++)
		xhash[cnt] = 0;
	if (v == 0)
		return;
	for (pv = v->vec; *pv; pv++, i++) {
		if (pv[0][0] != '/')
			continue;
		dirp = opendir(short2str(*pv));
		if (dirp == NULL)
			continue;
#ifdef COMMENT	/* this isn't needed.  opendir won't open non-dirs */
		if (fstat(dirp->dd_fd, &stb) < 0 || !isdir(stb)) {
			(void) closedir(dirp);
			continue;
		}
#endif
		while ((dp = readdir(dirp)) != NULL) {
			if (dp->d_ino == 0)
				continue;
			if (dp->d_name[0] == '.' &&
			    (dp->d_name[1] == '\0' ||
			     dp->d_name[1] == '.' && dp->d_name[2] == '\0'))
				continue;
			hashval = hash(hashname(str2short(dp->d_name)), i);
			bis(xhash, hashval);
			/* tw_add_comm_name (dp->d_name); */
		}
		(void) closedir(dirp);
	}
}

void
dounhash()
{

	havhash = 0;
}

#ifdef VFORK
void
hashstat()
{

	if (hits+misses)
		CSHprintf("%d hits, %d misses, %d%%\n",
			hits, misses, 100 * hits / (hits + misses));
}
#endif

/*
 * Hash a command name.
 */
static int
hashname(cp)
	register Char *cp;
{
	register long h = 0;

	while (*cp)
		h = hash(h, *cp++);
	return ((int) h);
}

int
iscommand(name)
	Char *name;
{
	register Char **pv;
	register Char *sav;
	register struct varent *v;
	register bool slash = any(short2str(name), '/');
	register int hashval = 0, hashval1, i;

	v = adrof(STRpath);
	if (v == 0 || v->vec[0] == 0 || slash)
		pv = justabs;
	else
		pv = v->vec;
	sav = Strspl(STRslash, name);	/* / command name for postpending */
	if (havhash)
		hashval = hashname(name);
	i = 0;
	do {
		if (!slash && pv[0][0] == '/' && havhash) {
			hashval1 = hash(hashval, i);
			if (!bit(xhash, hashval1))
				goto cont;
		}
		if (pv[0][0] == 0 || eq(pv[0], STRdot)) {/* don't make ./xxx */
			if (executable((Char *)0, name, 0)) {
				xfree((ptr_t) sav);
				return i+1;
			}
		} else {
			if (executable(*pv, sav, 0)) {
				xfree((ptr_t) sav);
				return i+1;
			}
		}
cont:
		pv++;
		i++;
	} while (*pv);
	xfree((ptr_t) sav);
	return 0;
}

/* Also by:
 *  Andreas Luik <luik@isaak.isa.de>
 *  I S A  GmbH - Informationssysteme fuer computerintegrierte Automatisierung
 *  Azenberstr. 35
 *  D-7000 Stuttgart 1
 *  West-Germany
 * is the executable() routine below and changes to iscommand().
 * Thanks again!!
 */

#ifndef S_IXOTH
#define S_IXOTH (S_IEXEC >> 6)
#define S_IXGRP (S_IEXEC >> 3)
#define S_IXUSR S_IEXEC
#endif

/*
 * executable() examines the pathname obtained by concatenating dir and name
 * (dir may be NULL), and returns 1 either if it is executable by us, or
 * if dir_ok is set and the pathname refers to a directory.
 * This is a bit kludgy, but in the name of optimization...
 */
int
executable(dir, name, dir_ok)
Char    *dir, *name;
bool	dir_ok;
{
    struct stat     stbuf;
    Char path[MAXPATHLEN+1];
    char *strname;

    if (dir && *dir) {
	copyn(path, dir, MAXPATHLEN);
	catn(path, name, MAXPATHLEN);
	strname = short2str(path);
    } else
	strname = short2str(name);
    return (stat (strname, &stbuf) != -1 &&
	    (((stbuf.st_mode & S_IFMT) == S_IFREG &&
	      /* save time by not calling access() in the hopeless case */
	      (stbuf.st_mode & (S_IXOTH|S_IXGRP|S_IXUSR)) &&
	      access (strname, 1) == 0) ||
	     (dir_ok && (stbuf.st_mode & S_IFMT) == S_IFDIR)));
}

void
tellmewhat(lex)
	struct wordent *lex;
{
    register int i;
    register struct biltins *bptr;
    register struct wordent *sp = lex->next;
    bool aliased = 0;
    Char *s0, *s1, *s2;
    Char qc;

    if (adrof1(sp->word, &aliases)) {
	alias(lex);
	sp = lex->next;
	aliased = 1;
    }

    s0 = sp->word;	/* to get the memory freeing right... */

    /* handle quoted alias hack */
    if ((*(sp->word) & (QUOTE|TRIM)) == QUOTE)
	(sp->word)++;

    /* do quoting, if it hasn't been done */
    s1 = s2 = sp->word;
    while (*s2) 
	switch (*s2) {
	case '\'':
	case '"':
	    qc = *s2++;
	    while (*s2 && *s2 != qc)
		*s1++ = *s2++ | QUOTE;
	    if (*s2)
		s2++;
	    break;
	case '\\':
	    if (*++s2)
		*s1++ = *s2++ | QUOTE;
	    break;
	default:
	    *s1++ = *s2++;
	}
    *s1 = '\0';

    for (bptr = bfunc; bptr < &bfunc[nbfunc]; bptr++) {
	if (eq(sp->word, str2short(bptr->bname))) {
	    if (aliased) prlex(lex);
	    CSHprintf("%s: shell built-in command.\n", short2str(sp->word));
	    flush();
	    sp->word = s0;	/* we save and then restore this */
	    return;
	}
    }

    if (i = iscommand(strip(sp->word))) {
	register Char **pv;
	register struct varent *v;
	bool slash = any(short2str(sp->word), '/');

	v = adrof(STRpath);
	if (v == 0 || v->vec[0] == 0 || slash)
	    pv = justabs;
	else
	    pv = v->vec;

	while (--i) pv++;
	if (pv[0][0] == 0 || eq(pv[0], STRdot)) {
	    sp->word = Strspl(STRdotsl, sp->word);
	    prlex(lex);
	    xfree((ptr_t) sp->word);
	    sp->word = s0;	/* we save and then restore this */
	    return;
	}
	s1 = Strspl(*pv, STRslash);
	sp->word = Strspl(s1, sp->word);
	xfree((ptr_t) s1);
	prlex(lex);
	xfree((ptr_t) sp->word);
    } else {
	if (aliased) prlex(lex);
	CSHprintf("%s: Command not found.\n", short2str(sp->word)); 
	flush();
    }
    sp->word = s0;	/* we save and then restore this */
}

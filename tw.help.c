/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/tw.help.c,v 1.4 1990/11/29 19:40:28 christos Exp $ */
/* tw.help.c: actually look up and print documentation on a file.  
 *	      Look down the path for an appropriate file, then print it.  
 *	      Note that the printing is NOT PAGED.  This is because the 
 *	      function is NOT meant to look at manual pages, it only does so
 *	      if there is no .help file to look in. 
 */
#include "config.h"

#ifndef lint
static char *rcsid = "$Id: tw.help.c,v 1.4 1990/11/29 19:40:28 christos Exp $";
#endif

#include "sh.h"
#include "tw.h"


static int f = -1;
static sigret_t cleanf();		/* forward ref */
static Char * skipslist();
static void nextslist();

void
do_help (command)
Char *command;
{
    Char name[FILSIZ + 1];
    Char *cmd_p;
    sigret_t (*orig_intr)();
    Char curdir[128];		/* Current directory being looked at */
    register Char *hpath;	/* The environment parameter */
    Char *skipslist();
    Char full[128], buf[512];	/* full path name and buffer for read */
    int len;			/* length of read buffer */
    Char *thpath;


				/* copy the string to a safe place */
    copyn (name, command, FILSIZ + 1);

			/* trim off the garbage that may be at the end */
    for (cmd_p = name; *cmd_p != '\0'; cmd_p++) {
	if (*cmd_p == ' ' || *cmd_p == '\t') *cmd_p = '\0';
    }

				/* if nothing left, return */
    if (*name == '\0') return;

    if (adrof1 (STRhelpcommand, &aliases)) { /* if we have an alias */
	jmp_buf osetexit;

	getexit(osetexit);	/* make sure to come back here */
	if (setexit() == 0)
	    aliasrun (2, STRhelpcommand, name); /* then use it. */
	resexit(osetexit);	/* and finish up */
    } else {			/* else cat something to them */
		/* got is, now "cat" the file based on the path $HPATH */

	hpath = str2short(getenv(SEARCHLIST));
	if(hpath == (Char *)0)
	    hpath = str2short(DEFAULTLIST);
	thpath = hpath = Strsave(hpath);

	while (1)
	    {
		if(!*hpath)
		    {
			CSHprintf("No help file for %s\n", name);
			break;
		    }
		nextslist(hpath, curdir);
		hpath = skipslist(hpath);

/* now make the full path name - try first /bar/foo.help, then /bar/foo.1,
   /bar/foo.8, then finally /bar/foo.6 .  This is so that you don't spit
   a binary at the tty when $HPATH == $PATH.  I know, I know, gotos are
   BOGUS */

		copyn (full, curdir, sizeof (full) / sizeof(Char));
		catn (full, STRslash, sizeof (full) / sizeof(Char));
		catn (full, name, sizeof (full) / sizeof(Char));
		catn (full, STRdothelp, sizeof (full) / sizeof(Char));
		f = open (short2str(full), 0);	/* try to open for reading */
		if (f != -1) goto cat_it;	/* no file there */

		copyn (full, curdir, sizeof (full) / sizeof(Char));
		catn (full, STRslash, sizeof (full) / sizeof(Char));
		catn (full, name, sizeof (full) / sizeof(Char));
		catn (full, STRdot1, sizeof (full) / sizeof(Char));
		f = open (short2str(full), 0);	/* try to open for reading */
		if (f != -1) goto cat_it;	/* no file there */

		copyn (full, curdir, sizeof (full) / sizeof(Char));
		catn (full, STRslash, sizeof (full) / sizeof(Char));
		catn (full, name, sizeof (full) / sizeof(Char));
		catn (full, STRdot8, sizeof (full) / sizeof(Char));
		f = open (short2str(full), 0);	/* try to open for reading */
		if (f != -1) goto cat_it;	/* no file there */

		copyn (full, curdir, sizeof (full) / sizeof(Char));
		catn (full, STRslash, sizeof (full) / sizeof(Char));
		catn (full, name, sizeof (full) / sizeof(Char));
		catn (full, STRdot6, sizeof (full) / sizeof(Char));
		f = open (short2str(full), 0);	/* try to open for reading */
		if (f == -1) continue;	/* no file there */

cat_it:
		orig_intr = signal(SIGINT, cleanf);
		while ((f != -1) && (len = read (f, (char *) buf, 512)) != 0) {
					     /* the file is here */
		    (void) write (SHOUT, (char *) buf, (size_t) len); 
						/* so cat it to the terminal */
		}
		if (f != -1)
		    (void) close(f);
		f = (-1);
		(void) signal(SIGINT, orig_intr);
		break;
	    }
	    xfree((ptr_t) thpath);
    }
}

static sigret_t
cleanf()
{
    if (f != -1)
	(void) close(f);
    f = (-1);
#ifndef SIGVOID
    return(0);
#endif
}

/* these next two are stolen from CMU's man(1) command for looking down 
 * paths.  they are prety straight forward. */

/*
 * nextslist takes a search list and copies the next path in it
 * to np.  A null search list entry is expanded to ".".
 * If there are no entries in the search list, then np will point
 * to a null string.
 */

static void
nextslist(sl, np)
  register Char *sl;
  register Char *np;
{
    if(!*sl)
	*np = '\000';
    else if(*sl == ':')
    {
	*np++ = '.';
	*np = '\000';
    }
    else
    {
	while(*sl && *sl != ':')
	    *np++ = *sl++;
	*np = '\000';
    }
}

/*
 * skipslist returns the pointer to the next entry in the search list.
 */

static Char *
skipslist(sl)
  register Char *sl;
{
    while(*sl && *sl++ != ':');
    return(sl);
}


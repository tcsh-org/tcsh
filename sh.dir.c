/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.dir.c,v 2.0 1991/03/26 02:59:29 christos Exp $ */
/*
 * sh.dir.c: Directory manipulation functions
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
static char *sccsid = "@(#)sh.dir.c	5.3 (Berkeley) 6/11/85";
#endif
#ifndef lint
static char *rcsid = "$Id: sh.dir.c,v 2.0 1991/03/26 02:59:29 christos Exp $";
#endif 


#include "sh.h"
#include "sh.dir.h"
/*
 * C Shell - directory management
 */

static struct	directory *dfind();
static Char	*dfollow();
static void 	printdirs();
static Char	*dgoto();
static void 	dnewcwd();

struct	directory dhead;		/* "head" of loop */
int	printd;				/* force name to be printed */
#ifdef CSHDIRS
int bequiet = 0;			/* do not print dir stack -strike */
#endif
static int dirflag = 0;

/*
 * dinit - initialize current working directory
 */
void
dinit(hp)
	Char *hp;
{
	register char *tcp;
	register Char *cp;
	register struct directory *dp;
	char path[MAXPATHLEN];
	static char *emsg = "tcsh: Trying to start from \"%s\"\n";

	/* Don't believe the login shell home, because it may be a symlink */
	tcp = getwd(path); /* see ngetwd.c for System V version */
	if (tcp == NULL || *tcp == '\0') {
	    (void) CSHprintf("tcsh: %s\n", path);
	    if (hp && *hp) {
		tcp = short2str(hp);
		(void) CSHprintf(emsg, tcp);
		if (chdir(tcp) == -1) 
		    cp = (Char *) 0;
		else
		    cp = hp;
	    }
	    else
		cp = (Char *) 0;
	    if (cp == (Char *) 0) {
		(void) CSHprintf(emsg, "/");
		if (chdir("/") == -1) 
		    /* I am not even try to print an error message! */
		    xexit(1);
		cp = SAVE("/");
	    }
	} else {
# ifdef S_IFLNK
	    struct stat swd, shp;
	    /*
	     * See if $HOME is the working directory we got and use that
	     */
	    if (hp && *hp &&
		stat(tcp, &swd) != -1 && stat(short2str(hp), &shp) != -1 &&
		swd.st_dev == shp.st_dev && swd.st_ino == shp.st_ino)
		cp = hp;
	    else
	    {
		char *cwd;
		/*
		 * use PWD if we have it (for subshells)
		 */
		if (cwd = getenv ("PWD"))
		{
		    if (stat(cwd, &shp) != -1 && swd.st_dev == shp.st_dev &&
			swd.st_ino == shp.st_ino)
			tcp = cwd;
		}
		cp = dcanon (str2short(tcp), STRNULL);
	    }
# else /* S_IFLNK */
	    cp = dcanon (str2short(tcp), STRNULL);
# endif /* S_IFLNK */
	}

	dp = (struct directory *)calloc(sizeof (struct directory), 1);
	dp->di_name = Strsave(cp);
	dp->di_count = 0;
	dhead.di_next = dhead.di_prev = dp;
	dp->di_next = dp->di_prev = &dhead;
	printd = 0;
	dnewcwd(dp);
}

#define DIR_LONG 1
#define DIR_VERT 2
#define DIR_LINE 4

static void
skipargs(v, str)
Char ***v;
char *str;
{
    Char **n = *v, *s;
    char errstring[100];

    dirflag = 0;
    for (n++; *n != NOSTR && (*n)[0] == '-'; n++)
	for (s = &((*n)[1]); *s; s++)
	    switch (*s) {
	    case 'l':
		dirflag |= DIR_LONG;
		break;
	    case 'v':
		dirflag |= DIR_VERT;
		break;
	    case 'n':
		dirflag |= DIR_LINE;
		break;
	    default:
		CSHsprintf(errstring, 
			   "Usage: %s [-lvn]%s", short2str(**v), str);
		error(errstring, NULL);
		break;
	    }
    *v = n;
}

/*
 * dodirs - list all directories in directory loop
 */
void
dodirs(v)
	Char **v;
{
	skipargs(&v, "");

	if (*v != NOSTR)
	    error("Usage: dirs [-lvn]");
	printdirs();
}

static void
printdirs()
{
	register struct directory *dp;
	Char *s, *hp = value(STRhome);
	int idx, len, cur;
	extern int T_Cols;

	if (*hp == '\0')
		hp = NOSTR;
	dp = dcwd;
	idx = 0;
	cur = 0;
	do {
		if (dp == &dhead)
			continue;
		if (dirflag & DIR_VERT) {
			CSHprintf("%d\t", idx++);
			cur = 0;
		}
		if (!(dirflag & DIR_LONG) && hp != NOSTR && !eq(hp, STRslash) && 
		    prefix(hp, dp->di_name))
		       len = Strlen(s = (dp->di_name + Strlen(hp))) + 2;
		else
		       len = Strlen(s = dp->di_name) + 1;

		cur += len;
		if ((dirflag & DIR_LINE) && cur >= T_Cols - 1 && len < T_Cols) {
			CSHprintf("\n");
			cur = len;
		}
		CSHprintf(s != dp->di_name ? "~%s%c" : "%s%c", 
			  short2str(s), (dirflag & DIR_VERT) ? '\n' : ' ');
	} while ((dp = dp->di_prev) != dcwd);
	if (!(dirflag & DIR_VERT))
	    CSHprintf("\n");
}

void
dtildepr(home, dir)
	register Char *home, *dir;
{

	if (!eq(home, STRslash) && prefix(home, dir))
		CSHprintf("~%s", short2str(dir + Strlen(home)));
	else
		CSHprintf("%s", short2str(dir));
}

void
dtilde()
{
    struct directory *d = dcwd;

    do
    {
	if (d == &dhead)
	    continue;
	d->di_name = dcanon (d->di_name, STRNULL);
    } while ((d = d->di_prev) != dcwd);

    set(STRcwd, Strsave(dcwd->di_name));
}

/*
 * dochngd - implement chdir command.
 */
void
dochngd(v)
	Char **v;
{
	register Char *cp;
	register struct directory *dp;

	skipargs(&v, " [<dir>]");
	printd = 0;
	if (*v == NOSTR) {
		if ((cp = value(STRhome)) == NOSTR || *cp == 0)
			stdbferr(ERR_NOHOMEDIR);
		if (chdir(short2str(cp)) < 0)
			stdbferr(ERR_CANTCHANGE);
		cp = Strsave(cp);
	} 
	else if (v[1] != NOSTR) {
		stdbferr(ERR_TOOMANY);
		/* NOTREACHED */
		return;
	}
	else if ((dp = dfind(*v)) != 0) {
		printd = 1;
		if (chdir(short2str(dp->di_name)) < 0)
			Perror(short2str(dp->di_name));
		dcwd->di_prev->di_next = dcwd->di_next;
		dcwd->di_next->di_prev = dcwd->di_prev;
		dfree(dcwd);
		dnewcwd(dp);
		return;
	} else
		cp = dfollow(*v);
	dp = (struct directory *)calloc(sizeof (struct directory), 1);
	dp->di_name = cp;
	dp->di_count = 0;
	dp->di_next = dcwd->di_next;
	dp->di_prev = dcwd->di_prev;
	dp->di_prev->di_next = dp;
	dp->di_next->di_prev = dp;
	dfree(dcwd);
	dnewcwd(dp);
}

static Char *
dgoto(cp)
Char *cp;
{
        Char *dp;
	if (*cp != '/') {
		register Char *p, *q;
		int cwdlen;

		/*
		 * All in the name of efficiency?
		 */
		for (p = dcwd->di_name; *p++;)	/* Strlen(dcwd->di_name); */
			;
		if ((cwdlen = p - dcwd->di_name - 1) == 1)	/* root */
			cwdlen = 0;
		for (p = cp; *p++;)
			;
		dp = (Char *) xalloc((size_t) ((cwdlen + (p - cp) + 1) *
				     sizeof(Char)));
		for (p = dp, q = dcwd->di_name; *p++ = *q++;)
			;
		if (cwdlen)
			p[-1] = '/';
		else
			p--;			/* don't add a / after root */
		for (q = cp; *p++ = *q++;)
			;
		xfree((ptr_t) cp);
		cp = dp;
		dp += cwdlen;
	} else
		dp = cp;

	cp = dcanon(cp, dp);
	/*
	 * Not a good place to do that
	 */
	if (adrof(STRignore_symlinks))
	    (void) chdir(short2str(cp));
	return cp;
}

/*
 * dfollow - change to arg directory; fall back on cdpath if not valid
 */
static Char *
dfollow(cp)
	register Char *cp;
{
	register Char *dp;
	struct varent *c;
	char ebuf[MAXPATHLEN];

	cp = globone(cp, G_ERROR);
	/*
	 * This is not a good test if we are ignoring symlinks
	 */
	if (chdir(short2str(cp)) >= 0)
		return dgoto(cp);
	if (cp[0] != '/' && !prefix(STRdotsl, cp) && !prefix(STRdotdotsl, cp)
	    && (c = adrof(STRcdpath))) {
		Char **cdp;
		register Char *p;
		Char buf[MAXPATHLEN];

		for (cdp = c->vec; *cdp; cdp++) {
			for (dp = buf, p = *cdp; *dp++ = *p++;)
				;
			dp[-1] = '/';
			for (p = cp; *dp++ = *p++;)
				;
			if (chdir(short2str(buf)) >= 0) {
				printd = 1;
				xfree((ptr_t) cp);
				cp = Strsave(buf);
				return dgoto(cp);
			}
		}
	}
	dp = value(cp);
	if ((dp[0] == '/' || dp[0] == '.') && chdir(short2str(dp)) >= 0) {
		xfree((ptr_t) cp);
		cp = Strsave(dp);
		printd = 1;
		return dgoto(cp);
	}
	(void) strcpy(ebuf, short2str(cp));
	xfree((ptr_t) cp);
#ifdef CSHDIRS
        /*
         * on login source of ~/.cshdirs, errors
         * are eaten. the dir stack is all directories
         * we could get to.
         */
        if (!bequiet)
                Perror(ebuf);
        else
                return(NOSTR);
#else
	Perror(buf);
#endif
	/*NOTREACHED*/
	return(NOSTR);
}


/*
 * dopushd - push new directory onto directory stack.
 *	with no arguments exchange top and second.
 *	with numeric argument (+n) bring it to top.
 */
void
dopushd(v)
	Char **v;
{
	register struct directory *dp;
	register Char *cp;

	skipargs(&v, " [<dir>|+<n>]");
	printd = 1;
	if (*v == NOSTR) {
	    if (adrof (STRpushdtohome)) {
		if ((cp = value(STRhome)) == NOSTR || *cp == 0)
			stdbferr(ERR_NOHOMEDIR);
		if (chdir(short2str(cp)) < 0)
			stdbferr(ERR_CANTCHANGE);
		cp = Strsave(cp); /* hmmm... PWP */
		cp = dfollow(cp);
		dp = (struct directory *)calloc(sizeof (struct directory), 1);
		dp->di_name = cp;
		dp->di_count = 0;
		dp->di_prev = dcwd;
		dp->di_next = dcwd->di_next;
		dcwd->di_next = dp;
		dp->di_next->di_prev = dp;
	    } else {
		if ((dp = dcwd->di_prev) == &dhead)
			dp = dhead.di_prev;
		if (dp == dcwd)
			bferr("No other directory");
		if (chdir(short2str(dp->di_name)) < 0)
			Perror(short2str(dp->di_name));
		dp->di_prev->di_next = dp->di_next;
		dp->di_next->di_prev = dp->di_prev;
		dp->di_next = dcwd->di_next;
		dp->di_prev = dcwd;
		dcwd->di_next->di_prev = dp;
		dcwd->di_next = dp;
	    }
	} else if (v[1] != NOSTR) {
		stdbferr(ERR_TOOMANY);
		/* NOTREACHED */
		return;
	}
	else if (dp = dfind(*v)) {
		    if (chdir(short2str(dp->di_name)) < 0)
			Perror(short2str(dp->di_name));
		/*
		 * kfk - 10 Feb 1984 - added new "extraction style" pushd +n
		 */
		if (adrof (STRdextract))
			dextract (dp);
	} else {
		register Char *ccp;

		ccp = dfollow(*v);
		dp = (struct directory *)calloc(sizeof (struct directory), 1);
		dp->di_name = ccp;
		dp->di_count = 0;
		dp->di_prev = dcwd;
		dp->di_next = dcwd->di_next;
		dcwd->di_next = dp;
		dp->di_next->di_prev = dp;
	}
	dnewcwd(dp);
}

/*
 * dfind - find a directory if specified by numeric (+n) argument
 */
static struct directory *
dfind(cp)
	register Char *cp;
{
	register struct directory *dp;
	register int i;
	register Char *ep;

	if (*cp++ != '+')
		return (0);
	for (ep = cp; isdigit(*ep); ep++)
		continue;
	if (*ep)
		return (0);
	i = getn(cp);
	if (i <= 0)
		return (0);
	for (dp = dcwd; i != 0; i--) {
		if ((dp = dp->di_prev) == &dhead)
			dp = dp->di_prev;
		if (dp == dcwd)
			bferr("Directory stack not that deep");
	}
	return (dp);
}

/*
 * dopopd - pop a directory out of the directory stack
 *	with a numeric argument just discard it.
 */
void
dopopd(v)
	Char **v;
{
	register struct directory *dp, *p = NULL;

	skipargs(&v, " [+<n>]");
	printd = 1;
	if (*v == NOSTR)
		dp = dcwd;
	else if (v[1] != NOSTR) {
		stdbferr(ERR_TOOMANY);
		/* NOTREACHED */
		return;
	}
	else if ((dp = dfind(*v)) == 0)
		bferr("Bad directory");
	if (dp->di_prev == &dhead && dp->di_next == &dhead)
		bferr("Directory stack empty");
	if (dp == dcwd) {
		if ((p = dp->di_prev) == &dhead)
			p = dhead.di_prev;
		if (chdir(short2str(p->di_name)) < 0)
			Perror(short2str(p->di_name));
	}
	dp->di_prev->di_next = dp->di_next;
	dp->di_next->di_prev = dp->di_prev;
	if (dp == dcwd)
		dnewcwd(p);
	else {
		printdirs();
	}
	dfree(dp);
}

/*
 * dfree - free the directory (or keep it if it still has ref count)
 */
void
dfree(dp)
	register struct directory *dp;
{

	if (dp->di_count != 0) {
		dp->di_next = dp->di_prev = 0;
	} else {
		xfree((char *) dp->di_name); xfree((ptr_t) dp);
	}
}

/*
 * dcanon - canonicalize the pathname, removing excess ./ and ../ etc.
 *	we are of course assuming that the file system is standardly
 *	constructed (always have ..'s, directories have links)
 */
Char *
dcanon(cp, p)
	register Char *cp, *p;
{
	register Char *sp;
	register Char *p1, *p2;		/* general purpose */
	bool slash;
#ifdef S_IFLNK	/* if we have symlinks */
	Char link[MAXPATHLEN];
	char tlink[MAXPATHLEN];
	int cc;
	Char *newcp;
#endif /* S_IFLNK */

	/*
	 * christos: if the path given does not start with
	 *           a slash prepend cwd. If cwd does not
	 *           start with a path or the result would
	 *           be too long abort().
	 */
	if (*cp != '/')  {
	    Char tmpdir[MAXPATHLEN];
	    p1 = value(STRcwd);
	    if (p1 == (Char *) 0 || *p1 != '/')
		abort();
	    if (Strlen(p1) + Strlen(cp) + 1 >= MAXPATHLEN)
		abort();
	    (void) Strcpy(tmpdir, p1);
	    (void) Strcat(tmpdir, STRslash);
	    (void) Strcat(tmpdir, cp);
	    xfree((ptr_t) cp);
	    cp = p = Strsave(tmpdir);
	}

#ifdef COMMENT
	if (*cp != '/')
		abort();
#endif

	while (*p) {			/* for each component */
		sp = p;			/* save slash address */
		while (*++p == '/')	/* flush extra slashes */
			;
		if (p != ++sp)
			for (p1 = sp, p2 = p; *p1++ = *p2++;)
				;
		p = sp;			/* save start of component */
		slash = 0;
		while (*++p)		/* find next slash or end of path */
			if (*p == '/') {
				slash = 1;
				*p = 0;
				break;
			}

		if (*sp == '\0')	/* if component is null */
			if (--sp == cp)	/* if path is one char (i.e. /) */
				break;
			else
				*sp = '\0';
		else if (sp[0] == '.' && sp[1] == 0) {
			if (slash) {
				for (p1 = sp, p2 = p + 1; *p1++ = *p2++;)
					;
				p = --sp;
			} else if (--sp != cp)
				*sp = '\0';
		} else if (sp[0] == '.' && sp[1] == '.' && sp[2] == 0) {
			/*
			 * We have something like "yyy/xxx/..", where "yyy"
			 * can be null or a path starting at /, and "xxx"
			 * is a single component.
			 * Before compressing "xxx/..", we want to expand
			 * "yyy/xxx", if it is a symbolic link.
			 */
			*--sp = 0;	/* form the pathname for readlink */
#ifdef S_IFLNK	/* if we have symlinks */
			if (sp != cp && !adrof(STRignore_symlinks) &&
			    (cc = readlink(short2str(cp), tlink, 
					   sizeof tlink)) >= 0) {
				(void) Strcpy(link, str2short(tlink));
				link[cc] = '\0';

				if (slash)
					*p = '/';
				/*
				 * Point p to the '/' in "/..", and restore
				 * the '/'.
				 */
				*(p = sp) = '/';
				/*
				 * find length of p
				 */
				for (p1 = p; *p1++;)
					;
				if (*link != '/') {
					/*
					 * Relative path, expand it between
					 * the "yyy/" and the "/..".
					 * First, back sp up to the character
					 * past "yyy/".
					 */
					while (*--sp != '/')
						;
					sp++;
					*sp = 0;
					/*
					 * New length is
					 * "yyy/" + link + "/.." and rest
					 */
					p1 = newcp = (Char *) xalloc((size_t)
						(((sp - cp) + cc + (p1 - p)) *
						sizeof(Char)));
					/*
					 * Copy new path into newcp
					 */
					for (p2 = cp; *p1++ = *p2++;)
						;
					for (p1--, p2 = link; *p1++ = *p2++;)
						;
					for (p1--, p2 = p; *p1++ = *p2++;)
						;
					/*
					 * Restart canonicalization at
					 * expanded "/xxx".
					 */
					p = sp - cp - 1 + newcp;
				} else {
					/*
					 * New length is link + "/.." and rest
					 */
					p1 = newcp = (Char *) xalloc((size_t)
					    ((cc + (p1 - p)) * sizeof(Char)));
					/*
					 * Copy new path into newcp
					 */
					for (p2 = link; *p1++ = *p2++;)
						;
					for (p1--, p2 = p; *p1++ = *p2++;)
						;
					/*
					 * Restart canonicalization at beginning
					 */
					p = newcp;
				}
				xfree((ptr_t) cp);
				cp = newcp;
				continue;	/* canonicalize the link */
			}
#endif /* S_IFLNK */
			*sp = '/';
			if (sp != cp)
				while (*--sp != '/')
					;
			if (slash) {
				for (p1 = sp + 1, p2 = p + 1; *p1++ = *p2++;)
					;
				p = sp;
			} else if (cp == sp)
				*++sp = '\0';
			else
				*sp = '\0';
		} else {	/* normal dir name (not . or .. or nothing) */

#ifdef S_IFLNK	/* if we have symlinks */
		    if (sp != cp && adrof(STRchase_symlinks) &&
			!adrof(STRignore_symlinks) &&
			(cc = readlink(short2str(cp), tlink, 
				       sizeof tlink)) >= 0) {
			(void) Strcpy(link, str2short(tlink));
			link[cc] = '\0';

			/*
			 * restore the '/'.
			 */
			if (slash)
			    *p = '/';

			/*
			 * point sp to p (rather than backing up).
			 */
			sp = p;

			/*
			 * find length of p
			 */
			for (p1 = p; *p1++;)
			    ;
			if (*link != '/') {
			    /*
			     * Relative path, expand it between
			     * the "yyy/" and the remainder.
			     * First, back sp up to the character
			     * past "yyy/".
			     */
			    while (*--sp != '/')
				;
			    sp++;
			    *sp = 0;
			    /*
			     * New length is
			     * "yyy/" + link + "/.." and rest
			     */
			    p1 = newcp = (Char *) xalloc((size_t)
						(((sp - cp) + cc + (p1 - p))
						* sizeof(Char)));
			    /*
			     * Copy new path into newcp
			     */
			    for (p2 = cp; *p1++ = *p2++;)
				;
			    for (p1--, p2 = link; *p1++ = *p2++;)
				;
			    for (p1--, p2 = p; *p1++ = *p2++;)
				;
			    /*
			     * Restart canonicalization at
			     * expanded "/xxx".
			     */
			    p = sp - cp - 1 + newcp;
			} else {
			    /*
			     * New length is link + the rest
			     */
			    p1 = newcp = (Char *) xalloc((size_t)
					 ((cc + (p1 - p)) * sizeof(Char)));
			    /*
			     * Copy new path into newcp
			     */
			    for (p2 = link; *p1++ = *p2++;)
				;
			    for (p1--, p2 = p; *p1++ = *p2++;)
				;
			    /*
			     * Restart canonicalization at beginning
			     */
			    p = newcp;
			}
			xfree((ptr_t) cp);
			cp = newcp;
			continue;	/* canonicalize the link */
		    }
#endif /* S_IFLNK */

		    if (slash)
			*p = '/';
		}
	}

	/*
	 * fix home...
	 */
#ifdef S_IFLNK
	p1 = value(STRhome);
	cc = Strlen (p1);
	/*
	 * See if we're not in a subdir of STRhome
	 */
	if (p1 && *p1 &&
	    (Strncmp (p1, cp, cc) != 0 || (cp[cc] != '/' && cp[cc] != '\0')))
	{
	    static ino_t home_ino = -1;
	    static dev_t home_dev = -1;
	    static Char *home_ptr = (Char *) 0;
	    struct stat statbuf;

	    /*
	     * Get dev and ino of STRhome
	     */
	    if (home_ptr != p1 &&
		stat (short2str(p1), &statbuf) != -1)
	    {
		home_dev = statbuf.st_dev;
		home_ino = statbuf.st_ino;
		home_ptr = p1;
	    }
	    /*
	     * Start comparing dev & ino backwards
	     */
	    p2 = Strcpy (link, cp);
	    for (sp = (Char *) 0; *p2 && stat (short2str (p2), &statbuf) != -1;) 
	    {
		if (statbuf.st_dev == home_dev &&
		    statbuf.st_ino == home_ino)
		{
		    sp = (Char *) -1;
		    break;
		}
		if (sp = Strrchr (p2, '/'))
		    *sp = '\0';
	    }
	    /*
	     * See if we found it
	     */
	    if (*p2 && sp == (Char *) -1)
	    {
		/*
		 * Use STRhome to make '~' work
		 */
		p2 = cp + Strlen (p2);
		sp = newcp = (Char *) xalloc((size_t)
			((cc + Strlen(p2)) * sizeof(Char)));
		while (*p1)
		    *sp++ = *p1++;
		while (*p2)
		    *sp++ = *p2++;
		*sp = '\0';
		xfree ((ptr_t) cp);
		cp = newcp;
	    }
	}
#endif /* S_IFLNK */

	return cp;
}


/*
 * dnewcwd - make a new directory in the loop the current one
 */
static void
dnewcwd(dp)
	register struct directory *dp;
{
	dcwd = dp;
	set(STRcwd, Strsave(dcwd->di_name));
	Setenv(STRPWD, Strsave(dcwd->di_name));
	if (printd && !(adrof(STRpushdsilent))     /* PWP: pushdsilent */
#ifdef CSHDIRS
		&& !bequiet	/* be quite while restoring stack -strike */
#endif
						) 
		printdirs();
	cwd_cmd();		/* PWP: run the defined cwd command */
}

/*
 * getstakd - added by kfk 17 Jan 1984
 * Support routine for the stack hack.  Finds nth directory in
 * the directory stack, or finds last directory in stack.
 */
void
getstakd (s, cnt, callerr)
	Char *s;
	int cnt, callerr;
{
	struct directory *dp;

	dp = dcwd;
	if (cnt < 0) {		/* < 0 ==> last dir requested. */
		dp = dp->di_next;
		if (dp == &dhead)
			dp = dp->di_next;
	} else {
		while (cnt-- > 0) {
			dp = dp->di_prev;
			if (dp == &dhead)
				dp = dp->di_prev;
			if (dp == dcwd) {
				if (callerr)
					error ("Not that many dir stack entries");
				else
					return;
			}
		}
	}
	(void) Strcpy (s, dp->di_name);
}

/*
 * Karl Kleinpaste - 10 Feb 1984
 * Added dextract(), which is used in pushd +n.
 * Instead of just rotating the entire stack around, dextract()
 * lets the user have the nth dir extracted from its current
 * position, and pushes it onto the top.
 */
void
dextract(dp)
struct directory *dp;
{
	if (dp == dcwd)
		return;
	dp->di_next->di_prev = dp->di_prev;
	dp->di_prev->di_next = dp->di_next;
	dp->di_next = dcwd->di_next;
	dp->di_prev = dcwd;
	dp->di_next->di_prev = dp;
	dcwd->di_next = dp;
}

#ifdef CSHDIRS
/*
 * create a file called ~/.cshdirs which has a sequence
 * of pushd commands which will restore the dir stack to
 * its state before exit/logout. remember that the order
 * is reversed in the file because we are pushing.
 * -strike
 */
void
recdirs()
{
	int fp, ftmp, oldidfds;
	int cdflag = 0;
	extern int fast;
	Char buf[BUFSIZ];

	if (!fast) {
	        if (!adrof(STRsavedirs))         /* does it exist */
	                return;
	        (void) Strcpy(buf, value(STRhome));
	        (void) Strcat(buf, STRsldtdirs);
	        if ((fp = creat(short2str(buf), 0666)) == -1)
	                return;
	        oldidfds = didfds;
	        didfds = 0;
	        ftmp = SHOUT;
	        SHOUT = fp;
	        {
	                extern struct directory dhead;
	                extern struct directory *dcwd;
	                struct directory *dp = dcwd -> di_next;
	                do {
	                        if (dp == &dhead)
	                                continue;
	                        if (cdflag == 0)
	                            cdflag++, CSHprintf("cd %s\n",
						    short2str(dp -> di_name));
	                        else
	                            CSHprintf("pushd %s\n", 
						    short2str(dp -> di_name));
	                } while ((dp = dp -> di_next) != dcwd -> di_next);
	        }
	        CSHprintf("dirs\n"); /* show the dir stack */

	        (void) close(fp);
	        SHOUT = ftmp;
	        didfds = oldidfds;
	}
}
#endif

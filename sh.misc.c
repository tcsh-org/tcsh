/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.misc.c,v 2.0 1991/03/26 02:59:29 christos Exp christos $ */
/*
 * sh.misc.c: Miscelaneous functions
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
static char *sccsid = "@(#)sh.misc.c	5.3 (Berkeley) 3/29/86";
#endif
#ifndef lint
static char *rcsid = "$Id: sh.misc.c,v 2.0 1991/03/26 02:59:29 christos Exp christos $";
#endif

#include "sh.h"

#if defined(BSD) || SVID == 0
#define DUP2	/* XXX: Should move that to config */
#endif
static int renum();

/*
 * C Shell
 */

int
any(s, c)
	register char *s;
	register int c;
{
    /* Bugfix (for Sun csh) from Sven-Ove Westberg <sow@cad.luth.se>
       via davide%cadillac.cad.mcc.com@mcc.com (David Eckelkamp).
       PWP: damn VAXen.  Berkeley should have unmapped the VAX
       zero page! */

	if(!s) return(0); 	/*  Check for nil pointer */
	while (*s)
		if (*s++ == c)
			return(1);
	return(0);
}
int
onlyread(cp)
	Char *cp;
{
#ifdef NeXT
	extern void * get_end();
	return (((void *) cp) < get_end());
#else
# if defined(sun) || defined(hpux)
	extern int end;
	return (((int) cp) < end);
# else
	extern char end[];
	return (((char *) cp) < end);
# endif
#endif

}

/* changed 6/04/87 Paul Placeway for use with the faster malloc */
void
xfree(cp)
	ptr_t cp;
{
#ifdef NeXT
	extern void * get_end();
	/* the nmalloc.c free() routine picks up garbage frees
	   and ignores them */
	if (cp && ((void *) cp) >= get_end())
		free(cp);
#else
# if defined(sun) || defined(hpux)
	extern int end;
	/* the nmalloc.c free() routine picks up garbage frees
	   and ignores them */
	if (cp && ((int) cp) >= end /* && cp < (char *) &cp */)
		free(cp);
# else
	extern char end[];
	/* the nmalloc.c free() routine picks up garbage frees
	   and ignores them */
	if (cp && (char *) cp >= end /* && cp < (char *) &cp */)
		free(cp);
# endif
#endif

}

memalign_t 
calloc(i, j)
	size_t i, j;
{
#ifndef lint
	register char *cp;

	i *= j;
	cp = (char *) xalloc((size_t) i);
  	setzero(cp, i);
	return(cp);
#else
    if (i && j)
	return((memalign_t) 0);
    else
	return((memalign_t) 0);
#endif
}

void
setzero(cp, i)
char *cp;
int i;
{
    if (i != 0)
	    do 
		    *cp++ = 0;
	    while(--i);
}

memalign_t
nomem(i)
	size_t i;
{
#ifndef lint
# ifdef debug
	static Char *av[2] = {0, 0};
# endif

	child++;
# ifndef debug
	error("Out of memory");
# else
	showall(av);
	CSHprintf("i=%d: Out of memory\n", i);
	/* chdir("/usr/bill/cshcore"); */
	abort();
# endif
	return 0;		/* fool lint */
#else
	if (i)
	    return((memalign_t) 0);
	else
	    return((memalign_t) 0);
#endif
}

char *
strsave(s)
	register char *s;
{
	char *n;
	register char *p;

	if (s == 0)
		s = "";
	for (p = s; *p++;)
		;
	n = p = (char *) xalloc((size_t)((p - s) * sizeof(char)));
	while (*p++ = *s++)
		;
	return (n);
}

Char **
blkend(up)
	register Char **up;
{

	while (*up)
		up++;
	return (up);
}

 
void
blkpr(av)
	register Char **av;
{

	for (; *av; av++) {
		CSHprintf("%s", short2str(*av));
		if (av[1])
			CSHprintf(" ");
	}
}

int
blklen(av)
	register Char **av;
{
	register int i = 0;

	while (*av++)
		i++;
	return (i);
}

Char **
blkcpy(oav, bv)
	Char **oav;
	register Char **bv;
{
	register Char **av = oav;

	while (*av++ = *bv++)
		continue;
	return (oav);
}

Char **
blkcat(up, vp)
	Char **up, **vp;
{

	(void) blkcpy(blkend(up), vp);
	return (up);
}

void
blkfree(av0)
	Char **av0;
{
	register Char **av = av0;

	if (!av0) return;	/* (PWP) no NULL pointers */
	for (; *av; av++)
		xfree((ptr_t) *av);
	xfree((ptr_t)av0);
}

Char **
saveblk(v)
	register Char **v;
{
	register Char **newv =
		(Char **) calloc((size_t) (blklen(v) + 1), sizeof (Char **));
	Char **onewv = newv;

	while (*v)
		*newv++ = Strsave(*v++);
	return (onewv);
}

#ifndef POSIX
char *
strstr(s, t) 
    register char *s, *t;
{
    do {
	register char *ss = s;
	register char *tt = t;
        do 
            if (*tt == '\0') return (s);
        while (*ss++ == *tt++);
    } while (*s++ != '\0');
    return ((char *) 0);
}
#endif /* POSIX */

#ifndef SHORT_STRINGS
char *
strspl(cp, dp)
	char *cp, *dp;
{
	char *ep;
	register char *p, *q;

	if (!cp) cp = "";	/* (PWP) no NULL strings! */
	if (!dp) dp = "";	/* (PWP) no NULL strings! */
	for (p = cp; *p++;)
		;
	for (q = dp; *q++;)
		;
	ep = (char *) xalloc((size_t)
			     (((p - cp)+(q - dp) - 1) * sizeof(char)));
	for (p = ep, q = cp; *p++ = *q++;)
		;
	for (p--, q = dp; *p++ = *q++;)
		;
	return (ep);
}
#endif

Char **
blkspl(up, vp)
	register Char **up, **vp;
{
	register Char **wp =
		(Char **) calloc((size_t) (blklen(up) + blklen(vp) + 1),
			sizeof (Char **));

	(void) blkcpy(wp, up);
	return (blkcat(wp, vp));
}

Char
lastchr(cp)
	register Char *cp;
{

	if (!cp) return (0);	/* (PWP) no NULL strings! */
	if (!*cp)
		return (0);
	while (cp[1])
		cp++;
	return (*cp);
}

/*
 * This routine is called after an error to close up
 * any units which may have been left open accidentally.
 */
void
closem()
{
	register int f;

#ifdef YPBUGS
	/* suggested by Justin Bur; thanks to Karl Kleinpaste */
	fix_yp_bugs();
#endif
	for (f = 0; f < NOFILE; f++)
		if (f != SHIN && f != SHOUT && f != SHDIAG && f != OLDSTD &&
		    f != FSHTTY)
			(void) close(f);
}

#ifndef FIOCLEX
/*
 * Close files before executing a file.
 * We could be MUCH more intelligent, since (on a version 7 system)
 * we need only close files here during a source, the other
 * shell fd's being in units 16-19 which are closed automatically!
 */
void
closech()
{
	register int f;

	if (didcch)
		return;
	didcch = 1;
	SHIN = 0; SHOUT = 1; SHDIAG = 2; OLDSTD = 0;
        isoutatty = isatty(SHOUT);
        isdiagatty = isatty(SHDIAG);
	for (f = 3; f < NOFILE; f++)
		(void) close(f);
}
#endif

void
donefds()
{

	(void) close(0);
	(void) close(1);
	(void) close(2);
	didfds = 0;
}

/*
 * Move descriptor i to j.
 * If j is -1 then we just want to get i to a safe place,
 * i.e. to a unit > 2.  This also happens in dcopy.
 */
int
dmove(i, j)
	register int i, j;
{

	if (i == j || i < 0)
		return (i);
#ifdef DUP2
	if (j >= 0) {
		(void) dup2(i, j);
		return (j);
	}
#endif
	j = dcopy(i, j);
	if (j != i)
		(void) close(i);
	return (j);
}

int
dcopy(i, j)
	register int i, j;
{

	if (i == j || i < 0 || j < 0 && i > 2)
		return (i);
#ifdef DUP2
	if (j >= 0) {
		(void) dup2(i, j);
		return (j);
	}
#endif
	(void) close(j);
	return (renum(i, j));
}

static int
renum(i, j)
	register int i, j;
{
	register int k = dup(i);

	if (k < 0)
		return (-1);
	if (j == -1 && k > 2)
		return (k);
	if (k != j) {
		j = renum(k, j);
		(void) close(k);
		return (j);
	}
	return (k);
}

void
copy(to, from, size)
	register char *to, *from;
	register int size;
{

	if (size && from && to)	/* (PWP) no NULL strings here */
		do
			*to++ = *from++;
		while (--size != 0);
}

/*
 * Left shift a command argument list, discarding
 * the first c arguments.  Used in "shift" commands
 * as well as by commands like "repeat".
 */
void
lshift(v, c)
	register Char **v;
	register int c;
{
	register Char **u = v;

	while (*u && --c >= 0)
		xfree((ptr_t) *u++);
	(void) blkcpy(v, u);
}

int
number(cp)
	Char *cp;
{
	if (!cp) return (0);	/* (PWP) if NULL string */
	if (*cp == '-') {
		cp++;
		if (!isdigit(*cp))
			return (0);
		cp++;
	}
	while (*cp && isdigit(*cp))
		cp++;
	return (*cp == 0);
}

Char **
copyblk(v)
	register Char **v;
{
	register Char **nv =
		(Char **) calloc((size_t) (blklen(v) + 1), sizeof (Char **));

	return (blkcpy(nv, v));
}

#ifndef SHORT_STRINGS
char *
strend(cp)
	register char *cp;
{
	if (!cp) return (cp);	/* (PWP) if NULL string */
	while (*cp)
		cp++;
	return (cp);
}
#endif /* SHORT_STRINGS */

Char *
strip(cp)
	Char *cp;
{
	register Char *dp = cp;

	if (!cp) return (cp);	/* (PWP) if NULL string */
	while (*dp++ &= TRIM)
		continue;
	return (cp);
}

void
udvar(name)
	Char *name;
{

	setname(short2str(name));
	bferr("Undefined variable");
}

int
prefix(sub, str)
	register Char *sub, *str;
{

	for (;;) {
		if (*sub == 0)
			return (1);
		if (*str == 0)
			return (0);
		if (*sub++ != *str++)
			return (0);
	}
}

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/sh.err.c,v 1.3 1990/11/25 10:12:18 christos Exp $ */
/*
 * sh.err.c: Error printing routines. There are lots of them
 *	     and none does the right thing!
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley Software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#include "config.h"

#ifdef notdef
static char *sccsid = "@(#)sh.err.c	5.3 (Berkeley) 5/13/86";
#endif
#ifndef lint
static char *rcsid = "$Id: sh.err.c,v 1.3 1990/11/25 10:12:18 christos Exp $";
#endif

#define _h_tc_err
#include "sh.h"

/*
 * C Shell
 */

static char *errarg = (char *) 0;	/* Argument to error */
static Char	*onev[2] = { STR1, NOSTR };
/*
 * Print error string s with optional argument arg.
 * This routine always resets or exits.  The flag haderr
 * is set so the routine who catches the unwind can propogate
 * it if they want.
 *
 * Note that any open files at the point of error will eventually
 * be closed in the routine process in sh.c which is the only
 * place error unwinds are ever caught.
 */
/*VARARGS1*/
void
error(s, arg)
char *s, *arg;
{
	register Char **v;

	/*
	 * Must flush before we print as we wish output before the error
	 * to go on (some form of) standard output, while output after
	 * goes on (some form of) diagnostic output.
	 * If didfds then output will go to 1/2 else to FSHOUT/FSHDIAG.
	 * See flush in sh.print.c.
	 */
	flush();
	haderr = 1;		/* Now to diagnostic output */
	timflg = 0;		/* This isn't otherwise reset */
	if (v = pargv)
		pargv = 0, blkfree(v);
	if (v = gargv)
		gargv = 0, blkfree(v);

	/*
	 * A zero arguments causes no printing, else print
	 * an error diagnostic here.
	 */
	if (s) {
		if (s == err && errarg)
			CSHprintf(s, errarg);
		else
			CSHprintf(s, arg);
		CSHprintf(".\n");
	}

	didfds = 0;		/* Forget about 0,1,2 */
	if (err && errarg) {
		xfree((ptr_t) errarg);
		errarg = (char *) 0;
	}
	errarg = 0;

	/*
	 * Go away if -e or we are a child shell
	 */
	if (exiterr || child)
		xexit(1);

	/*
	 * Reset the state of the input.
	 * This buffered seek to end of file will also
	 * clear the while/foreach stack.
	 */
	btoeof();

	setq(STRstatus, onev, &shvhed);
#ifdef BSDJOBS
	if (tpgrp > 0)
		(void) tcsetpgrp(FSHTTY, tpgrp);
#endif
	reset();		/* Unwind */
}

/*
 * Perror is the shells version of perror which should otherwise
 * never be called.
 */
void
Perror(s)
	char *s;
{

	/*
	 * Perror uses unit 2, thus if we didn't set up the fd's
	 * we must set up unit 2 now else the diagnostic will disappear
	 */
	if (!didfds) {
		register int oerrno = errno;

		(void) dcopy(SHDIAG, 2);
		errno = oerrno;
	}
	perror(s);
	error((char *)0, 0);		/* To exit or unwind */
}

void
bferr(cp)
	char *cp;
{

	flush();
	haderr = 1;
	CSHprintf("%s: ", bname);
	error(cp, NULL);
}

/*
 * The parser and scanner set up errors for later by calling seterr,
 * which sets the variable err as a side effect; later to be tested,
 * e.g. in process.
 */
void
/*VARARGS1*/
seterr(s, d)
char *s;
char *d;
{
	if (err == 0) {
		err = s; 
		if (d)
		    errarg = strsave(d);
		else
		    errarg = (char *) 0;
	}
}

static char *errorlist[] = {
#define ERR_SYNTAX	0
    "Syntax Error",
#define ERR_NOTALLOWED	1
    "%s is not allowed",
#define ERR_TOOLONG	2
    "%s too long",
#define ERR_VARBEGIN	3
    "Variable name must begin with a letter",
#define ERR_VARTOOLONG	4
    "Variable name too long",
#define ERR_VARALNUM	5
    "Variable name must contain alphanumeric characters",
#define ERR_JOBCONTROL	6
    "No job control in this shell",
#define ERR_EXPRESSION	7
    "Expression Syntax",
#define ERR_NOHOMEDIR	8
    "No home directory",
#define ERR_CANTCHANGE	9
    "Can't change to home directory",
#define ERR_TOOMANY	10
    "Too many arguments",
#define ERR_ASSIGN	11
    "Assignment missing expression",
#define ERR_UNKNOWNOP	12
    "Unknown operator",
#define ERR_AMBIG	13
    "Ambiguous",
#define ERR_RANGE	14
    "Subscript out of range",
#define ERR_OVERFLOW	15
    "Line overflow",
#define ERR_NOSUCHJOB	16
    "No such job",
#define ERR_TERMINAL	17
    "Can't from terminal",
#define ERR_NOTWHILE	18
    "Not in while/foreach",
#define ERR_NOPROC	19
    "No more processes",
#define ERR_NOMATCH	20
    "No match",
#define ERR_MISSING	21
    "Missing %c",
#define ERR_UNMATCHED	22
    "Unmatched %c",
#define ERR_INVALID	23
    "Invalid Error"
};


void
stdbferr(id)
{
    if (id >= 0 || id <= sizeof(errorlist) / sizeof(errorlist[0]))
	bferr(errorlist[id]);
    else
	stdbferr(errorlist[ERR_INVALID]);
    
}

void
/*VARARGS1*/
stderror(id, str)
int id;
char *str;
{
    if (id >= 0 || id <= sizeof(errorlist) / sizeof(errorlist[0]))
	error(errorlist[id], str);
    else
	stderror(errorlist[ERR_INVALID], str);
}

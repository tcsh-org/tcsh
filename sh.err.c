/* $Header: /u/christos/src/tcsh-6.04/RCS/sh.err.c,v 3.20 1993/06/25 21:17:12 christos Exp $ */
/*
 * sh.err.c: Error printing routines. 
 */
/*-
 * Copyright (c) 1980, 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#define _h_sh_err		/* Don't redefine the errors	 */
#include "sh.h"

RCSID("$Id: sh.err.c,v 3.20 1993/06/25 21:17:12 christos Exp $")

/*
 * C Shell
 */

#ifdef lint
#undef va_arg
#define va_arg(a, b) (a ? (b) 0 : (b) 0)
#endif

char   *seterr = NULL;	/* Holds last error if there was one */

#define ERR_FLAGS	0xf0000000
#define ERR_NAME	0x10000000
#define ERR_SILENT	0x20000000
#define ERR_OLD		0x40000000

static char *errorlist[] = 
{
#define ERR_SYNTAX	0
    "Syntax Error",
#define ERR_NOTALLOWED	1
    "%s is not allowed",
#define ERR_WTOOLONG	2
    "Word too long",
#define ERR_LTOOLONG	3
    "$< line too long",
#define ERR_DOLZERO	4
    "No file for $0",
#define ERR_INCBR	5
    "Incomplete [] modifier",
#define ERR_EXPORD	6
    "$ expansion must end before ]",
#define ERR_BADMOD	7
    "Bad : modifier in $ (%c)",
#define ERR_SUBSCRIPT	8
    "Subscript error",
#define ERR_BADNUM	9
    "Badly formed number",
#define ERR_NOMORE	10
    "No more words",
#define ERR_FILENAME	11
    "Missing file name",
#define ERR_GLOB	12
    "Internal glob error",
#define ERR_COMMAND	13
    "Command not found",
#define ERR_TOOFEW	14
    "Too few arguments",
#define ERR_TOOMANY	15
    "Too many arguments",
#define ERR_DANGER	16
    "Too dangerous to alias that",
#define ERR_EMPTYIF	17
    "Empty if",
#define ERR_IMPRTHEN	18
    "Improper then",
#define ERR_NOPAREN	19
    "Words not parenthesized",
#define ERR_NOTFOUND	20
    "%s not found",
#define ERR_MASK	21
    "Improper mask",
#define ERR_LIMIT	22
    "No such limit",
#define ERR_TOOLARGE	23
    "Argument too large",
#define ERR_SCALEF	24
    "Improper or unknown scale factor",
#define ERR_UNDVAR	25
    "Undefined variable",
#define ERR_DEEP	26
    "Directory stack not that deep",
#define ERR_BADSIG	27
    "Bad signal number",
#define ERR_UNKSIG	28
    "Unknown signal; kill -l lists signals",
#define ERR_VARBEGIN	29
    "Variable name must begin with a letter",
#define ERR_VARTOOLONG	30
    "Variable name too long",
#define ERR_VARALNUM	31
    "Variable name must contain alphanumeric characters",
#define ERR_JOBCONTROL	32
    "No job control in this shell",
#define ERR_EXPRESSION	33
    "Expression Syntax",
#define ERR_NOHOMEDIR	34
    "No home directory",
#define ERR_CANTCHANGE	35
    "Can't change to home directory",
#define ERR_NULLCOM	36
    "Invalid null command",
#define ERR_ASSIGN	37
    "Assignment missing expression",
#define ERR_UNKNOWNOP	38
    "Unknown operator",
#define ERR_AMBIG	39
    "Ambiguous",
#define ERR_EXISTS	40
    "%s: File exists",
#define ERR_ARGC	41
    "Argument for -c ends in backslash",
#define ERR_INTR	42
    "Interrupted",
#define ERR_RANGE	43
    "Subscript out of range",
#define ERR_OVERFLOW	44
    "Line overflow",
#define ERR_NOSUCHJOB	45
    "No such job",
#define ERR_TERMINAL	46
    "Can't from terminal",
#define ERR_NOTWHILE	47
    "Not in while/foreach",
#define ERR_NOPROC	48
    "No more processes",
#define ERR_NOMATCH	49
    "No match",
#define ERR_MISSING	50
    "Missing %c",
#define ERR_UNMATCHED	51
    "Unmatched %c",
#define ERR_NOMEM	52
    "Out of memory",
#define ERR_PIPE	53
    "Can't make pipe",
#define ERR_SYSTEM	54
    "%s: %s",
#define ERR_STRING	55
    "%s",
#define ERR_JOBS	56
    "Usage: jobs [ -l ]",
#define ERR_JOBARGS	57
    "Arguments should be jobs or process id's",
#define ERR_JOBCUR	58
    "No current job",
#define ERR_JOBPREV	59
    "No previous job",
#define ERR_JOBPAT	60
    "No job matches pattern",
#define ERR_NESTING	61
    "Fork nesting > %d; maybe `...` loop",
#define ERR_JOBCTRLSUB	62
    "No job control in subshells",
#define ERR_SYNC	63
    "Sunc fault: Process %d not found",
#define ERR_STOPPED	64
#ifdef SUSPENDED
    "%sThere are suspended jobs",
#else
    "%sThere are stopped jobs",
#endif				/* SUSPENDED */
#define ERR_NODIR	65
    "No other directory",
#define ERR_EMPTY	66
    "Directory stack empty",
#define ERR_BADDIR	67
    "Bad directory",
#define ERR_DIRUS	68
    "Usage: %s [-%s]%s",
#define ERR_HFLAG	69
    "No operand for -h flag",
#define ERR_NOTLOGIN	70
    "Not a login shell",
#define ERR_DIV0	71
    "Division by 0",
#define ERR_MOD0	72
    "Mod by 0",
#define ERR_BADSCALE	73
    "Bad scaling; did you mean \"%s\"?",
#define ERR_SUSPLOG	74
    "Can't suspend a login shell (yet)",
#define ERR_UNKUSER	75
    "Unknown user: %s",
#define ERR_NOHOME	76
    "No $home variable set",
#define ERR_HISTUS	77
    "Usage: history [-%s] [# number of events]",
#define ERR_SPDOLLT	78
    "$, ! or < not allowed with $# or $?",
#define ERR_NEWLINE	79
    "Newline in variable name",
#define ERR_SPSTAR	80
    "* not allowed with $# or $?",
#define ERR_DIGIT	81
    "$?<digit> or $#<digit> not allowed",
#define ERR_VARILL	82
    "Illegal variable name",
#define ERR_NLINDEX	83
    "Newline in variable index",
#define ERR_EXPOVFL	84
    "Expansion buffer overflow",
#define ERR_VARSYN	85
    "Variable syntax",
#define ERR_BADBANG	86
    "Bad ! form",
#define ERR_NOSUBST	87
    "No previous substitute",
#define ERR_BADSUBST	88
    "Bad substitute",
#define ERR_LHS		89
    "No previous left hand side",
#define ERR_RHSLONG	90
    "Right hand side too long",
#define ERR_BADBANGMOD	91
    "Bad ! modifier: %c",
#define ERR_MODFAIL	92
    "Modifier failed",
#define ERR_SUBOVFL	93
    "Substitution buffer overflow",
#define ERR_BADBANGARG	94
    "Bad ! arg selector",
#define ERR_NOSEARCH	95
    "No prev search",
#define ERR_NOEVENT	96
    "%s: Event not found",
#define ERR_TOOMANYRP	97
    "Too many )'s",
#define ERR_TOOMANYLP	98
    "Too many ('s",
#define ERR_BADPLP	99
    "Badly placed (",
#define ERR_MISRED	100
    "Missing name for redirect",
#define ERR_OUTRED	101
    "Ambiguous output redirect",
#define ERR_REDPAR	102
    "Can't << within ()'s",
#define ERR_INRED	103
    "Ambiguous input redirect",
#define ERR_BADPLPS	104
    "Badly placed ()'s",
#define ERR_ALIASLOOP	105
    "Alias loop",
#define ERR_NOWATCH	106
    "No $watch variable set",
#define ERR_NOSCHED	107
    "No scheduled events",
#define ERR_SCHEDUSAGE	108
    "Usage: sched -<item#>.\nUsage: sched [+]hh:mm <command>",
#define ERR_SCHEDEV	109
    "Not that many scheduled events",
#define ERR_SCHEDCOM	110
    "No command to run",
#define ERR_SCHEDTIME	111
    "Invalid time for event",
#define ERR_SCHEDREL	112
    "Relative time inconsistent with am/pm",
#define ERR_TCNOSTR	113
    "Out of termcap string space",
#define ERR_SETTCUS	114
    "Usage: settc %s [yes|no]",
#define ERR_TCCAP	115
    "Unknown capability `%s'",
#define ERR_TCPARM	116
    "Unknown termcap parameter `%%%c'",
#define ERR_TCARGS	117
    "Too many arguments for `%s' (%d)",
#define ERR_TCNARGS	118
    "`%s' requires %d arguments",
#define ERR_TCUSAGE	119
    "Usage: echotc [-v|-s] [<capability> [<args>]]",
#define ERR_ARCH	120
    "%s: %s. Wrong Architecture",
#define ERR_HISTLOOP	121
    "!# History loop",
#define ERR_FILEINQ	122
    "Malformed file inquiry",
#define ERR_SELOVFL	123
    "Selector overflow",
#define ERR_TCSHUSAGE   124
#ifdef apollo
    "Unknown option: -%s\nUsage: tcsh [ -bcdefilmnqstvVxX -Dname[=value] ] [ argument ... ]",
#else /* !apollo */
# ifdef convex
    "Unknown option: -%s\nUsage: tcsh [ -bcdefFilmnqstvVxX ] [ argument ... ]",
# else /* rest */
    "Unknown option: -%s\nUsage: tcsh [ -bcdefilmnqstvVxX ] [ argument ... ]",
# endif /* convex */
#endif /* apollo */
#define ERR_COMPCOM	125
    "\nInvalid completion: \"%s\"",
#define ERR_COMPINV	126
    "\nInvalid %s: '%c'",
#define ERR_COMPMIS	127
    "\nMissing separator '%c' after %s \"%s\"",
#define ERR_COMPINC	128
    "\nIncomplete %s: \"%s\"",
#define ERR_MFLAG	129
    "No operand for -m flag",
#define ERR_ULIMUS	130
    "Usage: unlimit [-fh] [limits]",
#define ERR_READONLY	131
    "$%S is read-only",
#define ERR_INVALID	132
    "Invalid Error"
};

/*
 * The parser and scanner set up errors for later by calling seterr,
 * which sets the variable err as a side effect; later to be tested,
 * e.g. in process.
 */
void
/*VARARGS1*/
#ifdef FUNCPROTO
seterror(unsigned int id, ...)
#else
seterror(va_alist)
    va_dcl
#endif
{

    if (seterr == 0) {
	va_list va;
	char    berr[BUFSIZE];
#ifdef FUNCPROTO
	va_start(va, id);
#else
	unsigned int id;
	va_start(va);
	id = va_arg(va, unsigned int);
#endif

	if (id >= sizeof(errorlist) / sizeof(errorlist[0]))
	    id = ERR_INVALID;
	xvsprintf(berr, errorlist[id], va);
	va_end(va);

	seterr = strsave(berr);
    }
}

/*
 * Print the error with the given id.
 *
 * Special ids:
 *	ERR_SILENT: Print nothing.
 *	ERR_OLD: Print the previously set error if one was there.
 *	         otherwise return.
 *	ERR_NAME: If this bit is set, print the name of the function
 *		  in bname
 *
 * This routine always resets or exits.  The flag haderr
 * is set so the routine who catches the unwind can propogate
 * it if they want.
 *
 * Note that any open files at the point of error will eventually
 * be closed in the routine process in sh.c which is the only
 * place error unwinds are ever caught.
 */
void
/*VARARGS*/
#ifdef FUNCPROTO
stderror(unsigned int id, ...)
#else
stderror(va_alist)
    va_dcl
#endif
{
    va_list va;
    register Char **v;
    int flags;
    int vareturn;

#ifdef FUNCPROTO
    va_start(va, id);
#else
    unsigned int id;

    va_start(va);
    id = va_arg(va, unsigned int);
#endif

    /*
     * Reset don't free flag for buggy os's
     */
    dont_free = 0;

    flags = (int) id & ERR_FLAGS;
    id &= ~ERR_FLAGS;

    /* Pyramid's OS/x has a subtle bug in <varargs.h> which prevents calling
     * va_end more than once in the same function. -- sterling@oldcolo.com
     */
    if (!((flags & ERR_OLD) && seterr == NULL)) {
	vareturn = 0;	/* Don't return immediately after va_end */
	if (id >= sizeof(errorlist) / sizeof(errorlist[0]))
	    id = ERR_INVALID;

	/*
	 * Must flush before we print as we wish output before the error to go
	 * on (some form of) standard output, while output after goes on (some
	 * form of) diagnostic output. If didfds then output will go to 1/2
	 * else to FSHOUT/FSHDIAG. See flush in sh.print.c.
	 */
	flush();
	haderr = 1;			/* Now to diagnostic output */
	timflg = 0;			/* This isn't otherwise reset */


	if (!(flags & ERR_SILENT)) {
	    if (flags & ERR_NAME)
		xprintf("%s: ", bname);
	    if ((flags & ERR_OLD)) {
		/* Old error. */
		xprintf("%s.\n", seterr);
		} else {
		   xvprintf(errorlist[id], va);
		    xprintf(".\n");
		}
	}
    } else {
	vareturn = 1;	/* Return immediately after va_end */
    }
    va_end(va);
    if (vareturn)
	return;

    if (seterr) {
	xfree((ptr_t) seterr);
	seterr = NULL;
    }

    if ((v = pargv) != 0)
	pargv = 0, blkfree(v);
    if ((v = gargv) != 0)
	gargv = 0, blkfree(v);

    didfds = 0;			/* Forget about 0,1,2 */
    /*
     * Go away if -e or we are a child shell
     */
    if (exiterr || child)
	xexit(1);

    /*
     * Reset the state of the input. This buffered seek to end of file will
     * also clear the while/foreach stack.
     */
    btoeof();

    set(STRstatus, Strsave(STR1), VAR_READWRITE);
#ifdef BSDJOBS
    if (tpgrp > 0)
	(void) tcsetpgrp(FSHTTY, tpgrp);
#endif
    reset();			/* Unwind */
}

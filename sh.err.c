/* $Header: /u/christos/src/tcsh-6.05/RCS/sh.err.c,v 3.24 1994/09/22 19:07:06 christos Exp $ */
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

RCSID("$Id: sh.err.c,v 3.24 1994/09/22 19:07:06 christos Exp $")

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

#define ERR_SYNTAX	0
#define ERR_NOTALLOWED	1
#define ERR_WTOOLONG	2
#define ERR_LTOOLONG	3
#define ERR_DOLZERO	4
#define ERR_INCBR	5
#define ERR_EXPORD	6
#define ERR_BADMOD	7
#define ERR_SUBSCRIPT	8
#define ERR_BADNUM	9
#define ERR_NOMORE	10
#define ERR_FILENAME	11
#define ERR_GLOB	12
#define ERR_COMMAND	13
#define ERR_TOOFEW	14
#define ERR_TOOMANY	15
#define ERR_DANGER	16
#define ERR_EMPTYIF	17
#define ERR_IMPRTHEN	18
#define ERR_NOPAREN	19
#define ERR_NOTFOUND	20
#define ERR_MASK	21
#define ERR_LIMIT	22
#define ERR_TOOLARGE	23
#define ERR_SCALEF	24
#define ERR_UNDVAR	25
#define ERR_DEEP	26
#define ERR_BADSIG	27
#define ERR_UNKSIG	28
#define ERR_VARBEGIN	29
#define ERR_VARTOOLONG	30
#define ERR_VARALNUM	31
#define ERR_JOBCONTROL	32
#define ERR_EXPRESSION	33
#define ERR_NOHOMEDIR	34
#define ERR_CANTCHANGE	35
#define ERR_NULLCOM	36
#define ERR_ASSIGN	37
#define ERR_UNKNOWNOP	38
#define ERR_AMBIG	39
#define ERR_EXISTS	40
#define ERR_ARGC	41
#define ERR_INTR	42
#define ERR_RANGE	43
#define ERR_OVERFLOW	44
#define ERR_NOSUCHJOB	45
#define ERR_TERMINAL	46
#define ERR_NOTWHILE	47
#define ERR_NOPROC	48
#define ERR_NOMATCH	49
#define ERR_MISSING	50
#define ERR_UNMATCHED	51
#define ERR_NOMEM	52
#define ERR_PIPE	53
#define ERR_SYSTEM	54
#define ERR_STRING	55
#define ERR_JOBS	56
#define ERR_JOBARGS	57
#define ERR_JOBCUR	58
#define ERR_JOBPREV	59
#define ERR_JOBPAT	60
#define ERR_NESTING	61
#define ERR_JOBCTRLSUB	62
#define ERR_SYNC	63
#define ERR_STOPPED	64
#define ERR_NODIR	65
#define ERR_EMPTY	66
#define ERR_BADDIR	67
#define ERR_DIRUS	68
#define ERR_HFLAG	69
#define ERR_NOTLOGIN	70
#define ERR_DIV0	71
#define ERR_MOD0	72
#define ERR_BADSCALE	73
#define ERR_SUSPLOG	74
#define ERR_UNKUSER	75
#define ERR_NOHOME	76
#define ERR_HISTUS	77
#define ERR_SPDOLLT	78
#define ERR_NEWLINE	79
#define ERR_SPSTAR	80
#define ERR_DIGIT	81
#define ERR_VARILL	82
#define ERR_NLINDEX	83
#define ERR_EXPOVFL	84
#define ERR_VARSYN	85
#define ERR_BADBANG	86
#define ERR_NOSUBST	87
#define ERR_BADSUBST	88
#define ERR_LHS		89
#define ERR_RHSLONG	90
#define ERR_BADBANGMOD	91
#define ERR_MODFAIL	92
#define ERR_SUBOVFL	93
#define ERR_BADBANGARG	94
#define ERR_NOSEARCH	95
#define ERR_NOEVENT	96
#define ERR_TOOMANYRP	97
#define ERR_TOOMANYLP	98
#define ERR_BADPLP	99
#define ERR_MISRED	100
#define ERR_OUTRED	101
#define ERR_REDPAR	102
#define ERR_INRED	103
#define ERR_BADPLPS	104
#define ERR_ALIASLOOP	105
#define ERR_NOWATCH	106
#define ERR_NOSCHED	107
#define ERR_SCHEDUSAGE	108
#define ERR_SCHEDEV	109
#define ERR_SCHEDCOM	110
#define ERR_SCHEDTIME	111
#define ERR_SCHEDREL	112
#define ERR_TCNOSTR	113
#define ERR_SETTCUS	114
#define ERR_TCCAP	115
#define ERR_TCPARM	116
#define ERR_TCARGS	117
#define ERR_TCNARGS	118
#define ERR_TCUSAGE	119
#define ERR_ARCH	120
#define ERR_HISTLOOP	121
#define ERR_FILEINQ	122
#define ERR_SELOVFL	123
#define ERR_TCSHUSAGE   124
#define ERR_COMPCOM	125
#define ERR_COMPINV	126
#define ERR_COMPMIS	127
#define ERR_COMPINC	128
#define ERR_MFLAG	129
#define ERR_ULIMUS	130
#define ERR_READONLY	131
#define ERR_BADJOB	132
#define ERR_INVALID	133
#define NO_ERRORS	133

static char *elst[NO_ERRORS];

/*
   Make elst entries
*/
#define MSGSAVE(i, s)	strsave(catgets(catd, 1, i, s))

/*
   Init the elst depending on the locale
*/
void
errinit()
{
    int i;
    for (i = 0; i < NO_ERRORS; i++)
	xfree((ptr_t) elst[i]);

    elst[ERR_SYNTAX] = MSGSAVE(260, "Syntax Error");
    elst[ERR_NOTALLOWED] = MSGSAVE(261, "%s is not allowed");
    elst[ERR_WTOOLONG] = MSGSAVE(262, "Word too long");
    elst[ERR_LTOOLONG] = MSGSAVE(263, "$< line too long");
    elst[ERR_DOLZERO] = MSGSAVE(264, "No file for $0");
    elst[ERR_INCBR] = MSGSAVE(265, "Incomplete [] modifier");
    elst[ERR_EXPORD] = MSGSAVE(266, "$ expansion must end before ]");
    elst[ERR_BADMOD] = MSGSAVE(267, "Bad : modifier in $ (%c)");
    elst[ERR_SUBSCRIPT] = MSGSAVE(268, "Subscript error");
    elst[ERR_BADNUM] = MSGSAVE(269, "Badly formed number");
    elst[ERR_NOMORE] = MSGSAVE(270, "No more words");
    elst[ERR_FILENAME] = MSGSAVE(271, "Missing file name");
    elst[ERR_GLOB] = MSGSAVE(272, "Internal glob error");
    elst[ERR_COMMAND] = MSGSAVE(273, "Command not found");
    elst[ERR_TOOFEW] = MSGSAVE(274, "Too few arguments");
    elst[ERR_TOOMANY] = MSGSAVE(275, "Too many arguments");
    elst[ERR_DANGER] = MSGSAVE(276, "Too dangerous to alias that");
    elst[ERR_EMPTYIF] = MSGSAVE(277, "Empty if");
    elst[ERR_IMPRTHEN] = MSGSAVE(278, "Improper then");
    elst[ERR_NOPAREN] = MSGSAVE(279, "Words not parenthesized");
    elst[ERR_NOTFOUND] = MSGSAVE(280, "%s not found");
    elst[ERR_MASK] = MSGSAVE(281, "Improper mask");
    elst[ERR_LIMIT] = MSGSAVE(282, "No such limit");
    elst[ERR_TOOLARGE] = MSGSAVE(283, "Argument too large");
    elst[ERR_SCALEF] = MSGSAVE(284, "Improper or unknown scale factor");
    elst[ERR_UNDVAR] = MSGSAVE(285, "Undefined variable");
    elst[ERR_DEEP] = MSGSAVE(286, "Directory stack not that deep");
    elst[ERR_BADSIG] = MSGSAVE(287, "Bad signal number");
    elst[ERR_UNKSIG] = MSGSAVE(288, "Unknown signal; kill -l lists signals");
    elst[ERR_VARBEGIN] = MSGSAVE(289, "Variable name must begin with a letter");
    elst[ERR_VARTOOLONG] = MSGSAVE(290, "Variable name too long");
    elst[ERR_VARALNUM] = MSGSAVE(291,
	"Variable name must contain alphanumeric characters");
    elst[ERR_JOBCONTROL] = MSGSAVE(292, "No job control in this shell");
    elst[ERR_EXPRESSION] = MSGSAVE(293, "Expression Syntax");
    elst[ERR_NOHOMEDIR] = MSGSAVE(294, "No home directory");
    elst[ERR_CANTCHANGE] = MSGSAVE(295, "Can't change to home directory");
    elst[ERR_NULLCOM] = MSGSAVE(296, "Invalid null command");
    elst[ERR_ASSIGN] = MSGSAVE(297, "Assignment missing expression");
    elst[ERR_UNKNOWNOP] = MSGSAVE(298, "Unknown operator");
    elst[ERR_AMBIG] = MSGSAVE(299, "Ambiguous");
    elst[ERR_EXISTS] = MSGSAVE(300, "%s: File exists");
    elst[ERR_ARGC] = MSGSAVE(301, "Argument for -c ends in backslash");
    elst[ERR_INTR] = MSGSAVE(302, "Interrupted");
    elst[ERR_RANGE] = MSGSAVE(303, "Subscript out of range");
    elst[ERR_OVERFLOW] = MSGSAVE(304, "Line overflow");
    elst[ERR_NOSUCHJOB] = MSGSAVE(305, "No such job");
    elst[ERR_TERMINAL] = MSGSAVE(306, "Can't from terminal");
    elst[ERR_NOTWHILE] = MSGSAVE(307, "Not in while/foreach");
    elst[ERR_NOPROC] = MSGSAVE(308, "No more processes");
    elst[ERR_NOMATCH] = MSGSAVE(309, "No match");
    elst[ERR_MISSING] = MSGSAVE(310, "Missing %c");
    elst[ERR_UNMATCHED] = MSGSAVE(311, "Unmatched %c");
    elst[ERR_NOMEM] = MSGSAVE(312, "Out of memory");
    elst[ERR_PIPE] = MSGSAVE(313, "Can't make pipe");
    elst[ERR_SYSTEM] = "%s: %s"; elst[ERR_STRING] = "%s";
    elst[ERR_JOBS] = MSGSAVE(314, "Usage: jobs [ -l ]");
    elst[ERR_JOBARGS] = MSGSAVE(315,
	"Arguments should be jobs or process id's");
    elst[ERR_JOBCUR] = MSGSAVE(316, "No current job");
    elst[ERR_JOBPREV] = MSGSAVE(317, "No previous job");
    elst[ERR_JOBPAT] = MSGSAVE(318, "No job matches pattern");
    elst[ERR_NESTING] = MSGSAVE(319, "Fork nesting > %d; maybe `...` loop");
    elst[ERR_JOBCTRLSUB] = MSGSAVE(320, "No job control in subshells");
    elst[ERR_SYNC] = MSGSAVE(321, "Sunc fault: Process %d not found");
    elst[ERR_STOPPED] =
#ifdef SUSPENDED
	MSGSAVE(322, "%sThere are suspended jobs");
#else
	MSGSAVE(323, "%sThere are stopped jobs");
#endif				/* SUSPENDED */
    elst[ERR_NODIR] = MSGSAVE(324, "No other directory");
    elst[ERR_EMPTY] = MSGSAVE(325, "Directory stack empty");
    elst[ERR_BADDIR] = MSGSAVE(326, "Bad directory");
    elst[ERR_DIRUS] = MSGSAVE(327, "Usage: %s [-%s]%s");
    elst[ERR_HFLAG] = MSGSAVE(328, "No operand for -h flag");
    elst[ERR_NOTLOGIN] = MSGSAVE(329, "Not a login shell");
    elst[ERR_DIV0] = MSGSAVE(330, "Division by 0");
    elst[ERR_MOD0] = MSGSAVE(331, "Mod by 0");
    elst[ERR_BADSCALE] = MSGSAVE(332, "Bad scaling; did you mean \"%s\"?");
    elst[ERR_SUSPLOG] = MSGSAVE(333, "Can't suspend a login shell (yet)");
    elst[ERR_UNKUSER] = MSGSAVE(334, "Unknown user: %s");
    elst[ERR_NOHOME] = MSGSAVE(335, "No $home variable set");
    elst[ERR_HISTUS] = MSGSAVE(336,
	"Usage: history [-%s] [# number of events]");
    elst[ERR_SPDOLLT] = MSGSAVE(337, "$, ! or < not allowed with $# or $?");
    elst[ERR_NEWLINE] = MSGSAVE(338, "Newline in variable name");
    elst[ERR_SPSTAR] = MSGSAVE(339, "* not allowed with $# or $?");
    elst[ERR_DIGIT] = MSGSAVE(340, "$?<digit> or $#<digit> not allowed");
    elst[ERR_VARILL] = MSGSAVE(341, "Illegal variable name");
    elst[ERR_NLINDEX] = MSGSAVE(342, "Newline in variable index");
    elst[ERR_EXPOVFL] = MSGSAVE(343, "Expansion buffer overflow");
    elst[ERR_VARSYN] = MSGSAVE(344, "Variable syntax");
    elst[ERR_BADBANG] = MSGSAVE(345, "Bad ! form");
    elst[ERR_NOSUBST] = MSGSAVE(346, "No previous substitute");
    elst[ERR_BADSUBST] = MSGSAVE(347, "Bad substitute");
    elst[ERR_LHS] = MSGSAVE(348, "No previous left hand side");
    elst[ERR_RHSLONG] = MSGSAVE(349, "Right hand side too long");
    elst[ERR_BADBANGMOD] = MSGSAVE(350, "Bad ! modifier: %c");
    elst[ERR_MODFAIL] = MSGSAVE(351, "Modifier failed");
    elst[ERR_SUBOVFL] = MSGSAVE(352, "Substitution buffer overflow");
    elst[ERR_BADBANGARG] = MSGSAVE(353, "Bad ! arg selector");
    elst[ERR_NOSEARCH] = MSGSAVE(354, "No prev search");
    elst[ERR_NOEVENT] = MSGSAVE(355, "%s: Event not found");
    elst[ERR_TOOMANYRP] = MSGSAVE(356, "Too many )'s");
    elst[ERR_TOOMANYLP] = MSGSAVE(356, "Too many ('s");
    elst[ERR_BADPLP] = MSGSAVE(358, "Badly placed (");
    elst[ERR_MISRED] = MSGSAVE(359, "Missing name for redirect");
    elst[ERR_OUTRED] = MSGSAVE(360, "Ambiguous output redirect");
    elst[ERR_REDPAR] = MSGSAVE(361, "Can't << within ()'s");
    elst[ERR_INRED] = MSGSAVE(362, "Ambiguous input redirect");
    elst[ERR_BADPLPS] = MSGSAVE(358, "Badly placed ()'s");
    elst[ERR_ALIASLOOP] = MSGSAVE(364, "Alias loop");
    elst[ERR_NOWATCH] = MSGSAVE(365, "No $watch variable set");
    elst[ERR_NOSCHED] = MSGSAVE(366, "No scheduled events");
    elst[ERR_SCHEDUSAGE] = MSGSAVE(367,
	"Usage: sched -<item#>.\nUsage: sched [+]hh:mm <command>");
    elst[ERR_SCHEDEV] = MSGSAVE(368, "Not that many scheduled events");
    elst[ERR_SCHEDCOM] = MSGSAVE(369, "No command to run");
    elst[ERR_SCHEDTIME] = MSGSAVE(370, "Invalid time for event");
    elst[ERR_SCHEDREL] = MSGSAVE(371, "Relative time inconsistent with am/pm");
    elst[ERR_TCNOSTR] = MSGSAVE(372, "Out of termcap string space");
    elst[ERR_SETTCUS] = MSGSAVE(373, "Usage: settc %s [yes|no]");
    elst[ERR_TCCAP] = MSGSAVE(374, "Unknown capability `%s'");
    elst[ERR_TCPARM] = MSGSAVE(375, "Unknown termcap parameter `%%%c'");
    elst[ERR_TCARGS] = MSGSAVE(376, "Too many arguments for `%s' (%d)");
    elst[ERR_TCNARGS] = MSGSAVE(377, "`%s' requires %d arguments");
    elst[ERR_TCUSAGE] = MSGSAVE(378,
	"Usage: echotc [-v|-s] [<capability> [<args>]]");
    elst[ERR_ARCH] = MSGSAVE(379, "%s: %s. Wrong Architecture");
    elst[ERR_HISTLOOP] = MSGSAVE(380, "!# History loop");
    elst[ERR_FILEINQ] = MSGSAVE(381, "Malformed file inquiry");
    elst[ERR_SELOVFL] = MSGSAVE(382, "Selector overflow");
#ifdef apollo
    elst[ERR_TCSHUSAGE] = MSGSAVE(383,
"Unknown option: `-%s'\nUsage: tcsh [ -bcdefilmnqstvVxX -Dname[=value] ] [ argument ... ]");
#else /* !apollo */
# ifdef convex
    elst[ERR_TCSHUSAGE] = MSGSAVE(384,
"Unknown option: `-%s'\nUsage: tcsh [ -bcdefFilmnqstvVxX ] [ argument ... ]");
# else /* rest */
    elst[ERR_TCSHUSAGE] = MSGSAVE(385,
"Unknown option: `-%s'\nUsage: tcsh [ -bcdefilmnqstvVxX ] [ argument ... ]");
# endif /* convex */
#endif /* apollo */
    elst[ERR_COMPCOM] = MSGSAVE(386, "\nInvalid completion: \"%s\"");
    elst[ERR_COMPINV] = MSGSAVE(387, "\nInvalid %s: '%c'");
    elst[ERR_COMPMIS] = MSGSAVE(388,
	"\nMissing separator '%c' after %s \"%s\"");
    elst[ERR_COMPINC] = MSGSAVE(389, "\nIncomplete %s: \"%s\"");
    elst[ERR_MFLAG] = MSGSAVE(390, "No operand for -m flag");
    elst[ERR_ULIMUS] = MSGSAVE(391, "Usage: unlimit [-fh] [limits]");
    elst[ERR_READONLY] = MSGSAVE(392, "$%S is read-only");
    elst[ERR_BADJOB] = MSGSAVE(1235, "No such job");
}
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

	if (id >= sizeof(elst) / sizeof(elst[0]))
	    id = ERR_INVALID;
	xvsprintf(berr, elst[id], va);
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
     * va_end more than once in the same function. -- sterling@netcom.com
     */
    if (!((flags & ERR_OLD) && seterr == NULL)) {
	vareturn = 0;	/* Don't return immediately after va_end */
	if (id >= sizeof(elst) / sizeof(elst[0]))
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
		   xvprintf(elst[id], va);
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

    inheredoc = 0; 		/* Not anymore in a heredoc */
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

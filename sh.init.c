/* $Header: /u/christos/src/tcsh-6.05/RCS/sh.init.c,v 3.36 1995/01/20 23:48:56 christos Exp christos $ */
/*
 * sh.init.c: Function and signal tables
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
#include "sh.h"

RCSID("$Id: sh.init.c,v 3.36 1995/01/20 23:48:56 christos Exp christos $")

#include "ed.h"
#include "tw.h"

/*
 * C shell
 */

#define	INF	0x7fffffff

struct	biltins bfunc[] = {
    { ":",		dozip,		0,	INF	},
    { "@",		dolet,		0,	INF	},
    { "alias",		doalias,	0,	INF	},
#ifdef OBSOLETE
    { "aliases",	doaliases,	0,	1,	},
#endif /* OBSOLETE */
    { "alloc",		showall,	0,	1	},
    { "bg",		dobg,		0,	INF	},
#ifdef OBSOLETE
    { "bind",		dobind,		0,	2	},
#endif /* OBSOLETE */
    { "bindkey",	dobindkey,	0,	8	},
    { "break",		dobreak,	0,	0	},
    { "breaksw",	doswbrk,	0,	0	},
    { "builtins",	dobuiltins,	0,	0	},
#ifdef KAI
    { "bye",		goodbye,	0,	0	},
#endif /* KAI */
    { "case",		dozip,		0,	1	},
    { "cd",		dochngd,	0,	INF	},
    { "chdir",		dochngd,	0,	INF	},
    { "complete",	docomplete,	0,	INF	},
    { "continue",	docontin,	0,	0	},
    { "default",	dozip,		0,	0	},
    { "dirs",		dodirs,		0,	INF	},
#ifdef _CRAY
    { "dmmode",		dodmmode,	0,	1	},
#endif /* _CRAY */
    { "echo",		doecho,		0,	INF	},
    { "echotc",		doechotc,	0,	INF	},
    { "else",		doelse,		0,	INF	},
    { "end",		doend,		0,	0	},
    { "endif",		dozip,		0,	0	},
    { "endsw",		dozip,		0,	0	},
    { "eval",		doeval,		0,	INF	},
    { "exec",		execash,	1,	INF	},
    { "exit",		doexit,		0,	INF	},
    { "fg",		dofg,		0,	INF	},
    { "filetest",	dofiletest,	2,	INF	},
    { "foreach",	doforeach,	3,	INF	},
#ifdef TCF
    { "getspath",	dogetspath,	0,	0	},
    { "getxvers",	dogetxvers,	0,	0	},
#endif /* TCF */
    { "glob",		doglob,		0,	INF	},
    { "goto",		dogoto,		1,	1	},
    { "hashstat",	hashstat,	0,	0	},
    { "history",	dohist,		0,	2	},
    { "hup",		dohup,		0,	INF	},
    { "if",		doif,		1,	INF	},
#ifdef apollo
    { "inlib", 		doinlib,	1,	INF	},
#endif /* apollo */
    { "jobs",		dojobs,		0,	1	},
    { "kill",		dokill,		1,	INF	},
#ifndef HAVENOLIMIT
    { "limit",		dolimit,	0,	3	},
#endif /* !HAVENOLIMIT */
#ifdef OBSOLETE
    { "linedit",	doecho,		0,	INF	},
#endif /* OBSOLETE */
#if !defined(HAVENOUTMP) && !defined(KAI)
    { "log",		dolog,		0,	0	},
#endif /* !HAVENOUTMP && !KAI */
    { "login",		dologin,	0,	1	},
    { "logout",		dologout,	0,	0	},
    { "ls-F",		dolist,		0,	INF	},
#ifdef TCF
    { "migrate",	domigrate,	1,	INF	},
#endif /* TCF */
#ifdef NEWGRP
    { "newgrp",		donewgrp,	1,	2	},
#endif /* NEWGRP */
    { "nice",		donice,		0,	INF	},
    { "nohup",		donohup,	0,	INF	},
    { "notify",		donotify,	0,	INF	},
    { "onintr",		doonintr,	0,	2	},
    { "popd",		dopopd,		0,	INF	},
    { "printenv",	doprintenv,	0,	1	},
    { "pushd",		dopushd,	0,	INF	},
    { "rehash",		dohash,		0,	3	},
    { "repeat",		dorepeat,	2,	INF	},
#ifdef apollo
    { "rootnode",	dorootnode,	1,	1	},
#endif /* apollo */
    { "sched",		dosched,	0,	INF	},
    { "set",		doset,		0,	INF	},
    { "setenv",		dosetenv,	0,	2	},
#ifdef MACH
    { "setpath",	dosetpath,	0,	INF	},
#endif	/* MACH */
#ifdef TCF
    { "setspath",	dosetspath,	1,	INF	},
#endif /* TCF */
    { "settc",		dosettc,	2,	2	},
    { "setty", 		dosetty,	0,      INF	},
#ifdef TCF
    { "setxvers",	dosetxvers,	0,	1	},
#endif /* TCF */
    { "shift",		shift,		0,	1	},
    { "source",		dosource,	1,	INF	},
    { "stop",		dostop,		1,	INF	},
    { "suspend",	dosuspend,	0,	0	},
    { "switch",		doswitch,	1,	INF	},
    { "telltc",		dotelltc,	0,	INF	},
    { "time",		dotime,		0,	INF	},
    { "umask",		doumask,	0,	1	},
    { "unalias",	unalias,	1,	INF	},
    { "uncomplete",	douncomplete,	1,	INF	},
    { "unhash",		dounhash,	0,	0	},
#if defined(masscomp) || defined(hcx)
    { "universe",	douniverse,	0,	1	},
#endif /* masscomp || hcx */
#ifndef HAVENOLIMIT
    { "unlimit",	dounlimit,	0,	INF	},
#endif /* !HAVENOLIMIT */
    { "unset",		unset,		1,	INF	},
    { "unsetenv",	dounsetenv,	1,	INF	},
#ifdef apollo
    { "ver",		dover,		0,	INF	},
#endif /* apollo */
    { "wait",		dowait,		0,	0	},
#ifdef WARP
    { "warp",		dowarp,		0,	2	},
#endif /* WARP */
#if !defined(HAVENOUTMP) && defined(KAI)
    { "watchlog",	dolog,		0,	0	},
#endif /* !HAVENOUTMP && KAI */
    { "where",		dowhere,	1,	INF	},
    { "which",		dowhich,	1,	INF	},
    { "while",		dowhile,	1,	INF	}
};
int nbfunc = sizeof bfunc / sizeof *bfunc;

struct srch srchn[] = {
    { "@",		TC_LET		},
    { "break",		TC_BREAK	},
    { "breaksw",	TC_BRKSW	},
    { "case",		TC_CASE		},
    { "default", 	TC_DEFAULT	},
    { "else",		TC_ELSE		},
    { "end",		TC_END		},
    { "endif",		TC_ENDIF	},
    { "endsw",		TC_ENDSW	},
    { "exit",		TC_EXIT		},
    { "foreach", 	TC_FOREACH	},
    { "goto",		TC_GOTO		},
    { "if",		TC_IF		},
    { "label",		TC_LABEL	},
    { "set",		TC_SET		},
    { "switch",		TC_SWITCH	},
    { "while",		TC_WHILE	}
};
int nsrchn = sizeof srchn / sizeof *srchn;

#define SAVEMSG(i, s)	strsave(catgets(catd, 1, i, s))

#ifdef SUSPENDED
# define SAVEMSG_STOP		SAVEMSG(445, "Suspended (signal)")
# define SAVEMSG_TSTP		SAVEMSG(446, "Suspended")
# define SAVEMSG_TTIN		SAVEMSG(447, "Suspended (tty input)")
# define SAVEMSG_TTOU		SAVEMSG(448, "Suspended (tty output)")
#else /* STOPPED */
# define SAVEMSG_STOP		SAVEMSG(445, "Stopped (signal)")
# define SAVEMSG_TSTP		SAVEMSG(446, "Stopped")
# define SAVEMSG_TTIN		SAVEMSG(447, "Stopped (tty input)")
# define SAVEMSG_TTOU		SAVEMSG(448, "Stopped (tty output)")
#endif /* SUSPENDED */
/*
 * Note: For some machines, (hpux eg.)
 * NSIG = number of signals + 1...
 * so we define 33 or 65 (POSIX) signals for 
 * everybody
 */

/* These are here for systems with bad NSIG */
#ifndef NSIG
#ifdef POSIX
#define NSIG 64
#else /* !POSIX */
#define NSIG 32
#endif /* POSIX */
#endif /* !NSIG */

int	nsig = NSIG;
struct	mesg mesg[NSIG+1];

void
mesginit()
{
    int i;

    for (i = 0; i < NSIG + 1; i++)
	xfree((ptr_t) mesg[i].pname);

#ifdef DECOSF1
/*  0 */	mesg[0].iname = "NULL";
		mesg[0].pname = SAVEMSG(453, "");
#else /* !DECOSF1 */
/*  0 */	mesg[0].iname = 0;
		mesg[0].pname = SAVEMSG(453, "");
#endif /* DECOSF1 */
/*  1 */	mesg[1].iname = "HUP";
		mesg[1].pname = SAVEMSG(454, "Hangup");
/*  2 */	mesg[2].iname = "INT";
		mesg[2].pname = SAVEMSG(455, "Interrupt");
/*  3 */	mesg[3].iname = "QUIT";
		mesg[3].pname = SAVEMSG(456, "Quit");
/*  4 */	mesg[4].iname = "ILL";
		mesg[4].pname = SAVEMSG(457, "Illegal instruction");
/*  5 */	mesg[5].iname = "TRAP";
		mesg[5].pname = SAVEMSG(458, "Trace/BPT trap");
#if SYSVREL > 3 || defined(__EMX__) || defined(_VMS_POSIX)
/*  6 */	mesg[3].iname = "ABRT";
		mesg[3].pname = SAVEMSG(459, "Abort");
#else /* SYSVREL < 3 */
/*  6 */	mesg[3].iname = "IOT";
		mesg[3].pname = SAVEMSG(460, "IOT trap");
#endif /* SYSVREL > 3 || __EMX__ || _VMS_POSIX */
#ifdef aiws
/*  7 */	mesg[3].iname = "DANGER";
		mesg[3].pname = SAVEMSG(461, "System Crash Imminent");
#else /* !aiws */
# ifdef linux
/*  7 */	mesg[7].iname = 0;
		mesg[7].pname = SAVEMSG(462, "Signal 7");
# else /* !linux */
#  ifdef _CRAY
/*  7 */	mesg[7].iname = "ERR";
		mesg[7].pname = SAVEMSG(463, "Error exit");
#  else /* !_CRAY */
/*  7 */	mesg[7].iname = "EMT";
		mesg[7].pname = SAVEMSG(464, "EMT trap");
#  endif /* _CRAY */
# endif /* linux */
#endif /* aiws */
/*  8 */	mesg[8].iname = "FPE";
		mesg[8].pname = SAVEMSG(465, "Floating exception");
/*  9 */	mesg[9].iname = "KILL";
		mesg[9].pname = SAVEMSG(466, "Killed");
#ifdef linux
/* 10 */	mesg[10].iname = "USR1";
		mesg[10].pname = SAVEMSG(467, "User signal 1");
/* 11 */	mesg[11].iname = "SEGV";
		mesg[11].pname = SAVEMSG(468, "Segmentation fault");
/* 12 */	mesg[12].iname = "USR2";
		mesg[12].pname = SAVEMSG(469, "User signal 2");
#else /* linux */
# ifndef _CRAY
/* 10 */	mesg[10].iname = "BUS";
		mesg[10].pname = SAVEMSG(470, "Bus error");
/* 11 */	mesg[11].iname = "SEGV";
		mesg[11].pname = SAVEMSG(468, "Segmentation fault");
# else /* _CRAY */
/* 10 */	mesg[10].iname = "PRE";
		mesg[10].pname = SAVEMSG(472, "Program range error");
/* 11 */	mesg[11].iname = "ORE";
		mesg[11].pname = SAVEMSG(473, "Operand range error");
# endif /* !_CRAY */
/* 12 */	mesg[12].iname = "SYS";
		mesg[12].pname = SAVEMSG(474, "Bad system call");
#endif /* linux */
/* 13 */	mesg[13].iname = "PIPE";
		mesg[13].pname = SAVEMSG(475, "Broken pipe");
/* 14 */	mesg[14].iname = "ALRM";
		mesg[14].pname = SAVEMSG(476, "Alarm clock");
/* 15 */	mesg[15].iname = "TERM";
		mesg[15].pname = SAVEMSG(477, "Terminated");

#if (SYSVREL > 0) || defined(DGUX) || defined(IBMAIX) || defined(apollo) || defined(masscomp) || defined(ardent) || defined(linux) || defined(hcx)

# ifdef _sigextra_
#  undef  _sigextra_
# endif /* _sigextra_ */

# if !defined(IBMAIX) && !defined(cray) && !defined(__EMX__) && !defined(linux) && !defined(SOLARIS2)
/* these are the real svid signals */
/* 16 */	mesg[0].iname = "USR1";
		mesg[0].pname = SAVEMSG(478, "User signal 1");
/* 17 */	mesg[17].iname = "USR2";
		mesg[17].pname = SAVEMSG(479, "User signal 2");
#  ifdef apollo
/* 18 */	mesg[18].iname = "CLD";
		mesg[18].pname = SAVEMSG(480, "Death of child");
/* 19 */	mesg[19].iname = "APOLLO";
		mesg[19].pname = SAVEMSG(481, "Apollo-specific fault");
#  else /* !apollo */
/* 18 */	mesg[18].iname = "CHLD";
		mesg[18].pname = SAVEMSG(482, "Child exited");
/* 19 */	mesg[19].iname = "PWR";
		mesg[19].pname = SAVEMSG(483, "Power failure");
#  endif /* apollo */
# endif /* !IBMAIX && !cray && !__EMX__ && !linux */

# ifdef SOLARIS2
/* 16 */	mesg[2].iname = "USR1";
		mesg[2].pname = SAVEMSG(484, "User signal 1");
/* 17 */	mesg[17].iname = "USR2";
		mesg[17].pname = SAVEMSG(485, "User signal 2");
/* 18 */	mesg[18].iname = "CLD";
		mesg[18].pname = SAVEMSG(486, "Child status change");
#  if SOLARIS2 >= 22
/* 19 */	mesg[2].iname = "PWR";
		mesg[2].pname = SAVEMSG(483, "Power failure");
#  else /* SOLARIS2 < 22 */
/* 19 */	mesg[2].iname = "LOST";
		mesg[2].pname = SAVEMSG(488, "Resource Lost");
#  endif /* SOLARIS2 >= 22 */
# endif /* SOLARIS2 */

# ifdef __EMX__
#  define _sigextra_
/* 16 */	mesg[2].iname = 0;
		mesg[2].pname = SAVEMSG(489, "Signal 16");
/* 17 */	mesg[17].iname = 0;
		mesg[17].pname = SAVEMSG(490, "Signal 17");
/* 18 */	mesg[18].iname = "CLD";
		mesg[18].pname = SAVEMSG(491, "Child exited");
/* 19 */	mesg[19].iname = 0;
		mesg[19].pname = SAVEMSG(492, "Signal 19");
/* 20 */	mesg[20].iname = 0;
		mesg[20].pname = SAVEMSG(493, "Signal 20");
/* 21 */	mesg[21].iname = "BREAK";
		mesg[21].pname = SAVEMSG(494, "Break (Ctrl-Break)");
# endif /* __EMX__ */


# ifdef _CRAYCOM
#  define _sigextra_
/* 16 */	mesg[16].iname = "IO";
		mesg[16].pname = SAVEMSG(495, "Input/output possible signal");
/* 17 */	mesg[17].iname = "URG";
		mesg[17].pname = SAVEMSG(496, "Urgent condition on I/O channel");
/* 18 */	mesg[18].iname = "CHLD";
		mesg[18].pname = SAVEMSG(497, "Child exited");
/* 19 */	mesg[19].iname = "PWR";
		mesg[19].pname = SAVEMSG(498, "Power failure");
/* 20 */	mesg[20].iname = "MT";
		mesg[20].pname = SAVEMSG(499, "Multitasking wake-up");
/* 21 */	mesg[21].iname = "MTKILL";
		mesg[21].pname = SAVEMSG(500, "Multitasking kill");
/* 22 */	mesg[22].iname = "BUFIO";
		mesg[22].pname = SAVEMSG(501, "Fortran asynchronous I/O completion");
/* 23 */	mesg[23].iname = "RECOVERY";
		mesg[23].pname = SAVEMSG(502, "Recovery");
/* 24 */	mesg[24].iname = "UME";
		mesg[24].pname = SAVEMSG(503, "Uncorrectable memory error");
/* 25 */	mesg[25].iname = 0;
		mesg[25].pname = SAVEMSG(504, "Signal 25");
/* 26 */	mesg[26].iname = "CPULIM";
		mesg[26].pname = SAVEMSG(505, "CPU time limit exceeded");
/* 27 */	mesg[27].iname = "SHUTDN";
		mesg[27].pname = SAVEMSG(506, "System shutdown imminent");
/* 28 */	mesg[28].iname = "NOWAK";
		mesg[28].pname = SAVEMSG(507, "micro-tasking group-no wakeup flag set");
/* 29 */	mesg[29].iname = "THERR";
		mesg[29].pname = SAVEMSG(508, "Thread error - (use cord -T for detailed info)");
/* 30 */	mesg[30].iname = 0;
		mesg[30].pname = SAVEMSG(509, "Signal 30");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(510, "Signal 31");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(511, "Signal 32");
# endif /* _CRAYCOM */

# if defined(cray) && !defined(_CRAYCOM)
# define _sigextra_
# define _64sig_
/* 16 */	mesg[64].iname = "IO";
		mesg[64].pname = SAVEMSG(512, "Input/output possible signal");
/* 17 */	mesg[17].iname = "URG";
		mesg[17].pname = SAVEMSG(513, "Urgent condition on I/O channel");
/* 18 */	mesg[18].iname = "CHLD";
		mesg[18].pname = SAVEMSG(514, "Child exited");
/* 19 */	mesg[19].iname = "PWR";
		mesg[19].pname = SAVEMSG(515, "Power failure");
/* 20 */	mesg[20].iname = "MT";
		mesg[20].pname = SAVEMSG(516, "Multitasking wake-up");
/* 21 */	mesg[21].iname = "MTKILL";
		mesg[21].pname = SAVEMSG(517, "Multitasking kill");
/* 22 */	mesg[22].iname = "BUFIO";
		mesg[22].pname = SAVEMSG(518, "Fortran asynchronous I/O completion");
/* 23 */	mesg[23].iname = "RECOVERY";
		mesg[23].pname = SAVEMSG(519, "Recovery");
/* 24 */	mesg[24].iname = "UME";
		mesg[24].pname = SAVEMSG(520, "Uncorrectable memory error");
/* 25 */	mesg[25].iname = "DLK";
		mesg[25].pname = SAVEMSG(521, "True deadlock detected");
/* 26 */	mesg[26].iname = "CPULIM";
		mesg[26].pname = SAVEMSG(522, "CPU time limit exceeded");
/* 27 */	mesg[27].iname = "SHUTDN";
		mesg[27].pname = SAVEMSG(523, "System shutdown imminent");
/* 28 */	mesg[28].iname = "STOP";
		mesg[28].pname = SAVEMSG_STOP;
/* 29 */	mesg[29].iname = "TSTP";
		mesg[29].pname = SAVEMSG_TSTP;
/* 30 */	mesg[30].iname = "CONT";
		mesg[30].pname = SAVEMSG(526, "Continue");
/* 31 */	mesg[31].iname = "TTIN";
		mesg[31].pname = SAVEMSG_TTIN;
/* 32 */	mesg[32].iname = "TTOU";
		mesg[32].pname = SAVEMSG_TTOU;
/* 33 */	mesg[33].iname = "WINCH";
		mesg[33].pname = SAVEMSG(529, "Window size changed");
/* 34 */	mesg[34].iname = "RPE";
		mesg[34].pname = SAVEMSG(530, "CRAY Y-MP register parity error");
/* 35 */	mesg[35].iname = 0;
		mesg[35].pname = SAVEMSG(531, "Signal 35");
/* 36 */	mesg[36].iname = 0;
		mesg[36].pname = SAVEMSG(532, "Signal 36");
/* 37 */	mesg[37].iname = 0;
		mesg[37].pname = SAVEMSG(533, "Signal 37");
/* 38 */	mesg[38].iname = 0;
		mesg[38].pname = SAVEMSG(534, "Signal 38");
/* 39 */	mesg[39].iname = 0;
		mesg[39].pname = SAVEMSG(535, "Signal 39");
/* 40 */	mesg[40].iname = 0;
		mesg[40].pname = SAVEMSG(536, "Signal 40");
/* 41 */	mesg[41].iname = 0;
		mesg[41].pname = SAVEMSG(537, "Signal 41");
/* 42 */	mesg[42].iname = 0;
		mesg[42].pname = SAVEMSG(538, "Signal 42");
/* 43 */	mesg[43].iname = 0;
		mesg[43].pname = SAVEMSG(539, "Signal 43");
/* 44 */	mesg[44].iname = 0;
		mesg[44].pname = SAVEMSG(540, "Signal 44");
/* 45 */	mesg[45].iname = 0;
		mesg[45].pname = SAVEMSG(541, "Signal 45");
/* 46 */	mesg[46].iname = 0;
		mesg[46].pname = SAVEMSG(542, "Signal 46");
/* 47 */	mesg[47].iname = 0;
		mesg[47].pname = SAVEMSG(543, "Signal 47");
/* 48 */	mesg[48].iname = "INFO";
		mesg[48].pname = SAVEMSG(544, "Information signal");
/* 49 */	mesg[49].iname = "USR1";
		mesg[49].pname = SAVEMSG(545, "User-defined signal 1");
/* 50 */	mesg[50].iname = "USR2";
		mesg[50].pname = SAVEMSG(546, "User-defined signal 2");
/* 51 */	mesg[51].iname = 0;
		mesg[51].pname = SAVEMSG(547, "Signal 51");
/* 52 */	mesg[52].iname = 0;
		mesg[52].pname = SAVEMSG(548, "Signal 52");
/* 53 */	mesg[53].iname = 0;
		mesg[53].pname = SAVEMSG(549, "Signal 53");
/* 54 */	mesg[54].iname = 0;
		mesg[54].pname = SAVEMSG(550, "Signal 54");
/* 55 */	mesg[55].iname = 0;
		mesg[55].pname = SAVEMSG(551, "Signal 55");
/* 56 */	mesg[56].iname = 0;
		mesg[56].pname = SAVEMSG(552, "Signal 56");
/* 57 */	mesg[57].iname = 0;
		mesg[57].pname = SAVEMSG(553, "Signal 57");
/* 58 */	mesg[58].iname = 0;
		mesg[58].pname = SAVEMSG(554, "Signal 58");
/* 59 */	mesg[59].iname = 0;
		mesg[59].pname = SAVEMSG(555, "Signal 59");
/* 60 */	mesg[60].iname = 0;
		mesg[60].pname = SAVEMSG(556, "Signal 60");
/* 61 */	mesg[61].iname = 0;
		mesg[61].pname = SAVEMSG(557, "Signal 61");
/* 62 */	mesg[62].iname = 0;
		mesg[62].pname = SAVEMSG(558, "Signal 62");
/* 63 */	mesg[63].iname = 0;
		mesg[63].pname = SAVEMSG(559, "Signal 63");
/* 64 */	mesg[64].iname = 0;
		mesg[64].pname = SAVEMSG(560, "Signal 64");
# endif /* cray */

/*
**  In the UNIXpc these signal *ARE* used!!
*/
# ifdef UNIXPC
/* 20 */	mesg[20].iname = "WIND";
		mesg[20].pname = SAVEMSG(561, "Window status changed");
/* 21 */	mesg[21].iname = "PHONE";
		mesg[21].pname = SAVEMSG(562, "Phone status changed");
# endif /* UNIXPC */

# ifdef OREO
#  define _sigextra_
/* 20 */	mesg[20].iname = "TSTP";
		mesg[20].pname = SAVEMSG_TSTP;
/* 21 */	mesg[21].iname = "TTIN";
		mesg[21].pname = SAVEMSG_TTIN;
/* 22 */	mesg[22].iname = "TTOU";
		mesg[22].pname = SAVEMSG_TTOU;
/* 23 */	mesg[23].iname = "STOP";
		mesg[23].pname = SAVEMSG_STOP;
/* 24 */	mesg[24].iname = "XCPU";
		mesg[24].pname = SAVEMSG(567, "Cputime limit exceeded");
/* 25 */	mesg[25].iname = "XFSZ";
		mesg[25].pname = SAVEMSG(568, "Filesize limit exceeded");
/* 26 */	mesg[26].iname = "VTALRM";
		mesg[26].pname = SAVEMSG(569, "Virtual time alarm");
/* 27 */	mesg[27].iname = "PROF";
		mesg[27].pname = SAVEMSG(570, "Profiling time alarm");
/* 28 */	mesg[28].iname = "WINCH";
		mesg[28].pname = SAVEMSG(571, "Window changed");
/* 29 */	mesg[29].iname = "CONT";
		mesg[29].pname = SAVEMSG(572, "Continued");
/* 30 */	mesg[30].iname = "URG";
		mesg[30].pname = SAVEMSG(573, "Urgent condition on IO channel");
/* 31 */	mesg[31].iname = "IO";
		mesg[31].pname = SAVEMSG(574, "Asynchronous I/O (select)");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(575, "Signal 32");
# endif /* OREO */

# ifdef hpux
#  define _sigextra_
/* 20 */	mesg[20].iname = "VTALRM";
		mesg[20].pname = SAVEMSG(576, "Virtual time alarm");
/* 21 */	mesg[21].iname = "PROF";
		mesg[21].pname = SAVEMSG(577, "Profiling time alarm");
/* 22 */	mesg[22].iname = "IO";
		mesg[22].pname = SAVEMSG(578, "Asynchronous I/O (select)");
/* 23 */	mesg[23].iname = "WINDOW";
		mesg[23].pname = SAVEMSG(579, "Window changed");
/* 24 */	mesg[24].iname = "STOP";
		mesg[24].pname = SAVEMSG_STOP;
/* 25 */	mesg[25].iname = "TSTP";
		mesg[25].pname = SAVEMSG_TSTP;
/* 26 */	mesg[26].iname = "CONT";
		mesg[26].pname = SAVEMSG(582, "Continued");
/* 27 */	mesg[27].iname = "TTIN";
		mesg[27].pname = SAVEMSG_TTIN;
/* 28 */	mesg[28].iname = "TTOU";
		mesg[28].pname = SAVEMSG_TTOU;
/* 29 */	mesg[29].iname = "URG";
		mesg[29].pname = SAVEMSG(585, "Urgent condition on IO channel");
/* 30 */	mesg[30].iname = "LOST";
		mesg[30].pname = SAVEMSG(586, "Remote lock lost (NFS)");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(587, "Reserved");
/* 32 */	mesg[32].iname = "DIL";
		mesg[32].pname = SAVEMSG(588, "DIL signal");
# endif /* hpux */

# ifdef stellar
#  define _sigextra_
/* 20 */	mesg[20].iname = "WINDOW";
		mesg[20].pname = SAVEMSG(589, "Window changed");
/* 21 */	mesg[21].iname = "URG";
		mesg[21].pname = SAVEMSG(590, "Urgent condition on IO channel");
/* 22 */	mesg[22].iname = "POLL";
		mesg[22].pname = SAVEMSG(591, "Pollable event occured");
/* 23 */	mesg[23].iname = "STOP";
		mesg[23].pname = SAVEMSG_STOP;
/* 24 */	mesg[24].iname = "TSTP";
		mesg[24].pname = SAVEMSG_TSTP;
/* 25 */	mesg[25].iname = "CONT";
		mesg[25].pname = SAVEMSG(594, "Continued");
/* 26 */	mesg[26].iname = "TTIN";
		mesg[26].pname = SAVEMSG_TTIN;
/* 27 */	mesg[27].iname = "TTOU";
		mesg[27].pname = SAVEMSG_TTOU;
/* 28 */	mesg[28].iname = "IO";
		mesg[28].pname = SAVEMSG(597, "Asynchronous I/O (select)");
/* 29 */	mesg[29].iname = "XCPU";
		mesg[29].pname = SAVEMSG(598, "Cputime limit exceeded");
/* 30 */	mesg[30].iname = "XFSZ";
		mesg[30].pname = SAVEMSG(599, "Filesize limit exceeded");
/* 31 */	mesg[31].iname = "VTALRM";
		mesg[31].pname = SAVEMSG(600, "Virtual time alarm");
/* 32 */	mesg[32].iname = "PROF";
		mesg[32].pname = SAVEMSG(601, "Profiling time alarm");
# endif /* stellar */

# ifdef ardent
#  define _sigextra_
/* 20 */	mesg[20].iname = "WINDOW";
		mesg[20].pname = SAVEMSG(602, "Window changed");
/* 21 */	mesg[21].iname = "URG";
		mesg[21].pname = SAVEMSG(603, "Urgent condition on IO channel");
/* 22 */	mesg[22].iname = "POLL";
		mesg[22].pname = SAVEMSG(604, "Pollable event occured");
/* 23 */	mesg[23].iname = "STOP";
		mesg[23].pname = SAVEMSG_STOP;
/* 24 */	mesg[24].iname = "TSTP";
		mesg[24].pname = SAVEMSG_TSTP;
/* 25 */	mesg[25].iname = "TTIN";
		mesg[25].pname = SAVEMSG_TTIN;
/* 26 */	mesg[26].iname = "TTOU";
		mesg[26].pname = SAVEMSG_TTOU;
/* 27 */	mesg[27].iname = "CONT";
		mesg[27].pname = SAVEMSG(609, "Continued");
/* 28 */	mesg[28].iname = "XCPU";
		mesg[28].pname = SAVEMSG(610, "Cputime limit exceeded");
/* 29 */	mesg[29].iname = "XFSZ";
		mesg[29].pname = SAVEMSG(611, "Filesize limit exceeded");
/* 30 */	mesg[30].iname = "VTALRM";
		mesg[30].pname = SAVEMSG(612, "Virtual time alarm");
/* 31 */	mesg[31].iname = "PROF";
		mesg[31].pname = SAVEMSG(613, "Profiling time alarm");
# endif /* ardent */

# if SYSVREL > 3
#  define _sigextra_
/* 20 */	mesg[3].iname = "WINCH";
		mesg[3].pname = SAVEMSG(614, "Window change");
/* 21 */	mesg[21].iname = "URG";
		mesg[21].pname = SAVEMSG(615, "Urgent socket condition");
/* 22 */	mesg[22].iname = "IO";
		mesg[22].pname = SAVEMSG(616, "Socket I/O possible");
/* 23 */	mesg[23].iname = "STOP";
		mesg[23].pname = SAVEMSG_STOP;
/* 24 */	mesg[24].iname = "TSTP";
		mesg[24].pname = SAVEMSG_TSTP;
/* 25 */	mesg[25].iname = "CONT";
		mesg[25].pname = SAVEMSG(619, "Continued");
/* 26 */	mesg[26].iname = "TTIN";
		mesg[26].pname = SAVEMSG_TTIN;
/* 27 */	mesg[27].iname = "TTOU";
		mesg[27].pname = SAVEMSG_TTOU;
/* 28 */	mesg[28].iname = "VTALRM";
		mesg[28].pname = SAVEMSG(622, "Virtual timer expired");
/* 29 */	mesg[29].iname = "PROF";
		mesg[29].pname = SAVEMSG(623, "Profiling timer expired");
/* 30 */	mesg[30].iname = "XCPU";
		mesg[30].pname = SAVEMSG(624, "CPU time limit exceeded");
/* 31 */	mesg[31].iname = "XFSZ";
		mesg[31].pname = SAVEMSG(625, "File size limit exceeded");
#  ifdef SOLARIS2
#   define _64sig_
/* 32 */	mesg[2].iname = "WAITING";
		mesg[2].pname = SAVEMSG(626, "Process's lwps are blocked");
/* 33 */	mesg[33].iname = "LWP";
		mesg[33].pname = SAVEMSG(627, "Special LWP signal");
#   if SOLARIS2 >= 23
/* 34 */	mesg[2].iname = "FREEZE";
		mesg[2].pname = SAVEMSG(628, "Special CPR Signal");
/* 35 */	mesg[35].iname = "THAW";
		mesg[35].pname = SAVEMSG(629, "Special CPR Signal");
/* 36 */	mesg[36].iname = "RTMIN";
		mesg[36].pname = SAVEMSG(630, "First Realtime Signal");
/* 37 */	mesg[37].iname = "RTMIN+1";
		mesg[37].pname = SAVEMSG(631, "Second Realtime Signal");
/* 38 */	mesg[38].iname = "RTMIN+2";
		mesg[38].pname = SAVEMSG(632, "Third Realtime Signal");
/* 39 */	mesg[39].iname = "RTMIN+3";
		mesg[39].pname = SAVEMSG(633, "Fourth Realtime Signal");
/* 40 */	mesg[40].iname = "RTMAX-3";
		mesg[40].pname = SAVEMSG(634, "Fourth Last Realtime Signal");
/* 41 */	mesg[41].iname = "RTMAX-2";
		mesg[41].pname = SAVEMSG(635, "Third Last Realtime Signal");
/* 42 */	mesg[42].iname = "RTMAX-1";
		mesg[42].pname = SAVEMSG(636, "Second Last Realtime Signal");
/* 43 */	mesg[43].iname = "RTMAX";
		mesg[43].pname = SAVEMSG(637, "Last Realtime Signal");
/* 44 */	mesg[44].iname = 0;
		mesg[44].pname = SAVEMSG(638, "Signal 44");
/* 45 */	mesg[45].iname = 0;
		mesg[45].pname = SAVEMSG(639, "Signal 45");
/* 46 */	mesg[46].iname = 0;
		mesg[46].pname = SAVEMSG(640, "Signal 46");
/* 47 */	mesg[47].iname = 0;
		mesg[47].pname = SAVEMSG(641, "Signal 47");
/* 48 */	mesg[48].iname = 0;
		mesg[48].pname = SAVEMSG(642, "Signal 48");
/* 49 */	mesg[49].iname = 0;
		mesg[49].pname = SAVEMSG(643, "Signal 49");
/* 50 */	mesg[50].iname = 0;
		mesg[50].pname = SAVEMSG(644, "Signal 50");
/* 51 */	mesg[51].iname = 0;
		mesg[51].pname = SAVEMSG(645, "Signal 51");
/* 52 */	mesg[52].iname = 0;
		mesg[52].pname = SAVEMSG(646, "Signal 52");
/* 53 */	mesg[53].iname = 0;
		mesg[53].pname = SAVEMSG(647, "Signal 53");
/* 54 */	mesg[54].iname = 0;
		mesg[54].pname = SAVEMSG(648, "Signal 54");
/* 55 */	mesg[55].iname = 0;
		mesg[55].pname = SAVEMSG(649, "Signal 55");
/* 56 */	mesg[56].iname = 0;
		mesg[56].pname = SAVEMSG(650, "Signal 56");
/* 57 */	mesg[57].iname = 0;
		mesg[57].pname = SAVEMSG(651, "Signal 57");
/* 58 */	mesg[58].iname = 0;
		mesg[58].pname = SAVEMSG(652, "Signal 58");
/* 59 */	mesg[59].iname = 0;
		mesg[59].pname = SAVEMSG(653, "Signal 59");
/* 60 */	mesg[60].iname = 0;
		mesg[60].pname = SAVEMSG(654, "Signal 60");
/* 61 */	mesg[61].iname = 0;
		mesg[61].pname = SAVEMSG(655, "Signal 61");
/* 62 */	mesg[62].iname = 0;
		mesg[62].pname = SAVEMSG(656, "Signal 62");
/* 63 */	mesg[63].iname = 0;
		mesg[63].pname = SAVEMSG(657, "Signal 63");
/* 64 */	mesg[64].iname = 0;
		mesg[64].pname = SAVEMSG(658, "Signal 64");
#   else /* SOLARIS2 < 23 */
/* 34 */	mesg[2].iname = 0;
		mesg[2].pname = SAVEMSG(659, "Signal 34");
/* 35 */	mesg[35].iname = 0;
		mesg[35].pname = SAVEMSG(660, "Signal 35");
/* 36 */	mesg[36].iname = 0;
		mesg[36].pname = SAVEMSG(661, "Signal 36");
/* 37 */	mesg[37].iname = 0;
		mesg[37].pname = SAVEMSG(662, "Signal 37");
/* 38 */	mesg[38].iname = 0;
		mesg[38].pname = SAVEMSG(663, "Signal 38");
/* 39 */	mesg[39].iname = 0;
		mesg[39].pname = SAVEMSG(664, "Signal 39");
/* 40 */	mesg[40].iname = 0;
		mesg[40].pname = SAVEMSG(665, "Signal 40");
/* 41 */	mesg[41].iname = 0;
		mesg[41].pname = SAVEMSG(666, "Signal 41");
/* 42 */	mesg[42].iname = 0;
		mesg[42].pname = SAVEMSG(667, "Signal 42");
/* 43 */	mesg[43].iname = 0;
		mesg[43].pname = SAVEMSG(668, "Signal 43");
/* 44 */	mesg[44].iname = 0;
		mesg[44].pname = SAVEMSG(669, "Signal 44");
/* 45 */	mesg[45].iname = 0;
		mesg[45].pname = SAVEMSG(670, "Signal 45");
/* 46 */	mesg[46].iname = 0;
		mesg[46].pname = SAVEMSG(671, "Signal 46");
/* 47 */	mesg[47].iname = 0;
		mesg[47].pname = SAVEMSG(672, "Signal 47");
/* 48 */	mesg[48].iname = 0;
		mesg[48].pname = SAVEMSG(673, "Signal 48");
/* 49 */	mesg[49].iname = 0;
		mesg[49].pname = SAVEMSG(674, "Signal 49");
/* 50 */	mesg[50].iname = 0;
		mesg[50].pname = SAVEMSG(675, "Signal 50");
/* 51 */	mesg[51].iname = 0;
		mesg[51].pname = SAVEMSG(676, "Signal 51");
/* 52 */	mesg[52].iname = 0;
		mesg[52].pname = SAVEMSG(677, "Signal 52");
/* 53 */	mesg[53].iname = 0;
		mesg[53].pname = SAVEMSG(678, "Signal 53");
/* 54 */	mesg[54].iname = 0;
		mesg[54].pname = SAVEMSG(679, "Signal 54");
/* 55 */	mesg[55].iname = 0;
		mesg[55].pname = SAVEMSG(680, "Signal 55");
/* 56 */	mesg[56].iname = 0;
		mesg[56].pname = SAVEMSG(681, "Signal 56");
/* 57 */	mesg[57].iname = 0;
		mesg[57].pname = SAVEMSG(682, "Signal 57");
/* 58 */	mesg[58].iname = 0;
		mesg[58].pname = SAVEMSG(683, "Signal 58");
/* 59 */	mesg[59].iname = 0;
		mesg[59].pname = SAVEMSG(684, "Signal 59");
/* 60 */	mesg[60].iname = 0;
		mesg[60].pname = SAVEMSG(685, "Signal 60");
/* 61 */	mesg[61].iname = 0;
		mesg[61].pname = SAVEMSG(686, "Signal 61");
/* 62 */	mesg[62].iname = 0;
		mesg[62].pname = SAVEMSG(687, "Signal 62");
/* 63 */	mesg[63].iname = 0;
		mesg[63].pname = SAVEMSG(688, "Signal 63");
/* 64 */	mesg[64].iname = 0;
		mesg[64].pname = SAVEMSG(689, "Signal 64");
#   endif /* SOLARIS2 >= 23 */
#  else /* !SOLARIS2 */
/* 32 */	mesg[2].iname = 0;
		mesg[2].pname = SAVEMSG(690, "Maximum number of signals");
#  endif /* SOLARIS2 */
# endif /* SYSVREL > 3 */

# if defined(ISC) && defined(POSIX) 
#  define _sigextra_
/* 20 */	mesg[2].iname = "WINCH";
		mesg[2].pname = SAVEMSG(691, "Window change");
		/* SIGPHONE used only for UNIXPC */
/* 21 */	mesg[21].iname = 0;
		mesg[21].pname = SAVEMSG(692, "Unused");
/* 22 */	mesg[22].iname = "POLL";
		mesg[22].pname = SAVEMSG(693, "Pollable event occured");
/* 23 */	mesg[23].iname = "CONT";
		mesg[23].pname = SAVEMSG(694, "Continued");
/* 24 */	mesg[24].iname = "STOP";
		mesg[24].pname = SAVEMSG_STOP;
/* 25 */	mesg[25].iname = "TSTP";
		mesg[25].pname = SAVEMSG_TSTP;
/* 26 */	mesg[26].iname = "TTIN";
		mesg[26].pname = SAVEMSG_TTIN;
/* 27 */	mesg[27].iname = "TTOU";
		mesg[27].pname = SAVEMSG_TTOU;
/* 28 */	mesg[28].iname = 0;
		mesg[28].pname = SAVEMSG(699, "number of signals");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(700, "Reserved");
/* 30 */	mesg[30].iname = 0;
		mesg[30].pname = SAVEMSG(701, "Reserved");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(702, "Reserved");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(703, "Maximum number of signals");
# endif /* ISC && POSIX */

# if defined(SCO) && defined(POSIX) 
#  define _sigextra_
/* 20 */	mesg[20].iname = "WINCH";
		mesg[20].pname = SAVEMSG(704, "Window change");
		/* SIGPHONE used only for UNIXPC */
/* 21 */	mesg[21].iname = 0;
		mesg[21].pname = SAVEMSG(705, "Unused");
/* 22 */	mesg[22].iname = "POLL";
		mesg[22].pname = SAVEMSG(706, "Pollable event occured");
/* 23 */	mesg[23].iname = "STOP";
		mesg[23].pname = SAVEMSG_STOP;
/* 24 */	mesg[24].iname = "TSTP";
		mesg[24].pname = SAVEMSG_TSTP;
/* 25 */	mesg[25].iname = "CONT";
		mesg[25].pname = SAVEMSG(709, "Continued");
/* 26 */	mesg[26].iname = "TTIN";
		mesg[26].pname = SAVEMSG_TTIN;
/* 27 */	mesg[27].iname = "TTOU";
		mesg[27].pname = SAVEMSG_TTOU;
/* 28 */	mesg[28].iname = 0;
		mesg[28].pname = SAVEMSG(712, "number of signals");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(713, "Reserved");
/* 30 */	mesg[30].iname = 0;
		mesg[30].pname = SAVEMSG(714, "Reserved");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(715, "Reserved");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(716, "Maximum number of signals");
# endif /* SCO && POSIX */

# if defined(IRIS4D) && (SYSVREL < 4)
#  define _sigextra_
/* 20 */	mesg[4].iname = "STOP";
		mesg[4].pname = SAVEMSG_STOP;
/* 21 */	mesg[21].iname = "TSTP";
		mesg[21].pname = SAVEMSG_TSTP;
/* 22 */	mesg[22].iname = "POLL";
		mesg[22].pname = SAVEMSG(719, "Stream I/O pending");
/* 23 */	mesg[23].iname = "IO";
		mesg[23].pname = SAVEMSG(720, "Asynchronous I/O (select)");
/* 24 */	mesg[24].iname = "URG";
		mesg[24].pname = SAVEMSG(721, "Urgent condition on IO channel");
/* 25 */	mesg[25].iname = "WINCH";
		mesg[25].pname = SAVEMSG(722, "Window changed");
/* 26 */	mesg[26].iname = "VTALRM";
		mesg[26].pname = SAVEMSG(723, "Virtual time alarm");
/* 27 */	mesg[27].iname = "PROF";
		mesg[27].pname = SAVEMSG(724, "Profiling time alarm");
/* 28 */	mesg[28].iname = "CONT";
		mesg[28].pname = SAVEMSG(725, "Continued");
/* 29 */	mesg[29].iname = "TTIN";
		mesg[29].pname = SAVEMSG_TTIN;
/* 30 */	mesg[30].iname = "TTOU";
		mesg[30].pname = SAVEMSG_TTOU;
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(728, "Signal 31");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(729, "Signal 32");
# endif /* IRIS4D */

# if defined(IRIS3D) && (SYSVREL < 4)
#  define _sigextra_
/* 20 */	mesg[4].iname = 0;
		mesg[4].pname = SAVEMSG(730, "Signal 20");
/* 21 */	mesg[21].iname = 0;
		mesg[21].pname = SAVEMSG(731, "Signal 21");
/* 22 */	mesg[22].iname = 0;
		mesg[22].pname = SAVEMSG(732, "Signal 22");
/* 23 */	mesg[23].iname = 0;
		mesg[23].pname = SAVEMSG(733, "Signal 23");
/* 24 */	mesg[24].iname = 0;
		mesg[24].pname = SAVEMSG(734, "Signal 24");
/* 25 */	mesg[25].iname = "WINCH";
		mesg[25].pname = SAVEMSG(735, "Window changed");
/* 26 */	mesg[26].iname = "IO";
		mesg[26].pname = SAVEMSG(736, "Asynchronous I/O (select)");
/* 27 */	mesg[27].iname = "URG";
		mesg[27].pname = SAVEMSG(737, "Urgent condition on IO channel");
/* 28 */	mesg[28].iname = "POLL";
		mesg[28].pname = SAVEMSG(738, "Stream I/O pending");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(739, "Signal 29");
/* 30 */	mesg[30].iname = 0;
		mesg[30].pname = SAVEMSG(740, "Signal 30");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(741, "Signal 31");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(742, "Signal 32");
# endif /* IRIS3D */

# ifdef apollo
#  define _sigextra_
/* 20 */	mesg[3].iname = "STOP";
		mesg[3].pname = SAVEMSG_STOP;
/* 21 */	mesg[21].iname = "TSTP";
		mesg[21].pname = SAVEMSG_TSTP;
/* 22 */	mesg[22].iname = "CONT";
		mesg[22].pname = SAVEMSG(745, "Continued");
/* 23 */	mesg[23].iname = "CHLD";
		mesg[23].pname = SAVEMSG(746, "Child stopped or exited");
/* 24 */	mesg[24].iname = "TTIN";
		mesg[24].pname = SAVEMSG_TTIN;
/* 25 */	mesg[25].iname = "TTOU";
		mesg[25].pname = SAVEMSG_TTOU;
/* 26 */	mesg[26].iname = "IO";
		mesg[26].pname = SAVEMSG(749, "Asynchronous I/O (select)");
/* 27 */	mesg[27].iname = "XCPU";
		mesg[27].pname = SAVEMSG(750, "Cputime limit exceeded");
/* 28 */	mesg[28].iname = "XFSZ";
		mesg[28].pname = SAVEMSG(751, "Filesize limit exceeded");
/* 29 */	mesg[29].iname = "VTALRM";
		mesg[29].pname = SAVEMSG(752, "Virtual time alarm");
/* 30 */	mesg[30].iname = "PROF";
		mesg[30].pname = SAVEMSG(753, "Profiling time alarm");
/* 31 */	mesg[31].iname = "URG";
		mesg[31].pname = SAVEMSG(754, "Urgent condition on IO channel");
/* 32 */	mesg[32].iname = "WINCH";
		mesg[32].pname = SAVEMSG(755, "Window changed");
# endif /* apollo */

# ifdef masscomp
#  define _sigextra_
/* 20 */	mesg[20].iname = "STOP";
		mesg[20].pname = SAVEMSG_STOP;
/* 21 */	mesg[21].iname = "TSTP";
		mesg[21].pname = SAVEMSG_TSTP;
/* 22 */	mesg[22].iname = "CONT";
		mesg[22].pname = SAVEMSG(758, "Continued");
/* 23 */	mesg[23].iname = "TTIN";
		mesg[23].pname = SAVEMSG_TTIN;
/* 24 */	mesg[24].iname = "TTOU";
		mesg[24].pname = SAVEMSG_TTOU;
/* 25 */	mesg[25].iname = "TINT";
		mesg[25].pname = SAVEMSG(761, "New input character");
/* 26 */	mesg[26].iname = "XCPU";
		mesg[26].pname = SAVEMSG(762, "Cputime limit exceeded");
/* 27 */	mesg[27].iname = "XFSZ";
		mesg[27].pname = SAVEMSG(763, "Filesize limit exceeded");
/* 28 */	mesg[28].iname = "WINCH";
		mesg[28].pname = SAVEMSG(764, "Window changed");
/* 29 */	mesg[29].iname = "URG";
		mesg[29].pname = SAVEMSG(765, "Urgent condition on IO channel");
/* 30 */	mesg[30].iname = "VTALRM";
		mesg[30].pname = SAVEMSG(766, "Virtual time alarm");
/* 31 */	mesg[31].iname = "PROF";
		mesg[31].pname = SAVEMSG(767, "Profiling time alarm");
/* 32 */	mesg[32].iname = "IO";
		mesg[32].pname = SAVEMSG(768, "Asynchronous I/O (select)");
# endif /* masscomp */

# ifdef aiws
#  define _sigextra_
/* 20 */	mesg[20].iname = 0;
		mesg[20].pname = SAVEMSG(769, "Signal 20");
/* 21 */	mesg[21].iname = 0;
		mesg[21].pname = SAVEMSG(770, "Signal 21");
/* 22 */	mesg[22].iname = 0;
		mesg[22].pname = SAVEMSG(771, "Signal 22");
/* 23 */	mesg[23].iname = "AIO";
		mesg[23].pname = SAVEMSG(772, "LAN Asyncronous I/O");
/* 24 */	mesg[24].iname = "PTY";
		mesg[24].pname = SAVEMSG(773, "PTY read/write availability");
/* 25 */	mesg[25].iname = "IOINT";
		mesg[25].pname = SAVEMSG(774, "I/O intervention required");
/* 26 */	mesg[26].iname = "GRANT";
		mesg[26].pname = SAVEMSG(775, "monitor mode granted");
/* 27 */	mesg[27].iname = "RETRACT";
		mesg[27].pname = SAVEMSG(776, "monitor mode retracted");
/* 28 */	mesg[28].iname = "WINCH";
		mesg[28].pname = SAVEMSG(777, "Window size changed");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(778, "Signal 29");
/* 30 */	mesg[30].iname = "SOUND";
		mesg[30].pname = SAVEMSG(779, "sound completed");
/* 31 */	mesg[31].iname = "SAVEMSG";
		mesg[31].pname = SAVEMSG(780, "input hft data pending");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(781, "Signal 32");
# endif /* aiws */

# if defined(m88k) || defined(__m88k__)	/* Motorola 88100: POSIX/BCS signals */
#  define _sigextra_
#  define _64sig_
/* 20 */	mesg[88].iname = "WINCH";
		mesg[88].pname = SAVEMSG(782, "Window changed");
/* 21 */	mesg[21].iname = 0;
		mesg[21].pname = SAVEMSG(783, "Signal 21");
/* 22 */	mesg[22].iname = "POLL";
		mesg[22].pname = SAVEMSG(784, "Stream I/O pending");
/* 23 */	mesg[23].iname = "STOP";
		mesg[23].pname = SAVEMSG_STOP;
/* 24 */	mesg[24].iname = "TSTP";
		mesg[24].pname = SAVEMSG_TSTP;
/* 25 */	mesg[25].iname = "CONT";
		mesg[25].pname = SAVEMSG(787, "Continued");
/* 26 */	mesg[26].iname = "TTIN";
		mesg[26].pname = SAVEMSG_TTIN;
/* 27 */	mesg[27].iname = "TTOU";
		mesg[27].pname = SAVEMSG_TTOU;
/* 28 */	mesg[28].iname = 0;
		mesg[28].pname = SAVEMSG(790, "Signal 28");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(791, "Signal 29");
/* 30 */	mesg[30].iname = 0;
		mesg[30].pname = SAVEMSG(792, "Signal 30");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(793, "Signal 31");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(794, "Signal 32");
/* 33 */	mesg[33].iname = "URG";
		mesg[33].pname = SAVEMSG(795, "Urgent condition on IO channel");
/* 34 */	mesg[34].iname = "IO";
		mesg[34].pname = SAVEMSG(796, "Asynchronous I/O (select)");
/* 35 */	mesg[35].iname = "XCPU";
		mesg[35].pname = SAVEMSG(797, "Cputime limit exceeded");
/* 36 */	mesg[36].iname = "XFSZ";
		mesg[36].pname = SAVEMSG(798, "Filesize limit exceeded");
/* 37 */	mesg[37].iname = "VTALRM";
		mesg[37].pname = SAVEMSG(799, "Virtual time alarm");
/* 38 */	mesg[38].iname = "PROF";
		mesg[38].pname = SAVEMSG(800, "Profiling time alarm");
/* 39 */	mesg[39].iname = 0;
		mesg[39].pname = SAVEMSG(801, "Signal 39");
/* 40 */	mesg[40].iname = "LOST";
		mesg[40].pname = SAVEMSG(802, "Resource lost");
/* 41 */	mesg[41].iname = 0;
		mesg[41].pname = SAVEMSG(803, "Signal 41");
/* 42 */	mesg[42].iname = 0;
		mesg[42].pname = SAVEMSG(804, "Signal 42");
/* 43 */	mesg[43].iname = 0;
		mesg[43].pname = SAVEMSG(805, "Signal 43");
/* 44 */	mesg[44].iname = 0;
		mesg[44].pname = SAVEMSG(806, "Signal 44");
/* 45 */	mesg[45].iname = 0;
		mesg[45].pname = SAVEMSG(807, "Signal 45");
/* 46 */	mesg[46].iname = 0;
		mesg[46].pname = SAVEMSG(808, "Signal 46");
/* 47 */	mesg[47].iname = 0;
		mesg[47].pname = SAVEMSG(809, "Signal 47");
/* 48 */	mesg[48].iname = 0;
		mesg[48].pname = SAVEMSG(810, "Signal 48");
/* 49 */	mesg[49].iname = 0;
		mesg[49].pname = SAVEMSG(811, "Signal 49");
/* 50 */	mesg[50].iname = 0;
		mesg[50].pname = SAVEMSG(812, "Signal 50");
/* 51 */	mesg[51].iname = 0;
		mesg[51].pname = SAVEMSG(813, "Signal 51");
/* 52 */	mesg[52].iname = 0;
		mesg[52].pname = SAVEMSG(814, "Signal 52");
/* 53 */	mesg[53].iname = 0;
		mesg[53].pname = SAVEMSG(815, "Signal 53");
/* 54 */	mesg[54].iname = 0;
		mesg[54].pname = SAVEMSG(816, "Signal 54");
/* 55 */	mesg[55].iname = 0;
		mesg[55].pname = SAVEMSG(817, "Signal 55");
/* 56 */	mesg[56].iname = 0;
		mesg[56].pname = SAVEMSG(818, "Signal 56");
/* 57 */	mesg[57].iname = 0;
		mesg[57].pname = SAVEMSG(819, "Signal 57");
/* 58 */	mesg[58].iname = 0;
		mesg[58].pname = SAVEMSG(820, "Signal 58");
/* 59 */	mesg[59].iname = 0;
		mesg[59].pname = SAVEMSG(821, "Signal 59");
/* 60 */	mesg[60].iname = 0;
		mesg[60].pname = SAVEMSG(822, "Signal 60");
/* 61 */	mesg[61].iname = 0;
		mesg[61].pname = SAVEMSG(823, "Signal 61");
/* 62 */	mesg[62].iname = 0;
		mesg[62].pname = SAVEMSG(824, "Signal 62");
/* 63 */	mesg[63].iname = 0;
		mesg[63].pname = SAVEMSG(825, "Signal 63");
/* 64 */	mesg[64].iname = 0;
		mesg[64].pname = SAVEMSG(826, "Signal 64");
# endif /* m88k || __m88k__ */

# ifdef IBMAIX
#  define _sigextra_
#  define _64sig_
/* 16 */	mesg[88].iname = "URG";
		mesg[88].pname = SAVEMSG(827, "Urgent condition on IO channel");
/* 17 */	mesg[17].iname = "STOP";
		mesg[17].pname = SAVEMSG_STOP;
/* 18 */	mesg[18].iname = "TSTP";
		mesg[18].pname = SAVEMSG_TSTP;
/* 19 */	mesg[19].iname = "CONT";
		mesg[19].pname = SAVEMSG(830, "Continued");
/* 20 */	mesg[20].iname = "CHLD";
		mesg[20].pname = SAVEMSG(831, "Child exited");
/* 21 */	mesg[21].iname = "TTIN";
		mesg[21].pname = SAVEMSG_TTIN;
/* 22 */	mesg[22].iname = "TTOU";
		mesg[22].pname = SAVEMSG_TTOU;
/* 23 */	mesg[23].iname = "IO";
		mesg[23].pname = SAVEMSG(834, "IO possible interrupt");
/* 24 */	mesg[24].iname = "XCPU";
		mesg[24].pname = SAVEMSG(835, "Cputime limit exceeded");
/* 25 */	mesg[25].iname = "XFSZ";
		mesg[25].pname = SAVEMSG(836, "Filesize limit exceeded");
/* 26 */	mesg[26].iname = 0;
		mesg[26].pname = SAVEMSG(837, "Signal 26");
/* 27 */	mesg[27].iname = "SAVEMSG";
		mesg[27].pname = SAVEMSG(838, "Data in HFT ring buffer");
/* 28 */	mesg[28].iname = "WINCH";
		mesg[28].pname = SAVEMSG(839, "Window size changed");
/* 29 */	mesg[29].iname = "PWR";
		mesg[29].pname = SAVEMSG(840, "Power failure");
/* 30 */	mesg[30].iname = "USR1";
		mesg[30].pname = SAVEMSG(841, "User signal 1");
/* 31 */	mesg[31].iname = "USR2";
		mesg[31].pname = SAVEMSG(842, "User signal 2");
/* 32 */	mesg[32].iname = "PROF";
		mesg[32].pname = SAVEMSG(843, "Profiling time alarm");
/* 33 */	mesg[33].iname = "DANGER";
		mesg[33].pname = SAVEMSG(844, "System Crash Imminent");
/* 34 */	mesg[34].iname = "VTALRM";
		mesg[34].pname = SAVEMSG(845, "Virtual time alarm");
/* 35 */	mesg[35].iname = "MIGRATE";
		mesg[35].pname = SAVEMSG(846, "Migrate process");
/* 36 */	mesg[36].iname = "PRE";
		mesg[36].pname = SAVEMSG(847, "Programming exception");
/* 37 */	mesg[37].iname = 0;
		mesg[37].pname = SAVEMSG(848, "Signal 37");
/* 38 */	mesg[38].iname = 0;
		mesg[38].pname = SAVEMSG(849, "Signal 38");
/* 39 */	mesg[39].iname = 0;
		mesg[39].pname = SAVEMSG(850, "Signal 39");
/* 40 */	mesg[40].iname = 0;
		mesg[40].pname = SAVEMSG(851, "Signal 40");
/* 41 */	mesg[41].iname = 0;
		mesg[41].pname = SAVEMSG(852, "Signal 41");
/* 42 */	mesg[42].iname = 0;
		mesg[42].pname = SAVEMSG(853, "Signal 42");
/* 43 */	mesg[43].iname = 0;
		mesg[43].pname = SAVEMSG(854, "Signal 43");
/* 44 */	mesg[44].iname = 0;
		mesg[44].pname = SAVEMSG(855, "Signal 44");
/* 45 */	mesg[45].iname = 0;
		mesg[45].pname = SAVEMSG(856, "Signal 45");
/* 46 */	mesg[46].iname = 0;
		mesg[46].pname = SAVEMSG(857, "Signal 46");
/* 47 */	mesg[47].iname = 0;
		mesg[47].pname = SAVEMSG(858, "Signal 47");
/* 48 */	mesg[48].iname = 0;
		mesg[48].pname = SAVEMSG(859, "Signal 48");
/* 49 */	mesg[49].iname = 0;
		mesg[49].pname = SAVEMSG(860, "Signal 49");
/* 50 */	mesg[50].iname = 0;
		mesg[50].pname = SAVEMSG(861, "Signal 50");
/* 51 */	mesg[51].iname = 0;
		mesg[51].pname = SAVEMSG(862, "Signal 51");
/* 52 */	mesg[52].iname = 0;
		mesg[52].pname = SAVEMSG(863, "Signal 52");
/* 53 */	mesg[53].iname = 0;
		mesg[53].pname = SAVEMSG(864, "Signal 53");
/* 54 */	mesg[54].iname = 0;
		mesg[54].pname = SAVEMSG(865, "Signal 54");
/* 55 */	mesg[55].iname = 0;
		mesg[55].pname = SAVEMSG(866, "Signal 55");
/* 56 */	mesg[56].iname = 0;
		mesg[56].pname = SAVEMSG(867, "Signal 56");
/* 57 */	mesg[57].iname = 0;
		mesg[57].pname = SAVEMSG(868, "Signal 57");
/* 58 */	mesg[58].iname = 0;
		mesg[58].pname = SAVEMSG(869, "Signal 58");
/* 59 */	mesg[59].iname = 0;
		mesg[59].pname = SAVEMSG(870, "Signal 59");
/* 60 */	mesg[60].iname = "GRANT";
		mesg[60].pname = SAVEMSG(871, "HFT monitor mode granted");
/* 61 */	mesg[61].iname = "RETRACT";
		mesg[61].pname = SAVEMSG(872, "HFT monitor mode should be relinguished");
/* 62 */	mesg[62].iname = "SOUND";
		mesg[62].pname = SAVEMSG(873, "HFT sound control has completed");
#  ifdef SIGSAK
/* 63 */	mesg[63].iname = "SAK";
		mesg[63].pname = SAVEMSG(874, "Secure attention key");
#  else
/* 63 */	mesg[63].iname = 0;
		mesg[63].pname = SAVEMSG(875, "Signal 63");
#  endif
/* 64 */	mesg[64].iname = 0;
		mesg[64].pname = SAVEMSG(876, "Signal 64");
# endif /* IBMAIX */

# ifdef _SEQUENT_
#  define _sigextra_
/* 20 */	mesg[20].iname = "WINCH";
		mesg[20].pname = SAVEMSG(877, "Window changed");
/* 21 */	mesg[21].iname = 0;
		mesg[21].pname = SAVEMSG(878, "Signal 21");
/* 22 */	mesg[22].iname = "POLL";
		mesg[22].pname = SAVEMSG(879, "Stream I/O pending");
/* 23 */	mesg[23].iname = "STOP";
		mesg[23].pname = SAVEMSG_STOP;
/* 24 */	mesg[24].iname = "TSTP";
		mesg[24].pname = SAVEMSG_TSTP;
/* 25 */	mesg[25].iname = "CONT";
		mesg[25].pname = SAVEMSG(882, "Continued");
/* 26 */	mesg[26].iname = "TTIN";
		mesg[26].pname = SAVEMSG_TTIN;
/* 27 */	mesg[27].iname = "TTOU";
		mesg[27].pname = SAVEMSG_TTOU;
/* 28 */	mesg[28].iname = 0;
		mesg[28].pname = SAVEMSG(885, "Signal 28");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(886, "Signal 29");
/* 30 */	mesg[30].iname = 0;
		mesg[30].pname = SAVEMSG(887, "Signal 30");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(888, "Signal 31");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(889, "Signal 32");
# endif /* _SEQUENT_ */

# ifdef linux
#  define _sigextra_
/* 16 */	mesg[16].iname = 0;
		mesg[16].pname = SAVEMSG(890, "Signal 16");
/* 17 */	mesg[17].iname = "CHLD";
		mesg[17].pname = SAVEMSG(891, "Child exited");
/* 18 */	mesg[18].iname = "CONT";
		mesg[18].pname = SAVEMSG(892, "Continued");
/* 19 */	mesg[19].iname = "STOP";
		mesg[19].pname = SAVEMSG_STOP;
/* 20 */	mesg[20].iname = "TSTP";
		mesg[20].pname = SAVEMSG_TSTP;
/* 21 */	mesg[21].iname = "TTIN";
		mesg[21].pname = SAVEMSG_TTIN;
/* 22 */	mesg[22].iname = "TTOU";
		mesg[22].pname = SAVEMSG_TTOU;
/* 23 */	mesg[23].iname = 0;
		mesg[23].pname = SAVEMSG(897, "Signal 23");
/* 24 */	mesg[24].iname = 0;
		mesg[24].pname = SAVEMSG(898, "Signal 24");
/* 25 */	mesg[25].iname = 0;
		mesg[25].pname = SAVEMSG(899, "Signal 25");
/* 26 */	mesg[26].iname = 0;
		mesg[26].pname = SAVEMSG(900, "Signal 26");
/* 27 */	mesg[27].iname = 0;
		mesg[27].pname = SAVEMSG(901, "Signal 27");
/* 28 */	mesg[28].iname = "WINCH";
		mesg[28].pname = SAVEMSG(902, "Window changed");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(903, "Signal 29");
/* 30 */	mesg[30].iname = 0;
		mesg[30].pname = SAVEMSG(904, "Signal 30");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(905, "Signal 31");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(906, "Signal 32");
# endif /* linux */

# ifdef hcx
#  define _64sig_	/* just for the sake of SIGRESCHED */
#  define _sigextra_
/* 20 */	mesg[64].iname = "URG";
		mesg[64].pname = SAVEMSG(907, "Urgent condition on IO channel");
/* 21 */	mesg[21].iname = "STOP";
		mesg[21].pname = SAVEMSG_STOP;
/* 22 */	mesg[22].iname = "TSTP";
		mesg[22].pname = SAVEMSG_TSTP;
/* 23 */	mesg[23].iname = "CONT";
		mesg[23].pname = SAVEMSG(910, "Continued");
/* 24 */	mesg[24].iname = "TTIN";
		mesg[24].pname = SAVEMSG_TTIN;
/* 25 */	mesg[25].iname = "TTOU";
		mesg[25].pname = SAVEMSG_TTOU;
/* 26 */	mesg[26].iname = "IO";
		mesg[26].pname = SAVEMSG(913, "Asynchronous I/O (select)");
/* 27 */	mesg[27].iname = "XCPU";
		mesg[27].pname = SAVEMSG(914, "Cputime limit exceeded");
/* 28 */	mesg[28].iname = "XFSZ";
		mesg[28].pname = SAVEMSG(915, "Filesize limit exceeded");
/* 29 */	mesg[29].iname = "VTALRM";
		mesg[29].pname = SAVEMSG(916, "Virtual time alarm");
/* 30 */	mesg[30].iname = "PROF";
		mesg[30].pname = SAVEMSG(917, "Profiling time alarm");
/* 31 */	mesg[31].iname = "LOST";
		mesg[31].pname = SAVEMSG(918, "Resource lost");
/* 32 */	mesg[32].iname = "WINCH";
		mesg[32].pname = SAVEMSG(919, "Window changed");
/* 33 */	mesg[33].iname = "RESCHED";
		mesg[33].pname = SAVEMSG(920, "Reschedule");
/* 34 */	mesg[34].iname = 0;
		mesg[34].pname = SAVEMSG(921, "Signal 34");
/* 35 */	mesg[35].iname = 0;
		mesg[35].pname = SAVEMSG(922, "Signal 35");
/* 36 */	mesg[36].iname = 0;
		mesg[36].pname = SAVEMSG(923, "Signal 36");
/* 37 */	mesg[37].iname = 0;
		mesg[37].pname = SAVEMSG(924, "Signal 37");
/* 38 */	mesg[38].iname = 0;
		mesg[38].pname = SAVEMSG(925, "Signal 38");
/* 39 */	mesg[39].iname = 0;
		mesg[39].pname = SAVEMSG(926, "Signal 39");
/* 40 */	mesg[40].iname = 0;
		mesg[40].pname = SAVEMSG(927, "Signal 40");
/* 41 */	mesg[41].iname = 0;
		mesg[41].pname = SAVEMSG(928, "Signal 41");
/* 42 */	mesg[42].iname = 0;
		mesg[42].pname = SAVEMSG(929, "Signal 42");
/* 43 */	mesg[43].iname = 0;
		mesg[43].pname = SAVEMSG(930, "Signal 43");
/* 44 */	mesg[44].iname = 0;
		mesg[44].pname = SAVEMSG(931, "Signal 44");
/* 45 */	mesg[45].iname = 0;
		mesg[45].pname = SAVEMSG(932, "Signal 45");
/* 46 */	mesg[46].iname = 0;
		mesg[46].pname = SAVEMSG(933, "Signal 46");
/* 47 */	mesg[47].iname = 0;
		mesg[47].pname = SAVEMSG(934, "Signal 47");
/* 48 */	mesg[48].iname = 0;
		mesg[48].pname = SAVEMSG(935, "Signal 48");
/* 49 */	mesg[49].iname = 0;
		mesg[49].pname = SAVEMSG(936, "Signal 49");
/* 50 */	mesg[50].iname = 0;
		mesg[50].pname = SAVEMSG(937, "Signal 50");
/* 51 */	mesg[51].iname = 0;
		mesg[51].pname = SAVEMSG(938, "Signal 51");
/* 52 */	mesg[52].iname = 0;
		mesg[52].pname = SAVEMSG(939, "Signal 52");
/* 53 */	mesg[53].iname = 0;
		mesg[53].pname = SAVEMSG(940, "Signal 53");
/* 54 */	mesg[54].iname = 0;
		mesg[54].pname = SAVEMSG(941, "Signal 54");
/* 55 */	mesg[55].iname = 0;
		mesg[55].pname = SAVEMSG(942, "Signal 55");
/* 56 */	mesg[56].iname = 0;
		mesg[56].pname = SAVEMSG(943, "Signal 56");
/* 57 */	mesg[57].iname = 0;
		mesg[57].pname = SAVEMSG(944, "Signal 57");
/* 58 */	mesg[58].iname = 0;
		mesg[58].pname = SAVEMSG(945, "Signal 58");
/* 59 */	mesg[59].iname = 0;
		mesg[59].pname = SAVEMSG(946, "Signal 59");
/* 60 */	mesg[60].iname = 0;
		mesg[60].pname = SAVEMSG(947, "Signal 60");
/* 61 */	mesg[61].iname = 0;
		mesg[61].pname = SAVEMSG(948, "Signal 61");
/* 62 */	mesg[62].iname = 0;
		mesg[62].pname = SAVEMSG(949, "Signal 62");
/* 63 */	mesg[63].iname = 0;
		mesg[63].pname = SAVEMSG(950, "Signal 63");
# endif /* hcx */

# ifndef _sigextra_
#  ifndef UNIXPC
/* 20 */	mesg[20].iname = 0;
		mesg[20].pname = SAVEMSG(951, "Signal 20");
/* 21 */	mesg[21].iname = 0;
		mesg[21].pname = SAVEMSG(952, "Signal 21");
#  endif /* !UNIXPC */
/* 22 */	mesg[22].iname = 0;
		mesg[22].pname = SAVEMSG(953, "Signal 22");
/* 23 */	mesg[23].iname = 0;
		mesg[23].pname = SAVEMSG(954, "Signal 23");
/* 24 */	mesg[24].iname = 0;
		mesg[24].pname = SAVEMSG(955, "Signal 24");
/* 25 */	mesg[25].iname = 0;
		mesg[25].pname = SAVEMSG(956, "Signal 25");
/* 26 */	mesg[26].iname = 0;
		mesg[26].pname = SAVEMSG(957, "Signal 26");
/* 27 */	mesg[27].iname = 0;
		mesg[27].pname = SAVEMSG(958, "Signal 27");
/* 28 */	mesg[28].iname = 0;
		mesg[28].pname = SAVEMSG(959, "Signal 28");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(960, "Signal 29");
/* 30 */	mesg[30].iname = 0;
		mesg[30].pname = SAVEMSG(961, "Signal 30");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(962, "Signal 31");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(963, "Signal 32");
# endif /* _sigextra_ */

#else /* bsd */

# ifdef _sigextra_
#  undef  _sigextra_
# endif /* _sigextra_ */

# ifdef _VMS_POSIX
#  define _sigextra_
/* 16 */	mesg[16].iname = 0;
		mesg[16].pname = SAVEMSG(964, "Signal 16");
/* 17 */	mesg[17].iname = 0;
		mesg[17].pname = SAVEMSG(965, "Signal 17");
/* 18 */	mesg[18].iname = "USR1";
		mesg[18].pname = SAVEMSG(966, "User defined signal 1");
/* 19 */	mesg[19].iname = "USR2";
		mesg[19].pname = SAVEMSG(967, "User defined signal 2");
/* 20 */	mesg[20].iname = "CHLD";
		mesg[20].pname = SAVEMSG(968, "Child exited");
/* 21 */	mesg[21].iname = "CONT";
		mesg[21].pname = SAVEMSG(969, "Continued");
/* 22 */	mesg[22].iname = "STOP";
		mesg[22].pname = SAVEMSG_STOP;
/* 23 */	mesg[23].iname = "TSTP";
		mesg[23].pname = SAVEMSG_TSTP;
/* 24 */	mesg[24].iname = "TTIN";
		mesg[24].pname = SAVEMSG_TTIN;
/* 25 */	mesg[25].iname = "TTOU";
		mesg[25].pname = SAVEMSG_TTOU;
/* 26 */        mesg[26].iname = "DEBUG";
		mesg[26].pname = SAVEMSG(974, "Signaling SS$_DEBUG");
# else /* BSD */
/* 16 */	mesg[16].iname = "URG";
		mesg[16].pname = SAVEMSG(975, "Urgent condition on IO channel");
/* 17 */	mesg[17].iname = "STOP";
		mesg[17].pname = SAVEMSG_STOP;
/* 18 */	mesg[18].iname = "TSTP";
		mesg[18].pname = SAVEMSG_TSTP;
/* 19 */	mesg[19].iname = "CONT";
		mesg[19].pname = SAVEMSG(978, "Continued");
/* 20 */	mesg[20].iname = "CHLD";
		mesg[20].pname = SAVEMSG(979, "Child exited");
/* 21 */	mesg[21].iname = "TTIN";
		mesg[21].pname = SAVEMSG_TTIN;
/* 22 */	mesg[22].iname = "TTOU";
		mesg[22].pname = SAVEMSG_TTOU;
/* 23 */	mesg[23].iname = "IO";
		mesg[23].pname = SAVEMSG(982, "IO possible interrupt");
/* 24 */	mesg[24].iname = "XCPU";
		mesg[24].pname = SAVEMSG(983, "Cputime limit exceeded");
/* 25 */	mesg[25].iname = "XFSZ";
		mesg[25].pname = SAVEMSG(984, "Filesize limit exceeded");
/* 26 */	mesg[26].iname = "VTALRM";
		mesg[26].pname = SAVEMSG(985, "Virtual time alarm");
/* 27 */	mesg[27].iname = "PROF";
		mesg[27].pname = SAVEMSG(986, "Profiling time alarm");
# endif /* _VMS_POSIX */

# ifndef _sigextra_
#  if defined(RENO) || defined(BSD4_4) || defined(__hp_osf) || defined(DECOSF1)
#   define _sigextra_
/* 28 */	mesg[4].iname = "WINCH";
		mesg[4].pname = SAVEMSG(987, "Window size changed");
/* 29 */	mesg[29].iname = "INFO";
		mesg[29].pname = SAVEMSG(988, "Information request");
/* 30 */	mesg[30].iname = "USR1";
		mesg[30].pname = SAVEMSG(989, "User defined signal 1");
/* 31 */	mesg[31].iname = "USR2";
		mesg[31].pname = SAVEMSG(990, "User defined signal 2");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(991, "Signal 32");
#  endif /* RENO || BSD4_4 || __hp_osf || DECOSF1 */
# endif /* !_sigextra_ */

# ifndef _sigextra_
#  if defined(SUNOS4) || defined(ultrix) || defined(hp9000) || defined(convex)
#   define _sigextra_
/* 28 */	mesg[4].iname = "WINCH";
		mesg[4].pname = SAVEMSG(992, "Window changed");
/* 29 */	mesg[29].iname = "LOST";
		mesg[29].pname = SAVEMSG(993, "Resource lost");
/* 30 */	mesg[30].iname = "USR1";
		mesg[30].pname = SAVEMSG(994, "User signal 1");
/* 31 */	mesg[31].iname = "USR2";
		mesg[31].pname = SAVEMSG(995, "User signal 2");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(996, "Signal 32");
#  endif /* SUNOS4 || ultrix || hp9000 || convex */
# endif /* !_sigextra_ */

# ifndef _sigextra_
#  ifdef pyr
#   define _sigextra_
/* 28 */	mesg[4].iname = "USR1";
		mesg[4].pname = SAVEMSG(997, "User signal 1");
/* 29 */	mesg[29].iname = "USR2";
		mesg[29].pname = SAVEMSG(998, "User signal 2");
/* 30 */	mesg[30].iname = "PWR";
		mesg[30].pname = SAVEMSG(999, "Power failure");
/* 31 */	mesg[31].iname = 0;
		mesg[31].pname = SAVEMSG(1000, "Signal 31");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(1001, "Signal 32");
#  endif /* pyr */
# endif /* !_sigextra_ */

# ifndef _sigextra_
#  ifdef Lynx
#   define _sigextra_
/* 28 */	mesg[28].iname = "WINCH";
		mesg[28].pname = SAVEMSG(1002, "Window changed");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(1003, "Signal 29");
/* 30 */	mesg[30].iname = 0;
		mesg[30].pname = SAVEMSG(1004, "Signal 30");
/* 31 */	mesg[31].iname = "USR1";
		mesg[31].pname = SAVEMSG(1005, "User signal 1");
/* 32 */	mesg[32].iname = "PRIO";
		mesg[32].pname = SAVEMSG(1006, "Priority changed");
#  endif /* Lynx */
# endif /* _sigextra_ */

# ifndef _sigextra_
/* 28 */	mesg[28].iname = "WINCH";
		mesg[28].pname = SAVEMSG(1007, "Window size changed");
/* 29 */	mesg[29].iname = 0;
		mesg[29].pname = SAVEMSG(1008, "Signal 29");
/* 30 */	mesg[30].iname = "USR1";
		mesg[30].pname = SAVEMSG(1009, "User defined signal 1");
/* 31 */	mesg[31].iname = "USR2";
		mesg[31].pname = SAVEMSG(1010, "User defined signal 2");
/* 32 */	mesg[32].iname = 0;
		mesg[32].pname = SAVEMSG(1011, "Signal 32");
# endif /* _sigextra_ */

#endif /* (SYSVREL > 0) || DGUX || IBMAIX */

#if defined(POSIX) && !defined(_64sig_)
/* 33 */	mesg[0].iname = 0;
		mesg[0].pname = SAVEMSG(1012, "Signal 33");
/* 34 */	mesg[34].iname = 0;
		mesg[34].pname = SAVEMSG(1013, "Signal 34");
/* 35 */	mesg[35].iname = 0;
		mesg[35].pname = SAVEMSG(1014, "Signal 35");
/* 36 */	mesg[36].iname = 0;
		mesg[36].pname = SAVEMSG(1015, "Signal 36");
/* 37 */	mesg[37].iname = 0;
		mesg[37].pname = SAVEMSG(1016, "Signal 37");
/* 38 */	mesg[38].iname = 0;
		mesg[38].pname = SAVEMSG(1017, "Signal 38");
/* 39 */	mesg[39].iname = 0;
		mesg[39].pname = SAVEMSG(1018, "Signal 39");
/* 40 */	mesg[40].iname = 0;
		mesg[40].pname = SAVEMSG(1019, "Signal 40");
/* 41 */	mesg[41].iname = 0;
		mesg[41].pname = SAVEMSG(1020, "Signal 41");
/* 42 */	mesg[42].iname = 0;
		mesg[42].pname = SAVEMSG(1021, "Signal 42");
/* 43 */	mesg[43].iname = 0;
		mesg[43].pname = SAVEMSG(1022, "Signal 43");
/* 44 */	mesg[44].iname = 0;
		mesg[44].pname = SAVEMSG(1023, "Signal 44");
/* 45 */	mesg[45].iname = 0;
		mesg[45].pname = SAVEMSG(1024, "Signal 45");
/* 46 */	mesg[46].iname = 0;
		mesg[46].pname = SAVEMSG(1025, "Signal 46");
/* 47 */	mesg[47].iname = 0;
		mesg[47].pname = SAVEMSG(1026, "Signal 47");
/* 48 */	mesg[48].iname = 0;
		mesg[48].pname = SAVEMSG(1027, "Signal 48");
/* 49 */	mesg[49].iname = 0;
		mesg[49].pname = SAVEMSG(1028, "Signal 49");
/* 50 */	mesg[50].iname = 0;
		mesg[50].pname = SAVEMSG(1029, "Signal 50");
/* 51 */	mesg[51].iname = 0;
		mesg[51].pname = SAVEMSG(1030, "Signal 51");
/* 52 */	mesg[52].iname = 0;
		mesg[52].pname = SAVEMSG(1031, "Signal 52");
/* 53 */	mesg[53].iname = 0;
		mesg[53].pname = SAVEMSG(1032, "Signal 53");
/* 54 */	mesg[54].iname = 0;
		mesg[54].pname = SAVEMSG(1033, "Signal 54");
/* 55 */	mesg[55].iname = 0;
		mesg[55].pname = SAVEMSG(1034, "Signal 55");
/* 56 */	mesg[56].iname = 0;
		mesg[56].pname = SAVEMSG(1035, "Signal 56");
/* 57 */	mesg[57].iname = 0;
		mesg[57].pname = SAVEMSG(1036, "Signal 57");
/* 58 */	mesg[58].iname = 0;
		mesg[58].pname = SAVEMSG(1037, "Signal 58");
/* 59 */	mesg[59].iname = 0;
		mesg[59].pname = SAVEMSG(1038, "Signal 59");
/* 60 */	mesg[60].iname = 0;
		mesg[60].pname = SAVEMSG(1039, "Signal 60");
/* 61 */	mesg[61].iname = 0;
		mesg[61].pname = SAVEMSG(1040, "Signal 61");
/* 62 */	mesg[62].iname = 0;
		mesg[62].pname = SAVEMSG(1041, "Signal 62");
/* 63 */	mesg[63].iname = 0;
		mesg[63].pname = SAVEMSG(1042, "Signal 63");
/* 64 */	mesg[64].iname = 0;
		mesg[64].pname = SAVEMSG(1043, "Signal 64");
#endif /* POSIX && ! _64sig_ */

#ifdef POSIX
/* These are here for systems with bad NSIG */
/* 65 */	mesg[64].iname = 0;
		mesg[64].pname = SAVEMSG(1044, "Signal 65");
#else /* !POSIX */
/* 33 */	mesg[33].iname = 0;
		mesg[33].pname = SAVEMSG(1045, "Signal 33");
#endif /* POSIX */
};


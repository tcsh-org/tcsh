/* $Header: /u/christos/src/tcsh-6.02/RCS/sh.init.c,v 3.15 1992/05/09 04:03:53 christos Exp $ */
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

RCSID("$Id: sh.init.c,v 3.15 1992/05/09 04:03:53 christos Exp $")

#include "ed.h"
#include "tw.h"

/*
 * C shell
 */

#define	INF	0x7fffffff

struct	biltins bfunc[] = {
    { "@",		dolet,		0,	INF, },
    { "alias",		doalias,	0,	INF, },
    { "aliases",	doaliases,	0,	1, /* PWP */ },
    { "alloc",		showall,	0,	1, },
    { "bg",		dobg,		0,	INF, },
    { "bind",		dobind,		0,	2, },
    { "bindkey",	dobindkey,	0,	8, },
    { "break",		dobreak,	0,	0, },
    { "breaksw",	doswbrk,	0,	0, },
    { "builtins", dobuiltins,	0,	0, },
#if defined(IIASA) || defined(KAI)
    { "bye",		goodbye,	0,	0, },
#endif
    { "case",		dozip,		0,	1, },
    { "cd",		dochngd,	0,	INF, },
    { "chdir",		dochngd,	0,	INF, },
    { "complete",	docomplete,	0,	INF, },
    { "continue",	docontin,	0,	0, },
    { "default",	dozip,		0,	0, },
    { "dirs",		dodirs,		0,	INF, },
    { "echo",		doecho,		0,	INF, },
    { "echotc",		doechotc,	0,	INF, },
    { "else",		doelse,		0,	INF, },
    { "end",		doend,		0,	0, },
    { "endif",		dozip,		0,	0, },
    { "endsw",		dozip,		0,	0, },
    { "eval",		doeval,		0,	INF, },
    { "exec",		execash,	1,	INF, },
    { "exit",		doexit,		0,	INF, },
    { "fg",		dofg,		0,	INF, },
    { "foreach",	doforeach,	3,	INF, },
#ifdef TCF
    { "getspath",	dogetspath,	0,	0, },
    { "getxvers",	 dogetxvers,	0,	0, },
#endif /* TCF */
#ifdef IIASA
    { "gd",		dopushd,	0,	INF, },
#endif
    { "glob",		doglob,		0,	INF, },
    { "goto",		dogoto,		1,	1, },
#ifdef VFORK
    { "hashstat",	hashstat,	0,	0, },
#endif
    { "history",	dohist,		0,	2, },
    { "if",		doif,		1,	INF, },
#ifdef apollo
    { "inlib", 		doinlib,	1,	INF, },
#endif
    { "jobs",		dojobs,		0,	1, },
    { "kill",		dokill,		1,	INF, },
#ifndef HAVENOLIMIT
    { "limit",		dolimit,	0,	3, },
#endif /* ! HAVENOLIMIT */
    { "linedit",	doecho,		0,	INF, },
#ifndef KAI
    { "log",		dolog,		0,	0, },
#endif
    { "login",		dologin,	0,	1, },
    { "logout",		dologout,	0,	0, },
    { "ls-F",		dolist,		0,	INF, },
#ifdef TCF
    { "migrate",	domigrate,	1,	INF, },
#endif /* TCF */
#ifdef NEWGRP
    { "newgrp",		donewgrp,	1,	1, },
#endif
    { "nice",		donice,		0,	INF, },
    { "nohup",		donohup,	0,	INF, },
    { "notify",		donotify,	0,	INF, },
    { "onintr",		doonintr,	0,	2, },
    { "popd",		dopopd,		0,	INF, },
    { "pushd",		dopushd,	0,	INF, },
#ifdef IIASA
    { "rd",		dopopd,		0,	INF, },
#endif
    { "rehash",		dohash,		0,	3, },
    { "repeat",		dorepeat,	2,	INF, },
#ifdef apollo
    { "rootnode",	dorootnode,	1,	1, },
#endif
    { "sched",		dosched,	0,	INF, },
    { "set",		doset,		0,	INF, },
    { "setenv",		dosetenv,	0,	2, },
#ifdef MACH
    { "setpath",	dosetpath,	0,	INF, },
#endif	/* MACH */
#ifdef TCF
    { "setspath",	dosetspath,	1,	INF, },
#endif /* TCF */
    { "settc",		dosettc,	2,	2, },
    { "setty", 		dosetty,	0,      INF },
#ifdef TCF
    { "setxvers",	dosetxvers,	0,	1, },
#endif /* TCF */
    { "shift",		shift,		0,	1, },
    { "source",		dosource,	1,	INF, },
    { "stop",		dostop,		1,	INF, },
    { "suspend",	dosuspend,	0,	0, },
    { "switch",		doswitch,	1,	INF, },
    { "telltc",		dotelltc,	0,	INF, },
    { "time",		dotime,		0,	INF, },
    { "umask",		doumask,	0,	1, },
    { "unalias",	unalias,	1,	INF, },
    { "uncomplete",	douncomplete,	1,	INF, },
    { "unhash",		dounhash,	0,	0, },
#ifdef masscomp
    { "universe",	douniverse,	0,	1, },
#endif
#ifndef HAVENOLIMIT
    { "unlimit",	dounlimit,	0,	INF, },
#endif /* !HAVENOLIMIT */
    { "unset",		unset,		1,	INF, },
    { "unsetenv",	dounsetenv,	1,	INF, },
#ifdef apollo
    { "ver",		dover,		0,	INF, },
#endif
    { "wait",		dowait,		0,	0, },
#ifdef WARP
    { "warp",		dowarp,		0,	2, },
#endif
#ifdef KAI
    { "watchlog",	dolog,		0,	0, },
#endif
    { "where",		dowhere,	1,	INF, },
    { "which",		dowhich,	1,	INF, },
    { "while",		dowhile,	1,	INF, },
};
int nbfunc = sizeof bfunc / sizeof *bfunc;

struct srch srchn[] = {
    { "@",		T_LET, },
    { "break",		T_BREAK, },
    { "breaksw",	T_BRKSW, },
    { "case",		T_CASE, },
    { "default", 	T_DEFAULT, },
    { "else",		T_ELSE, },
    { "end",		T_END, },
    { "endif",		T_ENDIF, },
    { "endsw",		T_ENDSW, },
    { "exit",		T_EXIT, },
    { "foreach", 	T_FOREACH, },
    { "goto",		T_GOTO, },
    { "if",		T_IF, },
    { "label",		T_LABEL, },
    { "set",		T_SET, },
    { "switch",		T_SWITCH, },
    { "while",		T_WHILE, },
};
int nsrchn = sizeof srchn / sizeof *srchn;

#ifdef SUSPENDED
# define MSG_STOP		"Suspended (signal)"
# define MSG_TSTP		"Suspended"
# define MSG_TTIN		"Suspended (tty input)"
# define MSG_TTOU		"Suspended (tty output)"
#else /* STOPPED */
# define MSG_STOP		"Stopped (signal)"
# define MSG_TSTP		"Stopped"
# define MSG_TTIN		"Stopped (tty input)"
# define MSG_TTOU		"Stopped (tty output)"
#endif /* SUSPENDED */
/*
 * Note: For some machines, (hpux eg.)
 * NSIG = number of signals + 1...
 * so we define 33 signals for 
 * everybody
 */
struct	mesg mesg[] = {
/*  0 */	0,		"",
/*  1 */	"HUP",		"Hangup",
/*  2 */	"INT",		"Interrupt",	
/*  3 */	"QUIT",		"Quit",
/*  4 */	"ILL",		"Illegal instruction",
/*  5 */	"TRAP",		"Trace/BPT trap",
#if SYSVREL > 3
/*  6 */	"ABRT",		"Abort",
#else /* SYSVREL > 3 */
/*  6 */	"IOT",		"IOT trap",
#endif /* SYSVREL > 3 */
#ifdef aiws
/*  7 */	"DANGER", 	"System Crash Imminent",
#else /* aiws */
# ifdef linux
/*  7 */	0,		"Signal 7",
# else /* linux */
/*  7 */	"EMT",		"EMT trap",
# endif /* linux */
#endif /* aiws */
/*  8 */	"FPE",		"Floating exception",
/*  9 */	"KILL",		"Killed",
#ifdef linux
/* 10 */	"USR1",		"User signal 1",
/* 11 */	"SEGV",		"Segmentation fault",
/* 12 */	"USR2",		"User signal 2",
#else /* linux */
/* 10 */	"BUS",		"Bus error",
/* 11 */	"SEGV",		"Segmentation fault",
/* 12 */	"SYS",		"Bad system call",
#endif /* linux */
/* 13 */	"PIPE",		"Broken pipe",
/* 14 */	"ALRM",		"Alarm clock",
/* 15 */	"TERM",		"Terminated",


#if (SYSVREL > 0) || defined(DGUX) || defined(IBMAIX) || defined(apollo) || defined(masscomp) || defined(ardent)

# ifdef _sigextra_
#  undef  _sigextra_
# endif /* _sigextra_ */

#if !defined(IBMAIX) && !defined(cray)
/* these are the real svid signals */
/* 16 */	"USR1",		"User signal 1",
/* 17 */	"USR2", 	"User signal 2",
# ifdef apollo
/* 18 */	"CLD",		"Death of child",
/* 19 */	"APOLLO",  	"Apollo-specific fault",
# else
/* 18 */	"CHLD",		"Child exited",
/* 19 */	"PWR",  	"Power failure",
# endif /* apollo */
#endif /* !IBMAIX && !cray */

# ifdef cray
# define _sigextra_
/* 16 */	"IO",		"Input/output possible signal",
/* 17 */	"URG",		"Urgent condition on I/O channel",
/* 18 */	"CHLD",		"Child exited",
/* 19 */	"PWR",		"Power failure",
/* 20 */	"MT",		"Multitasking wake-up",
/* 21 */	"MTKILL",	"Multitasking kill",
/* 22 */	"BUFIO",	"Fortran asynchronous I/O completion",
/* 23 */	"RECOVERY",	"Recovery",
/* 24 */	"UME",		"Uncorrectable memory error",
/* 25 */	"DLK",		"True deadlock detected",
/* 26 */	"CPULIM",	"CPU time limit exceeded",
/* 27 */	"SHUTDN",	"System shutdown imminent",
/* 28 */	"STOP", 	MSG_STOP,
/* 29 */	"TSTP", 	MSG_TSTP,
/* 30 */	"CONT",   	"Continue",
/* 31 */	"TTIN",		MSG_TTIN,
/* 32 */	"TTOU",		MSG_TTOU,
/* 33 */	"WINCH",	"Window size changed",
/* 34 */	"RPE",		"CRAY Y-MP register parity error",
/* 35 */	0,		"Signal 35",
/* 36 */	0,		"Signal 36",
/* 37 */	0,		"Signal 37",
/* 38 */	0,		"Signal 38",
/* 39 */	0,		"Signal 39",
/* 40 */	0,		"Signal 40",
/* 41 */	0,		"Signal 41",
/* 42 */	0,		"Signal 42",
/* 43 */	0,		"Signal 43",
/* 44 */	0,		"Signal 44",
/* 45 */	0,		"Signal 45",
/* 46 */	0,		"Signal 46",
/* 47 */	0,		"Signal 47",
/* 48 */	"INFO",		"Information signal",
/* 49 */	"USR1",		"User-defined signal 1",
/* 50 */	"USR2",		"User-defined signal 2",
/* 51 */	0,		"Signal 51",
/* 52 */	0,		"Signal 52",
/* 53 */	0,		"Signal 53",
/* 54 */	0,		"Signal 54",
/* 55 */	0,		"Signal 55",
/* 56 */	0,		"Signal 56",
/* 57 */	0,		"Signal 57",
/* 58 */	0,		"Signal 58",
/* 59 */	0,		"Signal 59",
/* 60 */	0,		"Signal 60",
/* 61 */	0,		"Signal 61",
/* 62 */	0,		"Signal 62",
/* 63 */	0,	    	"Signal 63",
/* 64 */	0,		"Signal 64",
# endif /* cray */

/*
**  In the UNIXpc these signal *ARE* used!!
*/
#ifdef UNIXPC
/* 20 */	"WIND",		"Window status changed",
/* 21 */	"PHONE", 	"Phone status changed",
#endif

# ifdef OREO
#  define _sigextra_
/* 20 */	"TSTP",		MSG_TSTP,
/* 21 */	"TTIN", 	MSG_TTIN,
/* 22 */	"TTOU", 	MSG_TTOU,
/* 23 */	"STOP",		MSG_STOP,
/* 24 */	"XCPU",		"Cputime limit exceeded",
/* 25 */	"XFSZ", 	"Filesize limit exceeded",
/* 26 */	"VTALRM", 	"Virtual time alarm",
/* 27 */	"PROF", 	"Profiling time alarm",
/* 28 */	"WINCH", 	"Window changed",
/* 29 */	"CONT",		"Continued",
/* 30 */	"URG",		"Urgent condition on IO channel",
/* 31 */	"IO",		"Asynchronous I/O (select)",
/* 32 */	0,		"Signal 32",
# endif /* OREO */

# ifdef hpux
#  define _sigextra_
/* 20 */	"VTALRM", 	"Virtual time alarm",
/* 21 */	"PROF", 	"Profiling time alarm",
/* 22 */	"IO", 		"Asynchronous I/O (select)",
/* 23 */	"WINDOW", 	"Window changed",
/* 24 */	"STOP",		MSG_STOP,
/* 25 */	"TSTP",		MSG_TSTP,
/* 26 */	"CONT",		"Continued",
/* 27 */	"TTIN", 	MSG_TTIN,
/* 28 */	"TTOU", 	MSG_TTOU,
/* 29 */	"URG",		"Urgent condition on IO channel",
/* 30 */	"LOST",		"Remote lock lost (NFS)",
/* 31 */	0, 		"Reserved", /* Reserved */
/* 32 */	"DIL",		"DIL signal",
# endif /* hpux */

# ifdef stellar
#  define _sigextra_
/* 20 */	"WINDOW", 	"Window changed",
/* 21 */	"URG",		"Urgent condition on IO channel",
/* 22 */	"POLL", 	"Pollable event occured",
/* 23 */	"STOP",		MSG_STOP,
/* 24 */	"TSTP",		MSG_TSTP,
/* 25 */	"CONT",		"Continued",
/* 26 */	"TTIN", 	MSG_TTIN,
/* 27 */	"TTOU", 	MSG_TTOU,
/* 28 */	"IO", 		"Asynchronous I/O (select)",
/* 29 */	"XCPU",		"Cputime limit exceeded",
/* 30 */	"XFSZ", 	"Filesize limit exceeded",
/* 31 */	"VTALRM", 	"Virtual time alarm",
/* 32 */	"PROF", 	"Profiling time alarm",
# endif /* stellar */

# ifdef ardent
#  define _sigextra_
/* 20 */	"WINDOW", 	"Window changed",
/* 21 */	"URG",		"Urgent condition on IO channel",
/* 22 */	"POLL", 	"Pollable event occured",
/* 23 */	"STOP",		MSG_STOP,
/* 24 */	"TSTP",		MSG_TSTP,
/* 25 */	"TTIN", 	MSG_TTIN,
/* 26 */	"TTOU", 	MSG_TTOU,
/* 27 */	"CONT",		"Continued",
/* 28 */	"XCPU",		"Cputime limit exceeded",
/* 29 */	"XFSZ", 	"Filesize limit exceeded",
/* 30 */	"VTALRM", 	"Virtual time alarm",
/* 31 */	"PROF", 	"Profiling time alarm",
# endif /* ardent */

# if SYSVREL > 3
#  define _sigextra_
/* 20 */	"WINCH", 	"Window change",
/* 21 */	"URG", 		"Urgent socket condition",
/* 22 */	"IO", 		"Socket I/O possible",
/* 23 */	"STOP",		MSG_STOP,
/* 24 */	"TSTP",		MSG_TSTP,
/* 25 */	"CONT",		"Continued",
/* 26 */	"TTIN", 	MSG_TTIN,
/* 27 */	"TTOU", 	MSG_TTOU,
/* 28 */	"VTALRM",	"Virtual timer expired",
/* 29 */	"PROF",		"Profiling timer expired",
/* 30 */	"XCPU",		"CPU time limit exceeded",
/* 31 */	"XFSZ", 	"File size limit exceeded",
/* 32 */	0,		"Maximum number of signals",
# endif /* SYSVREL > 3 */
# if defined(ISC) && defined(POSIX) 
#  define _sigextra_
/* 20 */	"WINCH", 	"Window change",
/* 21 */	0, 		"Unused", /* SIGPHONE used only for UNIXPC */
/* 22 */	"POLL", 	"Pollable event occured",
/* 23 */	"CONT", 	"Continued",
/* 24 */	"STOP",		MSG_STOP,
/* 25 */	"TSTP",		MSG_TSTP,
/* 26 */	"TTIN", 	MSG_TTIN,
/* 27 */	"TTOU", 	MSG_TTOU,
/* 28 */	0,	  	"number of signals",
/* 29 */	0,		"Reserved", /* Reserved */
/* 30 */	0,		"Reserved", /* Reserved */
/* 31 */	0, 		"Reserved", /* Reserved */
/* 32 */	0,		"Maximum number of signals",
# endif /* ISC && POSIX */

# if defined(SCO) && defined(POSIX) 
#  define _sigextra_
/* 20 */	"WINCH", 	"Window change",
/* 21 */	0, 		"Unused", /* SIGPHONE used only for UNIXPC */
/* 22 */	"POLL", 	"Pollable event occured",
/* 23 */	"STOP",		MSG_STOP,
/* 24 */	"TSTP",		MSG_TSTP,
/* 25 */	"CONT", 	"Continued",
/* 26 */	"TTIN", 	MSG_TTIN,
/* 27 */	"TTOU", 	MSG_TTOU,
/* 28 */	0,	  	"number of signals",
/* 29 */	0,		"Reserved", /* Reserved */
/* 30 */	0,		"Reserved", /* Reserved */
/* 31 */	0, 		"Reserved", /* Reserved */
/* 32 */	0,		"Maximum number of signals",
# endif /* SCO && POSIX */

# ifdef IRIS4D
#  define _sigextra_
/* 20 */	"STOP",		MSG_STOP,
/* 21 */	"TSTP",		MSG_TSTP,
/* 22 */	"POLL", 	"Stream I/O pending",
/* 23 */	"IO", 		"Asynchronous I/O (select)",
/* 24 */	"URG",		"Urgent condition on IO channel",
/* 25 */	"WINCH", 	"Window changed",
/* 26 */	"VTALRM", 	"Virtual time alarm",
/* 27 */	"PROF", 	"Profiling time alarm",
/* 28 */	"CONT",		"Continued",
/* 29 */	"TTIN", 	MSG_TTIN,
/* 30 */	"TTOU", 	MSG_TTOU,
/* 31 */	0,		"Signal 31",
/* 32 */	0,		"Signal 32",
# endif /* IRIS4D */

# ifdef IRIS3D
#  define _sigextra_
/* 20 */	0,		"Signal 20",
/* 21 */	0,		"Signal 21",
/* 22 */	0,		"Signal 22",
/* 23 */	0,		"Signal 23",
/* 24 */	0,		"Signal 24",
/* 25 */	"WINCH", 	"Window changed",
/* 26 */	"IO", 		"Asynchronous I/O (select)",
/* 27 */	"URG",		"Urgent condition on IO channel",
/* 28 */	"POLL", 	"Stream I/O pending",
/* 29 */	0,		"Signal 29",
/* 30 */	0,		"Signal 30",
/* 31 */	0,		"Signal 31",
/* 32 */	0,		"Signal 32",
# endif /* IRIS3D */

# ifdef apollo
#  define _sigextra_
/* 20 */	"STOP",		MSG_STOP,
/* 21 */	"TSTP",		MSG_TSTP,
/* 22 */	"CONT",		"Continued",
/* 23 */	"CHLD",		"Child stopped or exited",
/* 24 */	"TTIN", 	MSG_TTIN,
/* 25 */	"TTOU", 	MSG_TTOU,
/* 26 */	"IO", 		"Asynchronous I/O (select)",
/* 27 */	"XCPU",		"Cputime limit exceeded",
/* 28 */	"XFSZ", 	"Filesize limit exceeded",
/* 29 */	"VTALRM", 	"Virtual time alarm",
/* 30 */	"PROF", 	"Profiling time alarm",
/* 31 */	"URG",		"Urgent condition on IO channel",
/* 32 */	"WINCH", 	"Window changed",
# endif /* apollo */

# ifdef masscomp
#  define _sigextra_
/* 20 */	"STOP",		MSG_STOP,
/* 21 */	"TSTP",		MSG_TSTP,
/* 22 */	"CONT",		"Continued",
/* 23 */	"TTIN", 	MSG_TTIN,
/* 24 */	"TTOU", 	MSG_TTOU,
/* 25 */	"TINT", 	"New input character",
/* 26 */	"XCPU",		"Cputime limit exceeded",
/* 27 */	"XFSZ", 	"Filesize limit exceeded",
/* 28 */	"WINCH", 	"Window changed",
/* 29 */	"URG",		"Urgent condition on IO channel",
/* 30 */	"VTALRM", 	"Virtual time alarm",
/* 31 */	"PROF", 	"Profiling time alarm",
/* 32 */	"IO", 		"Asynchronous I/O (select)",
# endif /* masscomp */

# ifdef aiws
#  define _sigextra_
/* 20 */	0,		"Signal 20",
/* 21 */	0,		"Signal 21",
/* 22 */	0,		"Signal 22",
/* 23 */	"AIO", 		"LAN Asyncronous I/O",
/* 24 */	"PTY", 		"PTY read/write availability",
/* 25 */	"IOINT", 	"I/O intervention required",
/* 26 */	"GRANT", 	"monitor mode granted",
/* 27 */	"RETRACT", 	"monitor mode retracted",
/* 28 */	"WINCH",	"Window size changed",
/* 29 */	0,		"Signal 29",
/* 30 */	"SOUND", 	"sound completed",
/* 31 */	"MSG", 		"input hft data pending",
/* 32 */	0,		"Signal 32",
# endif /* aiws */

# if defined(m88k) || defined(__m88k__)	/* Motorola 88100: POSIX/BCS signals */
#  define _sigextra_
/* 20 */	"WINCH", 	"Window changed",
/* 21 */	0,		"Signal 21",
/* 22 */	"POLL", 	"Stream I/O pending",
/* 23 */	"STOP",		MSG_STOP,
/* 24 */	"TSTP",		MSG_TSTP,
/* 25 */	"CONT",		"Continued",
/* 26 */	"TTIN", 	MSG_TTIN,
/* 27 */	"TTOU", 	MSG_TTOU,
/* 28 */	0,		"Signal 28",
/* 29 */	0,		"Signal 29",
/* 30 */	0,		"Signal 30",
/* 31 */	0,		"Signal 31",
/* 32 */	0,		"Signal 32",
/* 33 */	"URG",		"Urgent condition on IO channel",
/* 34 */	"IO", 		"Asynchronous I/O (select)",
/* 35 */	"XCPU",		"Cputime limit exceeded",
/* 36 */	"XFSZ", 	"Filesize limit exceeded",
/* 37 */	"VTALRM", 	"Virtual time alarm",
/* 38 */	"PROF",		"Profiling time alarm",
/* 39 */	0,		"Signal 39",
/* 40 */	"LOST",		"Resource lost",
/* 41 */	0,		"Signal 41",
/* 42 */	0,		"Signal 42",
/* 43 */	0,		"Signal 43",
/* 44 */	0,		"Signal 44",
/* 45 */	0,		"Signal 45",
/* 46 */	0,		"Signal 46",
/* 47 */	0,		"Signal 47",
/* 48 */	0,		"Signal 48",
/* 49 */	0,		"Signal 49",
/* 50 */	0,		"Signal 50",
/* 51 */	0,		"Signal 51",
/* 52 */	0,		"Signal 52",
/* 53 */	0,		"Signal 53",
/* 54 */	0,		"Signal 54",
/* 55 */	0,		"Signal 55",
/* 56 */	0,		"Signal 56",
/* 57 */	0,		"Signal 57",
/* 58 */	0,		"Signal 58",
/* 59 */	0,		"Signal 59",
/* 60 */	0,		"Signal 60",
/* 61 */	0,		"Signal 61",
/* 62 */	0,		"Signal 62",
/* 63 */	0,		"Signal 63",
/* 64 */	0,		"Signal 64",
# endif /* m88k || __m88k__ */

#ifdef IBMAIX
# define _sigextra_
/* 16 */	"URG",		"Urgent condition on IO channel",
/* 17 */	"STOP",		MSG_STOP,
/* 18 */	"TSTP",		MSG_TSTP,
/* 19 */	"CONT",		"Continued",
/* 20 */	"CHLD",		"Child exited",
/* 21 */	"TTIN", 	MSG_TTIN,
/* 22 */	"TTOU", 	MSG_TTOU,
/* 23 */	"IO",   	"IO possible interrupt",
/* 24 */	"XCPU",		"Cputime limit exceeded",
/* 25 */	"XFSZ", 	"Filesize limit exceeded",
/* 26 */	0,		"Signal 26",
/* 27 */	"MSG", 		"Data in HFT ring buffer",
/* 28 */	"WINCH",	"Window size changed",
/* 29 */	"PWR",		"Power failure",
/* 30 */	"USR1",		"User signal 1",
/* 31 */	"USR2", 	"User signal 2",
/* 32 */	"PROF",		"Profiling time alarm",
/* 33 */	"DANGER", 	"System Crash Imminent",
/* 34 */	"VTALRM", 	"Virtual time alarm",
/* 35 */	"MIGRATE",	"Migrate process",
/* 36 */	"PRE",	  	"Programming exception",
/* 37 */	0,		"Signal 37",
/* 38 */	0,		"Signal 38",
/* 39 */	0,		"Signal 39",
/* 40 */	0,		"Signal 40",
/* 41 */	0,		"Signal 41",
/* 42 */	0,		"Signal 42",
/* 43 */	0,		"Signal 43",
/* 44 */	0,		"Signal 44",
/* 45 */	0,		"Signal 45",
/* 46 */	0,		"Signal 46",
/* 47 */	0,		"Signal 47",
/* 48 */	0,		"Signal 48",
/* 49 */	0,		"Signal 49",
/* 50 */	0,		"Signal 50",
/* 51 */	0,		"Signal 51",
/* 52 */	0,		"Signal 52",
/* 53 */	0,		"Signal 53",
/* 54 */	0,		"Signal 54",
/* 55 */	0,		"Signal 55",
/* 56 */	0,		"Signal 56",
/* 57 */	0,		"Signal 57",
/* 58 */	0,		"Signal 58",
/* 59 */	0,		"Signal 59",
/* 60 */	"GRANT", 	"HFT monitor mode granted",
/* 61 */	"RETRACT", 	"HFT monitor mode should be relinguished",
/* 62 */	"SOUND",	"HFT sound control has completed",
#ifdef SIGSAK
/* 63 */	"SAK",    	"Secure attention key",
#else
/* 63 */	0,	    	"Signal 63",
#endif
/* 64 */	0,		"Signal 64",
#endif /* IBMAIX */


# ifdef _SEQUENT_
#  define _sigextra_
/* 20 */	"WINCH", 	"Window changed",
/* 21 */	0,		"Signal 21",
/* 22 */	"POLL", 	"Stream I/O pending",
/* 23 */	"STOP",		MSG_STOP,
/* 24 */	"TSTP",		MSG_TSTP,
/* 25 */	"CONT",		"Continued",
/* 26 */	"TTIN", 	MSG_TTIN,
/* 27 */	"TTOU", 	MSG_TTOU,
/* 28 */	0, 		"Signal 28",
/* 29 */	0,		"Signal 29",
/* 30 */	0, 		"Signal 30",
/* 31 */	0, 		"Signal 31",
/* 32 */	0,		"Signal 32",
# endif /* _SEQUENT_ */

# ifndef _sigextra_
/* 20 */	0,		"Signal 20",
/* 21 */	0,		"Signal 21",
/* 22 */	0,		"Signal 22",
/* 23 */	0,		"Signal 23",
/* 24 */	0,		"Signal 24",
/* 25 */	0,		"Signal 25",
/* 26 */	0,		"Signal 26",
/* 27 */	0,		"Signal 27",
/* 28 */	0,		"Signal 28",
/* 29 */	0,		"Signal 29",
/* 30 */	0,		"Signal 30",
/* 31 */	0,		"Signal 31",
/* 32 */	0,		"Signal 32",
# endif /* _sigextra_ */


#else /* bsd */

# ifdef _sigextra_
#  undef  _sigextra_
# endif /* _sigextra_ */

#ifdef linux
# define _sigextra_
/* 16 */	0,		"Signal 16",
/* 17 */	"CHLD",		"Child exited",
/* 18 */	"CONT",		"Continued",
/* 19 */	"STOP",		MSG_STOP,
/* 20 */	"TSTP",		MSG_TSTP,
/* 21 */	"TTIN", 	MSG_TTIN,
/* 22 */	"TTOU", 	MSG_TTOU,
/* 23 */	0,   		"Signal 23",
/* 24 */	0,		"Signal 24",
/* 25 */	0, 		"Signal 25",
/* 26 */	0,	 	"Signal 26",
/* 27 */	0,		"Signal 27",
/* 28 */	"WINCH", 	"Window changed",
/* 29 */	0,		"Signal 29",
/* 30 */	0,		"Signal 30",
/* 31 */	0,		"Signal 31",
/* 32 */	0,		"Signal 32",
#else /* linux */
/* 16 */	"URG",		"Urgent condition on IO channel",
/* 17 */	"STOP",		MSG_STOP,
/* 18 */	"TSTP",		MSG_TSTP,
/* 19 */	"CONT",		"Continued",
/* 20 */	"CHLD",		"Child exited",
/* 21 */	"TTIN", 	MSG_TTIN,
/* 22 */	"TTOU", 	MSG_TTOU,
/* 23 */	"IO",   	"IO possible interrupt",
/* 24 */	"XCPU",		"Cputime limit exceeded",
/* 25 */	"XFSZ", 	"Filesize limit exceeded",
/* 26 */	"VTALRM", 	"Virtual time alarm",
/* 27 */	"PROF",		"Profiling time alarm",
#endif /* linux */

# if defined(RENO) || defined(BSD4_4)
# define _sigextra_
/* 28 */	"WINCH",	"Window size changed",
/* 29 */	"INFO",		"Information request",
/* 30 */	"USR1",		"User defined signal 1",
/* 31 */	"USR2",		"User defined signal 2",
/* 32 */	0,		"Signal 32",
# endif /* RENO || BSD4_4 */

# if (defined(sun) || defined(__sun__) || defined(ultrix) || defined(hp9000) || defined(convex) || defined(__convex__)) && !defined(_sigextra_)
#  define _sigextra_
/* 28 */	"WINCH", 	"Window changed",
/* 29 */	"LOST",		"Resource lost",
/* 30 */	"USR1",		"User signal 1",
/* 31 */	"USR2",		"User signal 2",
/* 32 */	0,		"Signal 32",
# endif /* sun */

# ifdef pyr
#  define _sigextra_
/* 28 */	"USR1",		"User signal 1",
/* 29 */	"USR2",		"User signal 2",
/* 30 */	"PWR",		"Power failure",
/* 31 */	0,		"Signal 31",
/* 32 */	0,		"Signal 32",
# endif /* pyr */

# ifndef _sigextra_
/* 28 */	"WINCH",	"Window size changed",
/* 29 */	0,		"Signal 29",
/* 30 */	"USR1",		"User defined signal 1",
/* 31 */	"USR2",		"User defined signal 2",
/* 32 */	0,		"Signal 32",
# endif /* _sigextra_ */

#endif /* (SYSVREL > 0) || DGUX || IBMAIX */

/* These are here for systems with bad NSIG */
#ifndef POSIX
/* 33 */	0,		"Signal 33"
#else /* POSIX */
/* 65 */	0,		"Signal 65"
#endif /* POSIX */
};

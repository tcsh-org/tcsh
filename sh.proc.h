/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.proc.h,v 2.0 1991/03/26 02:59:29 christos Exp $ */
/*
 * sh.proc.h: Process data structures and variables
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
#ifndef _h_sh_proc
#define _h_sh_proc
/*
 *	@(#)sh.proc.h	5.2 (Berkeley) 6/6/85
 */

/*
 * C shell - process structure declarations
 */

/*
 * Structure for each process the shell knows about:
 *	allocated and filled by pcreate.
 *	flushed by pflush; freeing always happens at top level
 *	    so the interrupt level has less to worry about.
 *	processes are related to "friends" when in a pipeline;
 *	    p_friends links makes a circular list of such jobs
 */
struct process	{
	struct	process *p_next;	/* next in global "proclist" */
	struct	process	*p_friends;	/* next in job list (or self) */
	struct	directory *p_cwd;	/* cwd of the job (only in head) */
	short	unsigned p_flags;	/* various job status flags */
	char	p_reason;		/* reason for entering this state */
	char	p_index;		/* shorthand job index */
	int	p_pid;
	int	p_jobid;		/* pid of job leader */
	/* if a job is stopped/background p_jobid gives its pgrp */
#ifdef BSDTIMES
	struct	timeval p_btime;	/* begin time */
	struct	timeval p_etime;	/* end time */
	struct	rusage p_rusage;
#else /* BSDTIMES */
#ifdef _SEQUENT_
	timeval_t p_btime;              /* begin time */
	timeval_t p_etime;              /* end time */
	struct  process_stats p_rusage;
#else /* _SEQUENT_ */
#  ifndef POSIX
	time_t	p_btime;	/* begin time */
	time_t	p_etime;	/* end time */
	time_t	p_utime;	/* user time */
	time_t	p_stime;	/* system time */
#  else /* POSIX */
	clock_t	p_btime;	/* begin time */
	clock_t	p_etime;	/* end time */
	clock_t	p_utime;	/* user time */
	clock_t	p_stime;	/* system time */
#  endif /* POSIX */
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */
	Char	*p_command;		/* first PMAXLEN chars of command */
};

/* flag values for p_flags */
#define	PRUNNING	(1<<0)		/* running */
#define	PSTOPPED	(1<<1)		/* stopped */
#define	PNEXITED	(1<<2)		/* normally exited */
#define	PAEXITED	(1<<3)		/* abnormally exited */
#define	PSIGNALED	(1<<4)		/* terminated by a signal != SIGINT */

#define	PALLSTATES	(PRUNNING|PSTOPPED|PNEXITED|PAEXITED|PSIGNALED|PINTERRUPTED)
#define	PNOTIFY		(1<<5)		/* notify async when done */
#define	PTIME		(1<<6)		/* job times should be printed */
#define	PAWAITED	(1<<7)		/* top level is waiting for it */
#define	PFOREGND	(1<<8)		/* started in shells pgrp */
#define	PDUMPED		(1<<9)		/* process dumped core */
#define	PDIAG		(1<<10)		/* diagnostic output also piped out */
#define	PPOU		(1<<11)		/* piped output */
#define	PREPORTED	(1<<12)		/* status has been reported */
#define	PINTERRUPTED	(1<<13)		/* job stopped via interrupt signal */
#define	PPTIME		(1<<14)		/* time individual process */
#define	PNEEDNOTE	(1<<15)		/* notify as soon as practical */

#define	PNULL		(struct process *)0
#define	PMAXLEN		80

/* defines for arguments to pprint */
#define	NUMBER		01
#define	NAME		02
#define	REASON		04
#define	AMPERSAND	010
#define	FANCY		020
#define	SHELLDIR	040		/* print shell's dir if not the same */
#define	JOBDIR		0100		/* print job's dir if not the same */
#define	AREASON		0200

struct	process	proclist;		/* list head of all processes */
bool	pnoprocesses;			/* pchild found nothing to wait for */

struct	process *pholdjob;		/* one level stack of current jobs */

struct	process *pcurrjob;		/* current job */
struct	process	*pcurrent;		/* current job in table */
struct	process *pprevious;		/* previous job in table */

short	pmaxindex;			/* current maximum job index */

#ifndef BSDTIMES
bool	timesdone;			/* shtimes buffer full ? */
#endif /* BSDTIMES */

#endif /* _h_sh_proc */

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.time.c,v 2.0 1991/03/26 02:59:29 christos Exp $ */
/*
 * sh.time.c: Shell time keeping and printing.
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
static char *sccsid = "@(#)sh.time.c	5.4 (Berkeley) 5/13/86";
#endif
#ifndef lint
static char *rcsid = "$Id: sh.time.c,v 2.0 1991/03/26 02:59:29 christos Exp $";
#endif

#include "sh.h"
#if defined(sun) && ! defined(MACH)
#include <machine/param.h>
#endif /* sun */

/*
 * C Shell - routines handling process timing and niceing
 */
#ifdef BSDTIMES
#ifndef RUSAGE_SELF
#define	RUSAGE_SELF	0
#define	RUSAGE_CHILDREN	-1
#endif /* RUSAGE_SELF */
#else /* BSDTIMES */
struct	tms times0;
#endif /* BSDTIMES */

#if !defined(BSDTIMES) && !defined(SEQUENT)
static void pdtimet();
#else
static void pdeltat();
#endif

void
settimes()
{
#ifdef BSDTIMES
	struct rusage ruch;

	(void) gettimeofday(&time0, (struct timezone *)0);
	(void) getrusage(RUSAGE_SELF, &ru0);
	(void) getrusage(RUSAGE_CHILDREN, &ruch);
	ruadd(&ru0, &ruch);
#else
# ifdef _SEQUENT_
	struct process_stats ruch;

	(void) get_process_stats(&time0, PS_SELF, &ru0, &ruch);
	ruadd (&ru0, &ruch);
# else /* _SEQUENT_ */
	time0 = times(&times0);
	times0.tms_stime += times0.tms_cstime;
	times0.tms_utime += times0.tms_cutime;
	times0.tms_cstime = 0;
	times0.tms_cutime = 0;
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */
}

/*
 * dotime is only called if it is truly a builtin function and not a
 * prefix to another command
 */
void
dotime()
{
#ifdef BSDTIMES
	timeval_t timedol;
	struct rusage ru1, ruch;

	(void) getrusage(RUSAGE_SELF, &ru1);
	(void) getrusage(RUSAGE_CHILDREN, &ruch);
	ruadd(&ru1, &ruch);
	(void) gettimeofday(&timedol, (struct timezone *)0);
	prusage(&ru0, &ru1, &timedol, &time0);
# else
# ifdef _SEQUENT_
	timeval_t timedol;
	struct process_stats ru1, ruch;

	(void) get_process_stats(&timedol, PS_SELF, &ru1, &ruch);
	ruadd(&ru1, &ruch);
	prusage(&ru0, &ru1, &timedol, &time0);
# else /* _SEQUENT_ */
#  ifndef POSIX
	time_t timedol;
#  else /* POSIX */
	clock_t timedol;
#  endif /* POSIX */
	struct tms times_dol;

	timedol = times(&times_dol);
	times_dol.tms_stime += times_dol.tms_cstime;
	times_dol.tms_utime += times_dol.tms_cutime;
	times_dol.tms_cstime = 0;
	times_dol.tms_cutime = 0;
	prusage(&times0, &times_dol, timedol, time0);
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */
}

/*
 * donice is only called when it on the line by itself or with a +- value
 */
void
donice(v)
	register Char **v;
{
	register Char *cp;
	int nval = 0;

	v++, cp = *v++;
	if (cp == 0)
		nval = 4;
	else if (*v == 0 && any("+-", cp[0]))
		nval = getn(cp);
#ifdef BSDNICE
	(void) setpriority(PRIO_PROCESS, 0, nval);
#else /* BSDNICE */
	(void) nice(nval);
#endif /* BSDNICE */
}

#ifdef BSDTIMES
void
ruadd(ru, ru2)
	register struct rusage *ru, *ru2;
{
	tvadd(&ru->ru_utime, &ru2->ru_utime);
	tvadd(&ru->ru_stime, &ru2->ru_stime);
	if (ru2->ru_maxrss > ru->ru_maxrss)
		ru->ru_maxrss = ru2->ru_maxrss;

	ru->ru_ixrss 	+= ru2->ru_ixrss;
	ru->ru_idrss 	+= ru2->ru_idrss;
	ru->ru_isrss 	+= ru2->ru_isrss;
	ru->ru_minflt 	+= ru2->ru_minflt;
	ru->ru_majflt 	+= ru2->ru_majflt;
	ru->ru_nswap 	+= ru2->ru_nswap;
	ru->ru_inblock 	+= ru2->ru_inblock;
	ru->ru_oublock 	+= ru2->ru_oublock;
	ru->ru_msgsnd 	+= ru2->ru_msgsnd;
	ru->ru_msgrcv 	+= ru2->ru_msgrcv;
	ru->ru_nsignals += ru2->ru_nsignals;
	ru->ru_nvcsw 	+= ru2->ru_nvcsw;
	ru->ru_nivcsw 	+= ru2->ru_nivcsw;
}
#else /* BSDTIMES */
# ifdef _SEQUENT_
void
ruadd(ru, ru2)
	register struct process_stats *ru, *ru2;
{
	tvadd(&ru->ps_utime, &ru2->ps_utime);
	tvadd(&ru->ps_stime, &ru2->ps_stime);
	if (ru2->ps_maxrss > ru->ps_maxrss)
	    ru->ps_maxrss = ru2->ps_maxrss;

	ru->ps_pagein	+= ru2->ps_pagein;
	ru->ps_reclaim	+= ru2->ps_reclaim;
	ru->ps_zerofill	+= ru2->ps_zerofill;
	ru->ps_pffincr	+= ru2->ps_pffincr;
	ru->ps_pffdecr	+= ru2->ps_pffdecr;
	ru->ps_swap	+= ru2->ps_swap;
	ru->ps_syscall	+= ru2->ps_syscall;
	ru->ps_volcsw	+= ru2->ps_volcsw;
	ru->ps_involcsw	+= ru2->ps_involcsw;
	ru->ps_signal	+= ru2->ps_signal;
	ru->ps_lread	+= ru2->ps_lread;
	ru->ps_lwrite	+= ru2->ps_lwrite;
	ru->ps_bread	+= ru2->ps_bread;
	ru->ps_bwrite	+= ru2->ps_bwrite;
	ru->ps_phread	+= ru2->ps_phread;
	ru->ps_phwrite	+= ru2->ps_phwrite;
}
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */

#ifdef BSDTIMES

/* 
 * PWP: the LOG1024 and pagetok stuff taken from the top command,
 * written by William LeFebvre
 */
/* Log base 2 of 1024 is 10 (2^10 == 1024) */
#define LOG1024         10

/* Convert clicks (kernel pages) to kbytes ... */
/* If there is no PGSHIFT defined, assume it is 11 */
/* Is this needed for compatability with some old flavor of 4.2 or 4.1? */
#ifndef PGSHIFT
#define pagetok(size)   ((size) << 1)
#else
#if PGSHIFT>10
#define pagetok(size)   ((size) << (PGSHIFT - LOG1024))
#else
#define pagetok(size)   ((size) >> (LOG1024 - PGSHIFT))
#endif
#endif

/*
 * if any other machines return wierd values in the ru_i* stuff, put
 * the adjusting macro here:
 */
# ifdef sun
#define IADJUST(i)	(pagetok(i)/2)
# else /* sun */
#define IADJUST(i)	(i)
# endif /* sun */

void
prusage(r0, r1, e, b)
	register struct rusage *r0, *r1;
	timeval_t *e, *b;
#else /* BSDTIMES */
# ifdef _SEQUENT_
void
prusage (r0, r1, e, b)
	register struct process_stats *r0, *r1;
	timeval_t *e, *b;
# else /* _SEQUENT_ */
void
prusage(bs, es, e, b)
	struct tms *bs, *es;
#  ifndef POSIX
	time_t e, b;
#  else /* POSIX */
	clock_t e, b;
#  endif /* POSIX */
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */
{
#ifdef BSDTIMES
	register time_t t =
	    (r1->ru_utime.tv_sec-r0->ru_utime.tv_sec)*100+
	    (r1->ru_utime.tv_usec-r0->ru_utime.tv_usec)/10000+
	    (r1->ru_stime.tv_sec-r0->ru_stime.tv_sec)*100+
	    (r1->ru_stime.tv_usec-r0->ru_stime.tv_usec)/10000;
#else
#ifdef _SEQUENT_
	register time_t t =
	    (r1->ps_utime.tv_sec-r0->ps_utime.tv_sec)*100+
	    (r1->ps_utime.tv_usec-r0->ps_utime.tv_usec)/10000+
	    (r1->ps_stime.tv_sec-r0->ps_stime.tv_sec)*100+
	    (r1->ps_stime.tv_usec-r0->ps_stime.tv_usec)/10000;
#else /* _SEQUENT_ */
#  ifndef POSIX
	register time_t t = (es->tms_utime - bs->tms_utime + 
			     es->tms_stime - bs->tms_stime) * 100 / HZ; 
#  else /* POSIX */
	register clock_t t = (es->tms_utime - bs->tms_utime + 
			      es->tms_stime - bs->tms_stime) * 100 / CLK_TCK;
#  endif /* POSIX */
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */

	register char *cp;
	register long i;
	register struct varent *vp = adrof(STRtime);

#ifdef BSDTIMES
	int ms =
	    (e->tv_sec-b->tv_sec)*100 + (e->tv_usec-b->tv_usec)/10000;
	cp = "%Uu %Ss %E %P %X+%Dk %I+%Oio %Fpf+%Ww";
#else
# ifdef _SEQUENT_
	int ms =
	     (e->tv_sec-b->tv_sec)*100 + (e->tv_usec-b->tv_usec)/10000;
	cp = "%Uu %Ss %E %P %I+%Oio %Fpf+%Ww";
# else /* _SEQUENT_ */
#  ifndef POSIX
	time_t ms = (e - b) * 100 / HZ;
#  else /* POSIX */
	clock_t ms = (e - b) * 100 / CLK_TCK;
#  endif /* POSIX */
	cp = "%Uu %Ss %E %P";
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */
#if ! ( defined(sequent) || defined(_SEQUENT_) )
	if ( ms < t ) ms = t;	/* granularity fix: can't be more than 100% */
#endif /* ! ( defined(sequent) || defined(_SEQUENT_) ) */
#ifdef TDEBUG
	CSHprintf( "es->tms_utime %lu bs->tms_utime %lu\n",
		es->tms_utime, bs->tms_utime);
	CSHprintf( "es->tms_stime %lu bs->tms_stime %lu\n",
		es->tms_stime, bs->tms_stime);
	CSHprintf( "ms %lu e %lu b %lu\n", ms, e, b);
	CSHprintf( "t %lu\n", t);
#endif /* TDEBUG */

	if (vp && vp->vec[0] && vp->vec[1])
		cp = short2str(vp->vec[1]);
	for (; *cp; cp++)
	if (*cp != '%')
		CSHputchar(*cp);
	else if (cp[1]) switch(*++cp) {

	case 'U':		/* user CPU time used */
#ifdef BSDTIMES
		pdeltat(&r1->ru_utime, &r0->ru_utime);
#else
# ifdef _SEQUENT_
		pdeltat(&r1->ps_utime, &r0->ps_utime);
# else /* _SEQUENT_ */
#  ifndef POSIX
		pdtimet(es->tms_utime, bs->tms_utime);
#  else /* POSIX */
		pdtimet(es->tms_utime, bs->tms_utime);
#  endif /* POSIX */
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */
		break;

	case 'S':		/* system CPU time used */
#ifdef BSDTIMES
		pdeltat(&r1->ru_stime, &r0->ru_stime);
#else
# ifdef _SEQUENT_
		pdeltat(&r1->ps_stime, &r0->ps_stime);
# else /* _SEQUENT_ */
#  ifndef POSIX
		pdtimet(es->tms_stime, bs->tms_stime);
#  else /* POSIX */
		pdtimet(es->tms_stime, bs->tms_stime);
#  endif /* POSIX */
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */
		break;

	case 'E':		/* elapsed (wall-clock) time */
#ifdef BSDTIMES
		pcsecs((long) ms);
#else /* BSDTIMES */
		pcsecs(ms);
#endif /* BSDTIMES */
		break;

	case 'P':		/* percent time spent running */
		i = (int) (t*1000 / ((ms ? ms : 1)));
		CSHprintf("%ld.%01ld%%", i/10, i%10); /* nn.n% */
		break;

#ifdef BSDTIMES
	case 'W':		/* number of swaps */
		i = r1->ru_nswap - r0->ru_nswap;
		CSHprintf("%ld", i);
		break;

	case 'X':		/* (average) shared text size */
		CSHprintf("%ld", t == 0 ? 0L :
		       IADJUST(r1->ru_ixrss-r0->ru_ixrss)/t);
		break;

	case 'D':		/* (average) unshared data size */
		CSHprintf("%ld", t == 0 ? 0L :
		       IADJUST(r1->ru_idrss+r1->ru_isrss-
			       (r0->ru_idrss+r0->ru_isrss))/t);
		break;

	case 'K':		/* (average) total data memory used  */
		CSHprintf("%ld", t == 0 ? 0L :
		       IADJUST((r1->ru_ixrss+r1->ru_isrss+r1->ru_idrss) -
		    (r0->ru_ixrss+r0->ru_idrss+r0->ru_isrss))/t);
		break;

	case 'M':		/* max. Resident Set Size */
#ifdef sun
		/* CSHprintf("%ld", r1->ru_maxrss * 1024L/(long) getpagesize()); */
		CSHprintf("%ld", pagetok(r1->ru_maxrss));
#else
		CSHprintf("%ld", r1->ru_maxrss/2L);
#endif /* sun */
		break;

	case 'F':		/* page faults */
		CSHprintf("%ld", r1->ru_majflt-r0->ru_majflt);
		break;

	case 'R':		/* page reclaims */
		CSHprintf("%ld", r1->ru_minflt-r0->ru_minflt);
		break;

	case 'I':		/* FS blocks in */
		CSHprintf("%ld", r1->ru_inblock-r0->ru_inblock);
		break;

	case 'O':		/* FS blocks out */
		CSHprintf("%ld", r1->ru_oublock-r0->ru_oublock);
		break;

	case 'r':		/* PWP: socket messages recieved */
		CSHprintf("%ld", r1->ru_msgrcv-r0->ru_msgrcv);
		break;

	case 's':		/* PWP: socket messages sent */
		CSHprintf("%ld", r1->ru_msgsnd-r0->ru_msgsnd);
		break;

	case 'k':		/* PWP: number of signals recieved */
		CSHprintf("%ld", r1->ru_nsignals-r0->ru_nsignals);
		break;

	case 'w':	/* PWP: num. voluntary context switches (waits) */
		CSHprintf("%ld", r1->ru_nvcsw-r0->ru_nvcsw);
		break;

	case 'c':	/* PWP: num. involuntary context switches */
		CSHprintf("%ld", r1->ru_nivcsw-r0->ru_nivcsw);
		break;
#else /* BSDTIMES */
# ifdef _SEQUENT_
	case 'W':		/* number of swaps */
		i = r1->ps_swap - r0->ps_swap;
		CSHprintf ("%ld", i);
		break;

	case 'M':
		CSHprintf("%ld", r1->ps_maxrss/2);
		break;

	case 'F':
		CSHprintf("%ld", r1->ps_pagein-r0->ps_pagein);
		break;

	case 'R':
		CSHprintf("%ld", r1->ps_reclaim-r0->ps_reclaim);
		break;

	case 'I':
		CSHprintf("%ld", r1->ps_bread-r0->ps_bread);
		break;

	case 'O':
		CSHprintf("%ld", r1->ps_bwrite-r0->ps_bwrite);
		break;
# endif /* _SEQUENT_ */
#endif /* BSDTIMES */
	}
	CSHputchar('\n');
}

#if defined(BSDTIMES) || defined(_SEQUENT_)
static void
pdeltat (t1, t0)
    timeval_t *t1, *t0;
{
    timeval_t td;

    tvsub (&td, t1, t0);
    CSHprintf ("%d.%01d", td.tv_sec, td.tv_usec / 100000);
}

void
tvadd (tsum, t0)
    timeval_t *tsum, *t0;
{

    tsum->tv_sec += t0->tv_sec;
    tsum->tv_usec += t0->tv_usec;
    if (tsum->tv_usec > 1000000)
	tsum->tv_sec++, tsum->tv_usec -= 1000000;
}

void
tvsub (tdiff, t1, t0)
    timeval_t *tdiff, *t1, *t0;
{

    tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
    tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
    if (tdiff->tv_usec < 0)
	tdiff->tv_sec--, tdiff->tv_usec += 1000000;
}
# else /* !BSDTIMES && !_SEQUENT_ */
static void
pdtimet(eval, bval)
#  ifndef POSIX
time_t eval, bval;
#  else /* POSIX */
clock_t eval, bval;
#  endif /* POSIX */
{
#  ifndef POSIX
	time_t val;
#  else /* POSIX */
	clock_t val;
#  endif /* POSIX */

#  ifndef POSIX
	val = (eval - bval) * 100 / HZ;
#  else /* POSIX */
	val = (eval - bval) * 100 / CLK_TCK;
#  endif /* POSIX */

	CSHprintf("%ld.%01ld", val / 100, val - (val / 100 * 100));
}
#endif /* BSDTIMES || _SEQUENT_ */

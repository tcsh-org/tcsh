/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/sh.rest.c,v 1.7 1991/03/20 19:04:50 christos Exp $ */
/*
 * sh.sig.c: Signal routine emulations
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id: sh.rest.c,v 1.7 1991/03/20 19:04:50 christos Exp $";

#endif

#include "sh.h"
#include "sh.local.h"


#ifndef BSDSIGS
/* libc.a contains these functions in SVID >= 3. */
#if SVID < 3 || defined(UNIXPC)
sigret_t
(*sigset(a, b)) ()
int     a;
int     (*b) ();
{
    return (signal(a, b));
}

/* release signal
 *	release all queued signals and
 *	set the default signal handler
 */
void
sigrelse(what)
int     what;
{
    if (what == SIGCHLD)
	sig_ch_rel();
}

/* hold signal
 * only works with child and interrupt
 */
void
sighold(what)
int     what;
{
    int     sig_ch_queue();

    if (what == SIGCHLD)
	(void) signal(SIGCHLD, sig_ch_queue);
}

/* ignore signal
 */
void
sigignore(a)
int     a;
{
    (void) signal(a, SIG_IGN);
}

#endif				/* SVID < 3 || (UNIXPC) */

/* this stack is used to queue signals
 * we can handle up to MAX_CHLD outstanding children now;
 */
#define MAX_CHLD 50
static struct mysigstack {
    int     s_w;		/* wait report			 */
    int     s_errno;		/* errno returned;		 */
    int     s_pid;		/* pid returned			 */
}       stk[MAX_CHLD];
static int stk_ptr = -1;


/* return either awaiting processes or do a wait now
 */
int
ourwait(w)
int    *w;
{
    int     pid;

#ifdef JOBDEBUG
    CSHprintf("our wait %d\n", stk_ptr);
    flush();
#endif				/* JOBDEBUG */

    if (stk_ptr == -1) {
	/* stack empty return signal from stack */
	pid = wait(w);
#ifdef JOBDEBUG
	CSHprintf("signal(SIGCHLD, pchild);\n");
#endif				/* JOBDEBUG */
	(void) signal(SIGCHLD, pchild);
	return (pid);
    }
    else {
	/* return signal from stack */
	errno = stk[stk_ptr].s_errno;
	*w = stk[stk_ptr].s_w;
	stk_ptr--;
	return (stk[stk_ptr + 1].s_pid);
    }
}				/* end ourwait */

/* queue child signals
 *
 */
static void
sig_ch_queue()
{
#ifdef JOBDEBUG
    CSHprintf("queue SIGCHLD\n");
    flush();
#endif				/* JOBDEBUG */
    stk_ptr++;
    stk[stk_ptr].s_pid = wait(&stk[stk_ptr].s_w);
    stk[stk_ptr].s_errno = errno;
    (void) signal(SIGCHLD, sig_ch_queue);
}




/* process all awaiting child signals
 */
static void
sig_ch_rel()
{
    while (stk_ptr > -1)
	pchild();
#ifdef JOBDEBUG
    CSHprintf("signal(SIGCHLD, pchild);\n");
#endif				/* JOBDEBUG */
    (void) signal(SIGCHLD, pchild);
}

/* libc.a contains sigpause.o in SVID >= 3. */
#if SVID < 3 || defined(UNIXPC)

/* atomically release one signal
 */
void
sigpause(what)
int     what;
{
#ifdef notdef
    if (what == SIGCHLD) {
	if (stk_ptr > -1) {
	    pchild();
	}
	else {
	    (void) sleep(1);
	}
    }
#endif
    /* From: Jim Mattson <mattson%cs@ucsd.edu> */
    if (what == SIGCHLD)
	pchild();

}

#endif				/* SVID < 3 || (UNIXPC) */

# ifdef SXA
/*
 * SX/A is SVID3 but does not have sys5-sigpause().
 * I've heard that sigpause() is not defined in SVID3.
 */
/* This is not need if you make tcsh by BSD option's cc. */
void
sigpause(what)
{
    if (what == SIGCHLD) {
	bsd_sigpause(bsd_sigblock(0) & ~sigmask(SIGBSDCHLD));
    }
    else if (what == 0) {
	pause();
    }
    else {
	CSHprintf("sigpause(%d)\n", what);
	pause();
    }
}

# endif	/* SXA */
#endif	/* BSDSIGS */

#if defined(hpux) || defined(aiws)
/* turn into bsd signals */
sigret_t(*
	 signal(s, a)) ()
int     s;

sigret_t(*a) ();
{
    sigvec_t osv,
            sv;

    (void) mysigvec(s, (struct sigvec *) 0, &osv);
    sv = osv;
    sv.sv_handler = a;
#ifdef SIG_STK
    sv.sv_onstack = SIG_STK;
#endif
#if defined(SV_BSDSIG) && defined(SV_ONSTACK)
    sv.sv_flags = SV_BSDSIG | SV_ONSTACK;
#endif

    if (mysigvec(s, &sv, (sigvec_t *) 0) < 0)
	return (BADSIG);
    return (osv.sv_handler);
}
#endif /* hpux || aiws */

#ifdef _SEQUENT_
/*
 * Support for signals.
 */

extern int errno;

/* Set and test a bit.  Bits numbered 1 to 32 */

#define SETBIT(x, y)	x |= (1 << ((y) -1) )
#define ISSET(x, y)	((x & (1 << ((y) - 1))) != 0)

/* #define SHOW_SIGNALS	1	/* to assist in debugging signals */

#ifdef SHOW_SIGNALS
char   *show_sig_mask();

#endif

int     debug_signals = 0;

/*
 * igsetmask(mask)
 *
 * Set a new signal mask.  Return old mask.
 */
sigmask_t
sigsetmask(mask)
int     mask;
{
    sigset_t set,
            oset;
    int     m;
    register int i;

    sigemptyset(&set);
    sigemptyset(&oset);

    for (i = 1; i <= MAXSIG; i++)
	if (ISSET(mask, i))
	    sigaddset(&set, i);

    if (sigprocmask(SIG_SETMASK, &set, &oset))
	CSHprintf("sigsetmask(0x%x) - sigprocmask failed, errno %d",
		  mask, errno);

    m = 0;
    for (i = 1; i < MAXSIG; i++)
	if (sigismember(&oset, i))
	    SETBIT(m, i);

    return (m);
}

/*
 * sigblock(mask)
 *
 * Add "mask" set of signals to the present signal mask.
 * Return old mask.
 */
sigmask_t
sigblock(mask)
int     mask;
{
    sigset_t set,
            oset;
    int     m;
    register int i;

    set = 0;
    oset = 0;

    /* Get present set of signals. */
    if (sigprocmask(SIG_SETMASK, NULL, &set))
	CSHprintf("sigblock(0x%x) - sigprocmask failed, errno %d",
		  mask, errno);

    /* Add in signals from mask. */
    for (i = 1; i <= MAXSIG; i++)
	if (ISSET(mask, i))
	    sigaddset(&set, i);

    sigprocmask(SIG_SETMASK, &set, &oset);

    /* Return old mask to user. */
    m = 0;
    for (i = 1; i < MAXSIG; i++)
	if (sigismember(&oset, i))
	    SETBIT(m, i);

    return (m);
}


/*
 * bsd_sigpause(mask)
 *
 * Set new signal mask and wait for signal;
 * Old mask is restored on signal.
 */
void
bsd_sigpause(mask)
int     mask;
{
    sigset_t set;
    register int i;

    sigemptyset(&set);

    for (i = 1; i <= MAXSIG; i++)
	if (ISSET(mask, i))
	    sigaddset(&set, i);
    sigsuspend(&set);
}
#endif /* _SEQUENT_ */


#ifdef SIGSYNCH
static long Synch_Cnt = 0;
sigret_t 
synch_handler(sno)
{
    if (sno != SIGSYNCH)
	abort();
    Synch_Cnt++;
}
#endif /* SIGSYNCH */

#ifdef SAVESIGVEC
sigmask_t
savesigvec(sv)
sigvec_t *sv;
{
    (void) mysigvec(SIGINT,  (sigvec_t *) 0, &sv[0]);
    (void) mysigvec(SIGQUIT, (sigvec_t *) 0, &sv[1]);
    (void) mysigvec(SIGTSTP, (sigvec_t *) 0, &sv[2]);
    (void) mysigvec(SIGTTIN, (sigvec_t *) 0, &sv[3]);
    (void) mysigvec(SIGTTOU, (sigvec_t *) 0, &sv[4]);
    (void) mysigvec(SIGTERM, (sigvec_t *) 0, &sv[5]);
    (void) mysigvec(SIGHUP,  (sigvec_t *) 0, &sv[6]);

    /*
     * Can't handle any of these signals until sigvec's
     * are restored (sg)
     */

    return(sigblock(sigmask(SIGINT) | sigmask(SIGQUIT) | sigmask(SIGTSTP) | 
		    sigmask(SIGTTIN) | sigmask(SIGTTOU) | sigmask(SIGTERM) |
		    sigmask(SIGHUP)));
} 

void
restoresigvec(sv, sm)
sigvec_t *sv;
sigmask_t sm;
{
    (void) mysigvec(SIGINT,  &sv[0], (sigvec_t *) 0);
    (void) mysigvec(SIGQUIT, &sv[1], (sigvec_t *) 0);
    (void) mysigvec(SIGTSTP, &sv[2], (sigvec_t *) 0);
    (void) mysigvec(SIGTTIN, &sv[3], (sigvec_t *) 0);
    (void) mysigvec(SIGTTOU, &sv[4], (sigvec_t *) 0);
    (void) mysigvec(SIGTERM, &sv[5], (sigvec_t *) 0);
    (void) mysigvec(SIGHUP,  &sv[6], (sigvec_t *) 0);
    (void) sigsetmask(sm);
}
#endif /* SAVESIGVEC */

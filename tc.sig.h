/* $Header$ */
/*
 * tc.sig.h: Signal handling
 *
 */
#ifndef _h_tc_sig
#define _h_tc_sig

#if SVID > 0
#include <signal.h>
#ifndef SIGCHLD
#define SIGCHLD SIGCLD
#endif				/* SIGCHLD */
#else				/* SVID == 0 */
#include <sys/signal.h>
#endif				/* SVID > 0 */

#ifdef SIGVOID
typedef void sigret_t;

#else
typedef int sigret_t;

#endif				/* SIGVOID */

#ifdef sun
# define SAVESIGVEC
#endif

#ifdef BSDSIGS
/*
 * sigvec is not the same everywhere
 */
#ifdef _SEQUENT_
#define HAVE_SIGVEC
#define mysigvec(a, b, c)	sigaction(a, b, c)
typedef struct sigaction sigvec_t;

#define sv_handler sa_handler
#endif				/* _SEQUENT */

#ifdef hpux
#define HAVE_SIGVEC
#define mysigvec(a, b, c)	sigvector(a, b, c)
typedef struct sigvec sigvec_t;

#endif				/* hpux */

#ifndef HAVE_SIGVEC
#define mysigvec(a, b, c)	sigvec(a, b, c)
typedef struct sigvec sigvec_t;

#endif				/* HAVE_SIGVEC */

#undef HAVE_SIGVEC
#endif				/* BSDSIGS */

#if SVID > 0
#ifdef BSDJOBS
/* here I assume that systems that have bsdjobs implement the
 * the setpgrp call correctly. Otherwise defining this would
 * work, but it would kill the world, because all the setpgrp
 * code is the the part defined when BSDJOBS are defined
 * NOTE: we don't want killpg(a, b) == kill(-getpgrp(a), b)
 * cause process a might be already dead and getpgrp would fail
 */
#define killpg(a, b) kill(-(a), (b))
#else
/* this is the poor man's version of killpg()! Just kill the
 * current process and don't worry about the rest. Someday
 * I hope I get to fix that.
 */
#define killpg(a, b) kill((a), (b))
#endif				/* BSDJOBS */
#endif				/* SVID > 0 */


#ifdef BSDSIGS
/*
 * For 4.2bsd signals.
 */
#ifdef sigmask
#undef sigmask
#endif				/* sigmask */
#define	sigmask(s)	(1 << ((s)-1))
#ifdef _SEQUENT_
#define 	sigpause(a)	bsd_sigpause(a)
#define 	signal(a, b)	sigset(a, b)
#else				/* _SEQUENT_ */
#define	sighold(s)	sigblock(sigmask(s))
#define	sigignore(s)	signal(s, SIG_IGN)
#define 	sigset(s, a)	signal(s, a)
#endif				/* _SEQUENT_ */
#ifdef aiws
#define 	sigrelse(a)	sigsetmask(sigblock(0) & ~sigmask(a))
#undef	killpg
#define 	killpg(a, b)	kill(-getpgrp(a), b)
#endif				/* aiws */
#endif				/* BSDSIGS */


/* PWP: for everybody */
#define	sigsys(s, a)	signal(s, a)

/*
 * We choose a define for the window signal if it exists..
 */
#ifdef SIGWINCH
#define SIG_WINDOW SIGWINCH
#else
#ifdef SIGWINDOW
#define SIG_WINDOW SIGWINDOW
#endif				/* SIGWINDOW */
#endif				/* SIGWINCH */

#if defined(convex) || defined(__convex__)
#define SIGSYNCH       0
#ifdef SIGSYNCH
#define SYNCHMASK 	(sigmask(SIGCHLD)|sigmask(SYNCH_SIG))
#else
#define SYNCHMASK 	(sigmask(SIGCHLD))
#endif
extern sigret_t synch_handler();

#endif				/* convex || __convex__ */

#ifdef SAVESIGVEC
# define NSIGSAVED 7
extern sigmask_t savesigvec();
extern void restoresigvec();
#endif /* SAVESIGVEC */

#endif				/* _h_tc_sig */

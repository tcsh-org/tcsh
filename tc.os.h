/* $Header$ */
/*
 * tc.os.h: Shell os dependent defines
 */
#ifndef _h_tc_os
#define _h_tc_os

#ifdef OREO
# include <sys/time.h>
# include <sys/resource.h>
#endif				/* OREO */

#ifdef hpux
# ifdef lint
/*
 * Hpux defines struct ucred, in <sys/user.h>, but if I include that
 * then I need to include the *world*
 * [all this to pass lint cleanly!!!]
 * so I define struct ucred here...
 */
struct ucred {
    int     foo;
};
# endif /* lint */

/*
 * hpux 7.0 does not define it
 */
# ifndef CSUSP
#  define CSUSP 032
# endif				/* CSUSP */

# ifndef POSIX
#  include <sys/bsdtty.h>
# endif				/* POSIX */

# ifdef BSDJOBS
#  define getpgrp(a) getpgrp2(a)
#  define setpgrp(a, b) setpgrp2(a, b)
# endif

#endif				/* hpux */

#ifdef HYPERCUBE
/*
 * for struct winsiz
 */
# include <sys/stream.h>
# include <sys/ptem.h>
# define gethostname(a, b) xgethostname(a, b)
#endif				/* HYPERCUBE */

#ifdef IRIS4D
# include <sys/time.h>
# include <sys/resource.h>
# include <bsd/net/soioctl.h>	/* this is the SGI TIOSPGRP etc. */
# define TIOSPGRP SIOSPGRP
# define TIOGPGRP SIOGPGRP
/*
 * BSDsetpgrp() and BSDgetpgrp() aren't documented and have no reference
 *   anywhere.  I found them out in libc.a, hope they're what I think.
 */
# define setpgrp BSDsetpgrp
# define getpgrp BSDgetpgrp
#endif				/* IRIS4D */

#ifdef ISC
/* these are not defined for _POSIX_SOURCE under ISC 2.2 */
# ifndef S_IFMT
#  define S_IFMT  0170000		/* type of file */
#  define S_IFDIR 0040000		/* directory */
#  define S_IFCHR 0020000		/* character special */
#  define S_IFBLK 0060000		/* block special */
#  define S_IFREG 0100000		/* regular */
#  define S_IFIFO 0010000		/* fifo */
#  define S_IFNAM 0050000		/* special named file */
# endif				/* S_IFMT */
#endif				/* ISC */


#ifdef _SEQUENT_
# include <sys/procstats.h>
# define gethostname(a, b) xgethostname(a, b)
#endif				/* _SEQUENT_ */

#if defined(BSD) && defined(POSIXJOBS)
# define setpgid(pid, pgrp)	setpgrp(pid, pgrp)
#endif				/* BSD && POSIXJOBS */

#if defined(BSDJOBS) && !(defined(POSIX) && defined(POSIXJOBS))
# if !defined(_AIX370) && !defined(_AIXPS2)
#  define setpgid(pid, pgrp)	setpgrp(pid, pgrp)
# endif				/* !_AIX370 && !_AIXPS2 */
# define tcsetpgrp(fd, pgrp)	ioctl((fd), TIOCSPGRP, (ioctl_t) &(pgrp))
# define tcgetpgrp(fd)		xtcgetpgrp(fd)
extern int xtcgetpgrp();
# define NEED_XTCGETPGRP
#endif				/* BSDJOBS && !(POSIX && POSIXJOBS) */

#ifdef RENO
/* tcgetpgrp() core dumps */
# define NEED_XTCGETPGRP
# define tcsetpgrp(fd, pgrp)	ioctl((fd), TIOCSPGRP, (ioctl_t) &(pgrp))
# define tcgetpgrp(fd) xtcgetpgrp(fd)
#endif				/* RENO */

#ifdef DGUX
# define setpgrp(a, b) setpgrp2(a, b)
# define getpgrp(a) getpgrp2(a)
#endif				/* DGUX */

#ifdef SXA
# ifndef BSDNICE
/*
 * We check BSDNICE cause this is not defined in config.sxa.
 * Only needed in the system V environment.
 */
#  define setrlimit(a, b) 	bsd_setrlimit(a, b)
#  define getrlimit(a, b)	bsd_getrlimit(a, b)
# endif				/* BSDNICE */
# ifndef NOFILE
#  define	NOFILE	64
# endif				/* NOFILE */
#endif				/* SXA */

#ifndef POSIX
# define mygetpgrp()    getpgrp(0)
#else				/* POSIX */
# if defined(BSD) || defined(sun)
#  define mygetpgrp()    getpgrp(0)
# else				/* BSD || sun */
#  define mygetpgrp()    getpgrp()
# endif				/* BSD || sun */
#endif				/* POSIX */

#ifndef R_OK
# define R_OK 4
#endif
#ifndef F_OK
# define F_OK 0
#endif

#if SVID > 0 && !defined(OREO) && !defined(IRIS4D)
# define getwd(a) xgetwd(a)
extern char *xgetwd();
#endif				/* SVID > 0 && !OREO && !IRIS4D */

#ifndef S_IFLNK
# define lstat stat
#endif				/* S_IFLNK */


#if defined(BSDTIMES) && !defined(_SEQUENT_)
typedef struct timeval timeval_t;
#endif

#ifdef NeXT
/*
 * From Tony_Mason@transarc.com, override NeXT's malloc stuff.
 */
# define malloc tcsh_malloc
# define calloc tcsh_calloc
# define realloc tcsh_realloc
# define free tcsh_free
#endif


#ifndef POSIX
extern time_t time();
extern char *getenv();
extern int atoi();
# ifndef gethostname
extern int gethostname();
# else
extern int xgethostname();
# endif
# ifdef BSDSIGS
#  ifndef _AIX370
extern sigret_t sigvec();
extern void sigpause();
#  else
extern int sigvec();
extern int sigpause();
#  endif
extern sigmask_t sigblock();
extern sigmask_t sigsetmask();
# endif
# ifndef killpg
extern int killpg();
# endif
extern char *ttyname();
extern void abort();
extern void perror();
extern int lstat();
extern int getrlimit();
extern int setrlimit();
# if defined(NLS) && !defined(NOSTRCOLL)
extern int strcoll();
# endif
extern void qsort();
# ifdef BSDJOBS
#  ifdef BSDTIMES
extern int wait3();
#  else /* ! BSDTIMES */
#   if !defined(POSIXJOBS) && !defined(_SEQUENT_)
extern int wait3();
#   else /* POSIXJOBS || _SEQUENT_ */
extern int waitpid();
#   endif /* POSIXJOBS || _SEQUENT_ */
#  endif /* ! BSDTIMES */
# else /* !BSDJOBS */
#  if SVID < 3
extern int ourwait();
#  else /* SVID >= 3 */
extern int wait();
#  endif /* SVID >= 3 */
# endif /* ! BSDJOBS */
extern int gettimeofday();
# ifdef BSDNICE
extern int setpriority();
# else
extern int nice();
# endif
# ifdef BSDTIMES
extern int getrusage();
# endif
extern void setpwent();
extern void endpwent();
extern struct passwd *getpwuid (), *getpwnam (), *getpwent();
# ifndef getwd
extern char *getwd();
# endif
#else
# if defined(sun) && !defined(__GNUC__)
extern char *getwd();
# endif
#endif


#endif				/* _h_tc_os */

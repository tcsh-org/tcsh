/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.types.h,v 3.3 1991/07/15 19:37:24 christos Exp $ */
/* sh.types.h: Do the necessary typedefs for each system.
 *             Up till now I avoided making this into a separate file
 *	       But I just wanted to eliminate the whole mess from sh.h
 *	       In reality this should not be here! It is OS and MACHINE
 *	       dependent, even between different revisions of OS's...
 *	       Ideally there should be a way in c, to find out if something
 *	       was typedef'ed, but unfortunately we rely in cpp kludges.
 *	       Someday, this file will be removed... 
 *						
 *						christos
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
#ifndef _h_sh_types
#define _h_sh_types


/***
 *** Suns running sunos4.1
 ***/
#ifdef sun
/* This used to be long, but lint dissagrees... */
# ifndef _SIGMASK_T
#  define _SIGMASK_T
    typedef int sigmask_t;
# endif /* _SIGMASK_T */
# ifndef _PTR_T
#  define _PTR_T 
#   ifdef __GNUC__
    typedef void * ptr_t;
#   else
    typedef char * ptr_t;
#   endif /* __GNUC__ */
# endif /* _PTR_T */
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
# ifndef __sys_stdtypes_h
#  define __sys_stdtypes_h
    typedef int pid_t;
    typedef unsigned int speed_t;
# endif /* __sys_stdtypes.h */
# ifndef _PID_T
#  define _PID_T
# endif /* _PID_T */
# ifndef _SPEED_T
#  define _SPEED_T
# endif /* _SPEED_T */
# ifdef notdef
/*
 * This is what sun's lint wants, but the .h file disagree
 */
extern char *getwd();
extern time_t time();
extern int getuid(), geteuid();
extern int getgid(), getguid();
extern int _exit();
extern int abort();
extern int alarm();
extern void endpwent();
extern char *sbrk();
extern int sleep();
# endif /* notdef */
#endif /*sun */


/***
 *** Hp's running hpux 7.0
 ***/
#ifdef __hpux
# ifndef _SIZE_T
#  define _SIZE_T
    typedef unsigned int size_t;
# endif /* _SIZE_T */

# ifndef _PTR_T
# define _PTR_T 
    typedef void * ptr_t;
# endif /* _PTR_T */

# ifndef _PID_T
#  define _PID_T
    typedef long pid_t;
# endif /* _PID_T */

# ifndef _SIGMASK_T
#  define _SIGMASK_T
    typedef long sigmask_t;
# endif /* _SIGMASK_T */
  
# ifndef _SPEED_T
   /* I thought POSIX was supposed to protect all typedefs! */
#  define _SPEED_T
# endif /* _SPEED_T */

extern uid_t getuid(), geteuid();
extern gid_t getgid(), getegid();
extern sigmask_t sigblock();
extern sigmask_t sigsetmask();
#ifndef __hp9000s700
extern void sigpause();
extern sigmask_t sigspace();
#endif
extern pid_t getpid();
extern pid_t fork();
extern void perror();
extern void _exit();
extern void abort();
extern void qsort();
extern void free();
extern unsigned int alarm();
extern unsigned int sleep();
#ifndef __hp9000s700
extern int lstat();
extern int readlink();
extern int sigvector();
extern int gethostname();
extern int ioctl();
extern int nice();
extern char *sbrk();
#endif
#endif /* __hpux */


/***
 *** Data General 88000, running dgux ???
 ***/
#ifdef DGUX
/*
 * DGUX types
 */
# ifdef ___int_size_t_h
#  ifdef _TARGETTING_M88KBCS_OR_DGUX
#   ifdef _USING_ANSI_C_OR_POSIX_OR_SYSV3_OR_BSD_OR_DGUX
#    ifndef _SIZE_T
#     define _SIZE_T
#    endif /* _SIZE_T */
#   endif  /* #ifdef _USING_ANSI_C_OR_POSIX_OR_SYSV3_OR_BSD_OR_DGUX */
#  endif  /* #ifdef _TARGETTING_M88KBCS_OR_DGUX */
# endif  /* #ifndef ___int_size_t_h */

# ifdef _USING_POSIX_OR_SYSV3_OR_BSD_OR_DGUX
#  ifndef _PID_T
#   define _PID_T
#  endif /* _PID_T */
# endif  /* #ifdef _USING_POSIX_OR_SYSV3_OR_BSD_OR_DGUX */

#endif


/***
 *** a PFU/Fujitsu A-xx computer SX/A Edition 60 or later
 ***/
#ifdef SXA
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
#endif /* SXA */


/***
 *** BSD systems, pre and post 4.3
 ***/
#ifdef BSD
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
#endif /* BSD */
/***
 *** BSD RENO advertises itself as POSIX, but
 *** it is missing speed_t (newer versions of RENO have it)
 ***/
#ifdef RENO
# ifndef _SPEED_T
#  define _SPEED_T
/*   typedef unsigned int speed_t; */
# endif /* _SPEED_T */
#endif /* RENO */


/***
 *** Pyramid, BSD universe
 *** In addition to the size_t
 ***/
#ifdef pyr
# ifndef _PID_T
#  define _PID_T
# endif /* _PID_T */
#endif /* pyr */


/***
 *** rs6000, ibm370, ps2: running flavors of aix.
 ***/
#ifdef IBMAIX
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
# ifndef _PID_T
#  define _PID_T
# endif /* _PID_T */
# ifdef _IBMR2
#  ifndef _SPEED_T 
#   define _SPEED_T
#  endif /* _SPEED_T */
# endif /* _IBMR2 */
#endif /* IBMAIX */


/***
 *** Ultrix...
 ***/
#ifdef ultrix
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
# ifndef _PID_T
#  define _PID_T
# endif /* _PID_T */
#endif /* ultrix */


/***
 *** Silicon graphics IRIS4D running IRIX3_3
 ***/
#if defined(IRIS4D) && defined(IRIX3_3)
# ifndef _PID_T
#  define _PID_T
# endif /* _PID_T */
#endif /* IRIS4D && IRIX3_3 */


/***
 *** Sequent
 ***/
#ifdef sequent
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
#endif /* sequent */

/***
 *** Apple AUX.
 ***/
#ifdef OREO
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
# ifndef _PID_T
#  define _PID_T
# endif /* _PID_T */
#endif /* OREO */

/***
 *** Intel 386, Hypercube
 ***/
#ifdef HYPERCUBE
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
#endif /* HYPERCUBE */

/***
 *** Concurrent (Masscomp) running RTU 4.1A & RTU 5.0.
 *** Added, DAS DEC-90.
 ***/
#ifdef	masscomp
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
#endif	/* masscomp */

/***
 *** Encore multimax running umax 4.2
 ***/
#ifdef	ns32000
# ifdef __TYPES_DOT_H__
#  ifndef _SIZE_T
#   define _SIZE_T
#  endif /* _SIZE_T */
# endif /* __TYPES_DOT_H__ */
#endif	/* ns32000 */

/***
 *** Silicon Graphics IRIS 3000
 ***
 ***/
#ifdef IRIS3D
# ifndef _SIZE_T
#  define _SIZE_T
# endif /* _SIZE_T */
#endif /* IRIS3D */

/***
 *** Catch all for non POSIX systems.
 *** Posix things *should* define these automatically
 *** I am open to suggestions on how to do this correctly!
 ***/
#ifndef POSIX
# ifndef _SIZE_T
#  define _SIZE_T
   typedef int size_t;		/* As sun comments ??? : meaning I take it */
# endif /* _SIZE_T */		/* Until we make the world POSIX... */

# ifndef _PID_T
#  define _PID_T
   typedef int pid_t;
# endif /* _PID_T */

# ifndef _SPEED_T
#  define _SPEED_T
   typedef unsigned int speed_t;
# endif /* _SPEED_T */

#ifndef _PTR_T
# define _PTR_T 
    typedef char * ptr_t;
#endif /* _PTR_T */

#ifndef _IOCTL_T
# define _IOCTL_T
    typedef char * ioctl_t;	/* Third arg of ioctl */
#endif /* _IOCTL_T */

#endif /* ! POSIX */



/***
 *** This is our own junk types.
 ***/
#ifndef _PTR_T
# define _PTR_T 
    typedef void * ptr_t;
#endif /* _PTR_T */

#ifndef _SIGMASK_T
# define _SIGMASK_T
    typedef int sigmask_t;
#endif /* _SIGMASK_T */

#ifndef _IOCTL_T
# define _IOCTL_T
    typedef void * ioctl_t;	/* Third arg of ioctl */
#endif /* _IOCTL_T */

#endif /* _h_sh_types */

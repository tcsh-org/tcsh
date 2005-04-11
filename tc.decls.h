/* $Header: /src/pub/tcsh/tc.decls.h,v 3.57 2005/01/18 20:24:51 christos Exp $ */
/*
 * tc.decls.h: Function declarations from all the tcsh modules
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
 * 3. Neither the name of the University nor the names of its contributors
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
#ifndef _h_tc_decls
#define _h_tc_decls

/*
 * tc.alloc.c
 */
#ifndef SYSMALLOC
#ifndef WINNT_NATIVE
extern	void		  free		(ptr_t);
extern	memalign_t	  malloc	(size_t);
extern	memalign_t	  realloc	(ptr_t, size_t);
extern	memalign_t	  calloc	(size_t, size_t);
#endif /* !WINNT_NATIVE */
#else /* SYSMALLOC */
extern	void		  sfree		(ptr_t);
extern	memalign_t	  smalloc	(size_t);
extern	memalign_t	  srealloc	(ptr_t, size_t);
extern	memalign_t	  scalloc	(size_t, size_t);
#endif /* SYSMALLOC */
extern	void		  showall	(Char **, struct command *);

/*
 * tc.bind.c
 */
extern	void		  dobindkey	(Char **, struct command *);
#ifdef OBSOLETE
extern	void		  dobind	(Char **, struct command *);
#endif /* OBSOLETE */

/*
 * tc.defs.c:
 */
extern	void		  getmachine	(void);


/*
 * tc.disc.c
 */
extern	int		  setdisc	(int);
extern	int		  resetdisc	(int);

/*
 * tc.func.c
 */
extern	Char		 *expand_lex    (Char *, size_t, struct wordent *, 
					 int, int);
extern	Char		 *sprlex	(Char *, size_t, struct wordent *);
extern	Char		 *Itoa		(int, Char *, int, int);
extern	void		  dolist	(Char **, struct command *);
extern	void		  dotermname	(Char **, struct command *);
extern	void		  dotelltc	(Char **, struct command *);
extern	void		  doechotc	(Char **, struct command *);
extern	void		  dosettc	(Char **, struct command *);
extern	int		  cmd_expand	(Char *, Char *);
extern	void		  dowhich	(Char **, struct command *);
extern	struct process	 *find_stop_ed	(void);
extern	void		  fg_proc_entry	(struct process *);
extern	RETSIGTYPE	  alrmcatch	(int);
extern	void		  precmd	(void);
extern	void		  postcmd	(void);
extern	void		  cwd_cmd	(void);
extern	void		  beep_cmd	(void);
extern	void		  period_cmd	(void);
extern	void		  job_cmd	(Char *);
extern	void		  aliasrun	(int, Char *, Char *);
extern	void		  setalarm	(int);
extern	void		  rmstar	(struct wordent *);
extern	void		  continue_jobs	(struct wordent *);
extern	Char		 *gettilde	(Char *);
extern	Char		 *getusername	(Char **);
#ifdef OBSOLETE
extern	void		  doaliases	(Char **, struct command *);
#endif /* OBSOLETE */
extern	void		  shlvl		(int);
extern	int		  fixio		(int, int);
extern	int		  collate	(const Char *, const Char *);
#ifdef HASHBANG
extern	int		  hashbang	(int, Char ***);
#endif /* HASHBANG */
#ifdef REMOTEHOST
extern	void		  remotehost	(void);
#endif /* REMOTEHOST */


/*
 * tc.os.c
 */
#ifdef MACH
extern	void		  dosetpath	(Char **, struct command *);
#endif /* MACH */

#ifdef TCF
extern	void		  dogetxvers	(Char **, struct command *);
extern	void		  dosetxvers	(Char **, struct command *);
extern	void		  dogetspath	(Char **, struct command *);
extern	void		  dosetspath	(Char **, struct command *);
extern	char		 *sitename	(pid_t);
extern	void		  domigrate	(Char **, struct command *);
#endif /* TCF */

#ifdef WARP
extern	void 		  dowarp	(Char **, struct command *);
#endif /* WARP */

#if defined(_CRAY) && !defined(_CRAYMPP)
extern	void 		  dodmmode	(Char **, struct command *);
#endif /* _CRAY && !_CRAYMPP */

#if defined(masscomp) || defined(hcx)
extern	void		  douniverse	(Char **, struct command *);
#endif /* masscomp */

#if defined(_OSD_POSIX) /* BS2000 */
extern	void		  dobs2cmd	(Char **, struct command *);
#endif /* _OSD_POSIX */

#if defined(hcx)
extern	void		  doatt		(Char **, struct command *);
extern	void		  doucb		(Char **, struct command *);
#endif /* hcx */

#ifdef _SEQUENT_
extern	void	 	  pr_stat_sub	(struct process_stats *, 
					 struct process_stats *, 
					 struct process_stats *);
#endif /* _SEQUENT_ */

#ifdef NEEDtcgetpgrp
extern	int	 	  xtcgetpgrp	(int);
extern	int		  xtcsetpgrp	(int, int);
# undef tcgetpgrp
# define tcgetpgrp(a) 	  xtcgetpgrp(a)
# undef tcsetpgrp
# define tcsetpgrp(a, b)  xtcsetpgrp((a), (b))
#endif /* NEEDtcgetpgrp */

#ifdef YPBUGS
extern	void	 	  fix_yp_bugs	(void);
#endif /* YPBUGS */
#ifdef STRCOLLBUG
extern	void	 	  fix_strcoll_bug	(void);
#endif /* STRCOLLBUG */

extern	void	 	  osinit	(void);

#ifndef HAVE_MEMMOVE
extern ptr_t 		 xmemmove	(ptr_t, const ptr_t, size_t);
# define memmove(a, b, c) xmemmove(a, b, c)
#endif /* !HAVE_MEMMOVE */

#ifndef HAVE_MEMSET
extern ptr_t 		 xmemset	(ptr_t, int, size_t);
# define memset(a, b, c) xmemset(a, b, c)
#endif /* !HAVE_MEMSET */


#ifndef HAVE_GETCWD
extern	char		 *xgetcwd	(char *, size_t);
# undef getcwd
# define getcwd(a, b) xgetcwd(a, b)
#endif /* !HAVE_GETCWD */

#ifndef HAVE_GETHOSTNAME
extern	int	 	  xgethostname	(char *, int);
# undef gethostname
# define gethostname(a, b) xgethostname(a, b)
#endif /* !HAVE_GETHOSTNAME */

#ifndef HAVE_NICE
extern	int	 	  xnice	(int);
# undef nice
# define nice(a)	  xnice(a)
#endif /* !HAVE_NICE */

#ifndef HAVE_STRERROR
extern	char	 	 *xstrerror	(int);
# undef strerror
# define strerror(a) 	  xstrerror(a)
#endif /* !HAVE_STRERROR */

#ifdef apollo
extern	void		  doinlib	(Char **, struct command *);
extern	void		  dover		(Char **, struct command *);
extern	void		  dorootnode	(Char **, struct command *);
extern	int		  getv		(Char *);
#endif /* apollo */


/*
 * tc.printf.h
 */
extern	pret_t		  xprintf	(const char *, ...);
extern	pret_t		  xsnprintf	(char *, size_t, const char *, ...);
extern	pret_t		  xvprintf	(const char *, va_list);
extern	pret_t		  xvsnprintf	(char *, size_t, const char *,
					 va_list);

/*
 * tc.prompt.c
 */
extern	void		  dateinit	(void);
extern	void		  printprompt	(int, const char *);
extern  Char 		 *expdollar	(Char **, const Char **, size_t *,
					 int);
extern	void		  tprintf	(int, Char *, const Char *, size_t, 
					 const char *, time_t, ptr_t);

/*
 * tc.sched.c
 */
extern	time_t		  sched_next	(void);
extern	void		  dosched	(Char **, struct command *);
extern	void		  sched_run	(int);

/*
 * tc.sig.c
 */
#ifndef BSDSIGS
# ifdef UNRELSIGS
#  ifdef COHERENT
extern	RETSIGTYPE	(*xsignal	(int, RETSIGTYPE (*)(int))) ();
#   define signal(x,y)	  xsignal(x,y)
#  endif /* COHERENT */
extern	RETSIGTYPE	(*xsigset	(int, RETSIGTYPE (*)(int))) ();
#  define sigset(x,y)	  xsigset(x,y)
extern	void		  xsigrelse	(int);
#  define sigrelse(x)	  xsigrelse(x)
extern	void		  xsighold	(int);
#  define sighold(x)	  xsighold(x)
extern	void		  xsigignore	(int);
#  define sigignore(x)	  xsigignore(x)
extern	void 		  xsigpause	(int);
#  define sigpause(x)	  xsigpause(x)
extern	pid_t 		  ourwait	(int *);
# endif /* UNRELSIGS */
# ifdef SXA
extern	void 		  sigpause	(int);
# endif /* SXA */
#endif /* !BSDSIGS */

#ifdef NEEDsignal
extern	RETSIGTYPE	(*xsignal	(int, RETSIGTYPE (*)(int))) ();
# define signal(a, b)	  xsignal(a, b)
#endif /* NEEDsignal */
#if defined(_SEQUENT_) || ((SYSVREL > 3 || defined(_DGUX_SOURCE)) && defined(POSIXSIGS)) || ((defined(_AIX) || defined(__CYGWIN__)) && defined(POSIXSIGS)) || defined(WINNT_NATIVE)
extern	sigmask_t	  sigsetmask	(sigmask_t);
# if !defined(DGUX) || (defined(DGUX) && defined(__ix86))
extern	sigmask_t	  sigblock	(sigmask_t);
# endif /* !DGUX */
extern	void		  bsd_sigpause	(sigmask_t);
extern  RETSIGTYPE        (*bsd_signal    (int, RETSIGTYPE (*)(int))) (int);
#endif /* _SEQUENT_ */
#ifdef SIGSYNCH
extern	RETSIGTYPE	  synch_handler	(int);
#endif /* SIGSYNCH */


/*
 * tc.str.c:
 */
#ifdef WIDE_STRINGS
extern	size_t		  one_mbtowc	(wchar_t *, const char *, size_t);
extern	size_t		  one_wctomb	(char *, wchar_t);
#else
#define one_mbtowc(PWC, S, N) \
	((void)(N), *(PWC) = (unsigned char)*(S), (size_t)1)
#define one_wctomb(S, WCHAR) (*(S) = (WCHAR), (size_t)1)
#endif
#ifdef SHORT_STRINGS
extern  int		  rt_mbtowc	(wchar_t *, const char *, size_t);
extern	Char		 *s_strchr	(const Char *, int);
extern	Char		 *s_strrchr	(const Char *, int);
extern	Char		 *s_strcat	(Char *, const Char *);
# ifdef NOTUSED
extern	Char		 *s_strncat	(Char *, const Char *, size_t);
# endif /* NOTUSED */
extern	Char		 *s_strcpy	(Char *, const Char *);
extern	Char		 *s_strncpy	(Char *, const Char *, size_t);
extern	Char		 *s_strspl	(const Char *, const Char *);
extern	size_t		  s_strlen	(const Char *);
extern	int		  s_strcmp	(const Char *, const Char *);
extern	int		  s_strncmp	(const Char *, const Char *, size_t);
extern	int		  s_strcasecmp	(const Char *, const Char *);
extern	Char		 *s_strsave	(const Char *);
extern	Char		 *s_strend	(const Char *);
extern	Char		 *s_strstr	(const Char *, const Char *);
extern	Char		 *str2short	(const char *);
extern	Char		**blk2short	(char **);
extern	char		 *short2str	(const Char *);
extern	char		**short2blk	(Char **);
#endif /* SHORT_STRINGS */
extern	char		 *short2qstr	(const Char *);


/*
 * tc.vers.c:
 */
extern	void		  fix_version	(void);

/*
 * tc.who.c
 */
#if defined (HAVE_UTMP_H) || defined (HAVE_UTMPX_H) || defined (WINNT_NATIVE)
extern	void		  initwatch	(void);
extern	void		  resetwatch	(void);
extern	void		  watch_login	(int);
extern	const char 	 *who_info	(ptr_t, int, char *, size_t);
extern	void		  dolog		(Char **, struct command *);
# ifdef HAVE_STRUCT_UTMP_UT_HOST
extern	char		 *utmphost	(void);
extern	size_t		  utmphostsize	(void);
# endif /* HAVE_STRUCT_UTMP_UT_HOST */
#else
# define HAVENOUTMP
#endif

#endif /* _h_tc_decls */

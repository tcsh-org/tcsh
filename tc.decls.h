/* $Header: /u/christos/src/tcsh-6.02/RCS/tc.decls.h,v 3.17 1992/07/06 15:26:18 christos Exp $ */
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
#ifndef _h_tc_decls
#define _h_tc_decls

/*
 * tc.alloc.c
 */
#ifndef SYSMALLOC
extern	void		  free		__P((ptr_t));
extern	memalign_t	  malloc	__P((size_t));
extern	memalign_t	  realloc	__P((ptr_t, size_t));
extern	memalign_t	  calloc	__P((size_t, size_t));

#else /* SYSMALLOC */
extern	void		  Free		__P((ptr_t));
extern	memalign_t	  Malloc	__P((size_t));
extern	memalign_t	  Realloc	__P((ptr_t, size_t));
extern	memalign_t	  Calloc	__P((size_t, size_t));
#endif /* SYSMALLOC */
extern	void		  showall	__P((Char **, struct command *));

/*
 * tc.bind.c
 */
extern	void		  dobindkey	__P((Char **, struct command *));
extern	int		  parseescape	__P((Char **));
extern	unsigned char    *unparsestring	__P((Char *, unsigned char *, Char *));
extern	void		  dobind	__P((Char **, struct command *));


/*
 * tc.disc.c
 */
extern	int		  setdisc	__P((int));
extern	int		  resetdisc	__P((int));

/*
 * tc.func.c
 */
extern	Char		 *expand_lex	__P((Char *, int, struct wordent *, 
					     int, int));
extern	Char		 *sprlex	__P((Char *, struct wordent *));
extern	void		  Itoa		__P((int, Char *));
extern	void		  dolist	__P((Char **, struct command *));
extern	void		  dotelltc	__P((Char **, struct command *));
extern	void		  doechotc	__P((Char **, struct command *));
extern	void		  dosettc	__P((Char **, struct command *));
extern	void		  dowhich	__P((Char **, struct command *));
extern	struct process	 *find_stop_ed	__P((void));
extern	void		  fg_proc_entry	__P((struct process *));
extern	sigret_t	  alrmcatch	__P((int));
extern	void		  precmd	__P((void));
extern	void		  cwd_cmd	__P((void));
extern	void		  beep_cmd	__P((void));
extern	void		  period_cmd	__P((void));
extern	void		  aliasrun	__P((int, Char *, Char *));
extern	void		  setalarm	__P((int));
extern	void		  rmstar	__P((struct wordent *));
extern	void		  continue_jobs	__P((struct wordent *));
extern	Char		 *gettilde	__P((Char *));
extern	Char		 *getusername	__P((Char **));
extern	void		  doaliases	__P((Char **, struct command *));
extern	void		  shlvl		__P((int));
extern	int		  fixio		__P((int, int));
extern	int		  collate	__P((const Char *, const Char *));
#ifdef HASHBANG
extern	int		  hashbang	__P((int, Char ***));
#endif /* HASHBANG */


/*
 * tc.os.c
 */
#ifdef MACH
extern	void		  dosetpath	__P((Char **, struct command *));
#endif /* MACH */

#ifdef TCF
extern	void		  dogetxvers	__P((Char **, struct command *));
extern	void		  dosetxvers	__P((Char **, struct command *));
extern	void		  dogetspath	__P((Char **, struct command *));
extern	void		  dosetspath	__P((Char **, struct command *));
extern	char		 *sitename	__P((pid_t));
extern	void		  domigrate	__P((Char **, struct command *));
#endif /* TCF */

#ifdef WARP
extern	void 		  dowarp	__P((Char **, struct command *));
#endif /* WARP */

#ifdef masscomp
extern	void		  douniverse	__P((Char **, struct command *));
#endif /* masscomp */

#ifdef _SEQUENT_
extern	void	 	  pr_stat_sub	__P((struct process_stats *, 
					     struct process_stats *, 
					     struct process_stats *));
#endif /* _SEQUENT_ */

#ifdef NEEDtcgetpgrp
extern	int	 	  xtcgetpgrp	__P((int));
extern	int		  xtcsetpgrp	__P((int, int));
# undef tcgetpgrp
# define tcgetpgrp(a) 	  xtcgetpgrp(a)
# undef tcsetpgrp
# define tcsetpgrp(a, b)  xtcsetpgrp((a), (b))
#endif /* NEEDtcgetpgrp */

#ifdef YPBUGS
extern	void	 	  fix_yp_bugs	__P((void));
#endif /* YPBUGS */
#ifdef STRCOLLBUG
extern	void	 	  fix_strcoll_bug	__P((void));
#endif /* STRCOLLBUG */

extern	void	 	  osinit	__P((void));

#ifdef NEEDmemmove
extern void 		 *xmemmove	__P((ptr_t, const ptr_t, size_t));
# define memmove(a, b, c) xmemmove((a), (b), (c))
#endif /* NEEDmemmove */


#ifdef NEEDgetwd
extern	char		 *xgetwd	__P((char *));
# undef getwd
# define getwd(a) xgetwd(a)
#endif /* NEEDgetwd */

#ifdef NEEDgethostname
extern	int	 	  xgethostname	__P((char *, int));
# undef gethostname
# define gethostname(a, b) xgethostname((a), (b))
#endif /* NEEDgethostname */

#ifdef NEEDnice
extern	int	 	  xnice	__P((int));
# undef nice
# define nice(a)	  xnice(a)
#endif /* NEEDnice */

#ifdef NEEDstrerror
extern	char	 	 *xstrerror	__P((int));
# undef strerror
# define strerror(a) 	  xstrerror(a)
#endif /* NEEDstrerror */

#ifdef apollo
extern	void		  doinlib	__P((Char **, struct command *));
extern	void		  dover		__P((Char **, struct command *));
extern	void		  dorootnode	__P((Char **, struct command *));
extern	int		  getv		__P((Char *));
#endif /* apollo */


/*
 * tc.printf.h
 */
extern	void		  xprintf	__P((char *, ...));
extern	void		  xsprintf	__P((char *, char *, ...));
extern	void		  xvprintf	__P((char *, va_list));
extern	void		  xvsprintf	__P((char *, char *, va_list));

/*
 * tc.prompt.c
 */
extern	void		  printprompt	__P((int, char *));
extern	void		  tprintf	__P((int, Char *, Char *, size_t, 
					     char *, time_t, ptr_t));

/*
 * tc.sched.c
 */
extern	time_t		  sched_next	__P((void));
extern	void		  dosched	__P((Char **, struct command *));
extern	void		  sched_run	__P((void));

/*
 * tc.sig.c
 */
#ifndef BSDSIGS
# if SYSVREL < 3 || defined(UNIXPC)
extern	sigret_t	(*sigset	__P((int, sigret_t (*)(int)))) ();
extern	void		  sigrelse	__P((int));
extern	void		  sighold	__P((int));
extern	void		  sigignore	__P((int));
extern	void 		  sigpause	__P((int));
# endif
# ifdef SXA
extern	void 		  sigpause	__P((int));
# endif
extern	pid_t 		  ourwait	__P((int *));
#endif
#ifdef NEEDsignal
extern	sigret_t	(*xsignal	__P((int, sigret_t (*)(int)))) ();
#define signal(a, b)	  xsignal(a, b)
#endif
#if defined(_SEQUENT_) || defined(linux)
extern	sigmask_t	  sigsetmask	__P((sigmask_t));
extern	sigmask_t	  sigblock	__P((sigmask_t));
extern	void		  bsd_sigpause	__P((sigmask_t));
extern  sigret_t        (*bsd_signal    __P((int, sigret_t (*)(int)))) ();
#endif /* _SEQUENT_ || linux */
#ifdef SIGSYNCH
extern	sigret_t	  synch_handler	__P((int));
#endif


/*
 * tc.str.c:
 */
#ifdef SHORT_STRINGS
extern	Char		 *s_strchr	__P((const Char *, int));
extern	Char		 *s_strrchr	__P((const Char *, int));
extern	Char		 *s_strcat	__P((Char *, const Char *));
#ifdef NOTUSED
extern	Char		 *s_strncat	__P((Char *, const Char *, size_t));
#endif
extern	Char		 *s_strcpy	__P((Char *, const Char *));
extern	Char		 *s_strncpy	__P((Char *, const Char *, size_t));
extern	Char		 *s_strspl	__P((const Char *, const Char *));
extern	size_t		  s_strlen	__P((const Char *));
extern	int		  s_strcmp	__P((const Char *, const Char *));
extern	int		  s_strncmp	__P((const Char *, const Char *, 
					     size_t));
extern	Char		 *s_strsave	__P((const Char *));
extern	Char		 *s_strend	__P((const Char *));
extern	Char		 *s_strstr	__P((const Char *, const Char *));
extern	Char		 *str2short	__P((const char *));
extern	Char		**blk2short	__P((char **));
extern	char		 *short2str	__P((const Char *));
extern	char		**short2blk	__P((Char **));
#endif
extern	char		 *short2qstr	__P((const Char *));


/*
 * tc.vers.h:
 */
extern	void		  fix_version	__P((void));
extern	char		 *gethosttype	__P((void));

/*
 * tc.who.c
 */
extern	void		  initwatch	__P((void));
extern	void		  resetwatch	__P((void));
extern	void		  watch_login	__P((void));
extern	char 		 *who_info	__P((ptr_t, int, char *));
extern	void		  dolog		__P((Char **, struct command *));

#endif				/* _h_tc_decls */

/* $Header: /src/pub/tcsh/sh.decls.h,v 3.47 2005/04/11 22:10:56 kim Exp $ */
/*
 * sh.decls.h	 External declarations from sh*.c
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
#ifndef _h_sh_decls
#define _h_sh_decls

/*
 * sh.c
 */
extern	int	 	  gethdir	(Char *);
extern	void		  dosource	(Char **, struct command *);
extern	void		  exitstat	(void);
extern	void		  goodbye	(Char **, struct command *);
extern	void		  importpath	(Char *);
extern	void		  initdesc	(void);
extern	RETSIGTYPE	  pintr		(int);
extern	void		  pintr1	(int);
extern	void		  process	(int);
extern	void		  untty		(void);
#ifdef PROF
extern	void		  done		(int);
#else
extern	void		  xexit		(int);
#endif

/*
 * sh.dir.c
 */
extern	void		  dinit		(Char *);
extern	void		  dodirs	(Char **, struct command *);
extern	Char		 *dcanon	(Char *, Char *);
extern	void		  dtildepr	(Char *);
extern	void		  dtilde	(void);
extern	void		  dochngd	(Char **, struct command *);
extern	Char		 *dnormalize	(Char *, int);
extern	void		  dopushd	(Char **, struct command *);
extern	void		  dopopd	(Char **, struct command *);
extern	void		  dfree		(struct directory *);
extern	void		  dsetstack	(void);
extern	int		  getstakd	(Char *, int);
extern	void		  recdirs	(Char *, int);
extern	void		  loaddirs	(Char *);

/*
 * sh.dol.c
 */
extern	void		  Dfix		(struct command *);
extern	Char		 *Dfix1		(Char *);
extern	void		  heredoc	(Char *);

/*
 * sh.err.c
 */
extern	void		  errinit	(void);
extern	void		  seterror	(unsigned int, ...);
extern	void		  stderror	(unsigned int, ...);

/*
 * sh.exec.c
 */
extern	void		  doexec	(struct command *, int);
extern	void		  dohash	(Char **, struct command *);
extern	void		  dounhash	(Char **, struct command *);
extern	void		  execash	(Char **, struct command *);
extern	void		  hashstat	(Char **, struct command *);
extern	void		  xechoit	(Char **);
extern	int		  executable	(Char *, Char *, int);
extern	int		  tellmewhat	(struct wordent *, Char *);
extern	void		  dowhere	(Char **, struct command *);
extern	int		  find_cmd	(Char *, int);

/*
 * sh.exp.c
 */
extern  Char     *filetest       (Char *, Char ***, int);
extern	int	 	  expr		(Char ***);
extern	int		  exp0		(Char ***, int);

/*
 * sh.file.c
 */
#if defined(FILEC) && defined(TIOCSTI)
extern	int		  tenex		(Char *, int);
#endif

/*
 * sh.func.c
 */
extern	void		  tsetenv	(const Char *, const Char *);
extern	void		  Unsetenv	(Char *);
extern	void		  doalias	(Char **, struct command *);
extern	void		  dobreak	(Char **, struct command *);
extern	void		  docontin	(Char **, struct command *);
extern	void		  doecho	(Char **, struct command *);
extern	void		  doelse	(Char **, struct command *);
extern	void		  doend		(Char **, struct command *);
extern	void		  doeval	(Char **, struct command *);
extern	void		  doexit	(Char **, struct command *);
extern	void		  doforeach	(Char **, struct command *);
extern	void		  doglob	(Char **, struct command *);
extern	void		  dogoto	(Char **, struct command *);
extern	void		  doif		(Char **, struct command *);
extern	void		  dolimit	(Char **, struct command *);
extern	void		  dologin	(Char **, struct command *);
extern	void		  dologout	(Char **, struct command *);
#ifdef NEWGRP
extern	void		  donewgrp	(Char **, struct command *);
#endif
extern	void		  donohup	(Char **, struct command *);
extern	void		  dohup		(Char **, struct command *);
extern	void		  doonintr	(Char **, struct command *);
extern	void		  doprintenv	(Char **, struct command *);
extern	void		  dorepeat	(Char **, struct command *);
extern	void		  dofiletest	(Char **, struct command *);
extern	void		  dosetenv	(Char **, struct command *);
extern	void		  dosuspend	(Char **, struct command *);
extern	void		  doswbrk	(Char **, struct command *);
extern	void		  doswitch	(Char **, struct command *);
extern	void		  doumask	(Char **, struct command *);
extern	void		  dounlimit	(Char **, struct command *);
extern	void		  dounsetenv	(Char **, struct command *);
extern	void		  dowhile	(Char **, struct command *);
extern	void		  dozip		(Char **, struct command *);
extern	void		  func		(struct command *, struct biltins *);
extern	void		  gotolab	(Char *);
extern struct biltins 	 *isbfunc	(struct command *);
extern	void		  prvars	(void);
extern	int		  srchx		(Char *);
extern	void		  unalias	(Char **, struct command *);
extern	void		  wfree		(void);
extern	void		  dobuiltins	(Char **, struct command *);
extern	void		  reexecute	(struct command *);

/*
 * sh.glob.c
 */
extern	Char	 	 *globequal	(Char *, Char *);
extern	Char		**dobackp	(Char *, int);
extern	void		  Gcat		(Char *, Char *);
extern	Char		 *globone	(Char *, int);
extern	int		  Gmatch	(Char *, Char *);
extern	int		  Gnmatch	(Char *, Char *, Char **);
extern	void		  ginit		(void);
extern	Char		**globall	(Char **);
extern	void		  rscan		(Char **, void (*)(Char));
extern	void		  tglob		(Char **);
extern	void		  trim		(Char **);
#if defined(FILEC) && defined(TIOCSTI)
extern	int		  sortscmp	(Char **, Char **);
#endif

#ifndef WINNT_NATIVE
#if defined(NLS_CATALOGS) && defined(HAVE_ICONV) && defined(HAVE_NL_LANGINFO)
extern	char		 *iconv_catgets	(nl_catd, int, int, const char *);
#endif
#endif
extern	void		  nlsinit	(void);
extern	void	          nlsclose	(void);
extern  int	  	  t_pmatch	(Char *, Char *, Char **, int);

/*
 * sh.hist.c
 */
extern	void	 	  dohist	(Char **, struct command *);
extern  struct Hist 	 *enthist	(int, struct wordent *, int, int);
extern	void	 	  savehist	(struct wordent *, int);
extern	void		  fmthist	(int, ptr_t, char *, size_t);
extern	void		  rechist	(Char *, int);
extern	void		  loadhist	(Char *, int);

/*
 * sh.init.c
 */
extern	void		  mesginit	(void);

/*
 * sh.lex.c
 */
extern	void		  addla		(Char *);
extern	void		  bseek		(struct Ain *);
extern	void		  btell		(struct Ain *);
extern	void		  btoeof	(void);
extern	void		  copylex	(struct wordent *, struct wordent *);
extern	Char		 *domod		(Char *, Char);
extern	void		  freelex	(struct wordent *);
extern	int		  lex		(struct wordent *);
extern	void		  prlex		(struct wordent *);
extern	eChar		  readc		(int);
extern	void		  settell	(void);
extern	void		  unreadc	(Char);


/*
 * sh.misc.c
 */
extern	int		  any		(const char *, Char);
extern	Char		**blkcpy	(Char **, Char **);
extern	void		  blkfree	(Char **);
extern	int		  blklen	(Char **);
extern	void		  blkpr		(Char **);
extern	void		  blkexpand	(Char **, Char *);
extern	Char		**blkspl	(Char **, Char **);
extern	void		  closem	(void);
#ifndef CLOSE_ON_EXEC
extern  void 		  closech	(void);
#endif /* !CLOSE_ON_EXEC */
extern	Char		**copyblk	(Char **);
extern	int		  dcopy		(int, int);
extern	int		  dmove		(int, int);
extern	void		  donefds	(void);
extern	Char		  lastchr	(Char *);
extern	void		  lshift	(Char **, int);
extern	int		  number	(Char *);
extern	int		  prefix	(const Char *, const Char *);
extern	Char		**saveblk	(Char **);
extern	void		  setzero	(char *, int);
extern	Char		 *strip		(Char *);
extern	Char		 *quote		(Char *);
extern	Char		 *quote_meta	(Char *, const Char *);
extern	char		 *strsave	(const char *);
extern	void		  udvar		(Char *);
#ifndef POSIX
extern  char   	  	 *strstr	(const char *, const char *);
#endif /* !POSIX */
#ifndef SHORT_STRINGS
extern	char		 *strspl	(const char *, const char *);
extern	char		 *strend	(char *);
#endif /* SHORT_STRINGS */

/*
 * sh.parse.c
 */
extern	void		  alias		(struct wordent *);
extern	void		  freesyn	(struct command *);
extern struct command 	 *syntax	(struct wordent *, 
					 struct wordent *, int);

/*
 * sh.print.c
 */
extern	void		  drainoline	(void);
extern	void		  flush		(void);
#ifdef BSDTIMES
extern	void		  pcsecs	(unsigned long);
#else /* !BSDTIMES */
# ifdef POSIX
extern	void		  pcsecs	(clock_t);
# else /* !POSIX */
extern	void		  pcsecs	(time_t);
# endif /* !POSIX */
#endif /* BSDTIMES */
#ifdef BSDLIMIT
extern	void		  psecs		(unsigned long);
#endif /* BSDLIMIT */
extern	int		  putpure	(int);
extern	int		  putraw	(int);
extern	void		  xputchar	(int);
#ifdef WIDE_STRINGS
extern	void		  putwraw	(Char);
extern	void		  xputwchar	(Char);
#else
# define putwraw(C) putraw(C)
# define xputwchar(C) xputchar(C)
#endif


/*
 * sh.proc.c
 */
extern	void		  dobg		(Char **, struct command *);
extern	void		  dobg1		(Char **, struct command *);
extern	void		  dofg		(Char **, struct command *);
extern	void		  dofg1		(Char **, struct command *);
extern	void		  dojobs	(Char **, struct command *);
extern	void		  dokill	(Char **, struct command *);
extern	void		  donotify	(Char **, struct command *);
extern	void		  dostop	(Char **, struct command *);
extern	void		  dowait	(Char **, struct command *);
extern	void		  palloc	(int, struct command *);
extern	void		  panystop	(int);
extern	RETSIGTYPE	  pchild	(int);
extern	void		  pendjob	(void);
extern	int		  pfork		(struct command *, int);
extern	void		  pgetty	(int, int);
extern	void		  pjwait	(struct process *);
extern	void		  pnote		(void);
extern	void		  prestjob	(void);
extern	void		  psavejob	(void);
extern	int		  pstart	(struct process *, int);
extern	void		  pwait		(void);
extern  struct process   *pfind		(Char *);

/*
 * sh.sem.c
 */
extern	void		  execute	(struct command *, int, int *,
					 int *, int);
extern	void		  mypipe	(int *);

/*
 * sh.set.c
 */
extern	struct varent 	 *adrof1	(Char *, struct varent *);
extern	void		  doset		(Char **, struct command *);
extern	void		  dolet		(Char **, struct command *);
extern	Char		 *putn		(int);
extern	int		  getn		(Char *);
extern	Char		 *value1	(Char *, struct varent *);
extern	void		  set		(Char *, Char *, int);
extern	void		  set1		(Char *, Char **, struct varent *,
					 int);
extern	void		  setq		(Char *, Char **, struct varent *,
					 int);
extern	void		  unset		(Char **, struct command *);
extern	void		  unset1	(Char *[], struct varent *);
extern	void		  unsetv	(Char *);
extern	void		  setNS		(Char *);
extern	void		  shift		(Char **, struct command *);
extern	void		  plist		(struct varent *, int);
extern	Char		 *unparse	(struct command *);
#if defined(DSPMBYTE)
extern	void 		  update_dspmbyte_vars	(void);
extern	void		  autoset_dspmbyte	(Char *);
#endif

/*
 * sh.time.c
 */
extern	void		  donice	(Char **, struct command *);
extern	void		  dotime	(Char **, struct command *);
#ifdef BSDTIMES
extern	void		  prusage	(struct sysrusage *,
					 struct sysrusage *, 
					 timeval_t *, timeval_t *);
extern	void		  ruadd		(struct sysrusage *,
					 struct sysrusage *);
#else /* BSDTIMES */
# ifdef _SEQUENT_
extern	void		  prusage	(struct process_stats *,
					 struct process_stats *, 
					 timeval_t *, timeval_t *);
extern	void		  ruadd		(struct process_stats *,
					 struct process_stats *);
# else /* !_SEQUENT_ */
#  ifdef POSIX
extern	void		  prusage	(struct tms *, struct tms *, 
					 clock_t, clock_t);
#  else	/* !POSIX */
extern	void		  prusage	(struct tms *, struct tms *, 
					 time_t, time_t);
#  endif /* !POSIX */
# endif	/* !_SEQUENT_ */
#endif /* BSDTIMES */
extern	void		  settimes	(void);
#if defined(BSDTIMES) || defined(_SEQUENT_)
extern	void		  tvsub		(struct timeval *, 
					 struct timeval *, 
					 struct timeval *);
#endif /* BSDTIMES || _SEQUENT_ */

#endif /* _h_sh_decls */

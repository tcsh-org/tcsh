/* $Header$ */
/*
 * tc.decls.h: Function declarations from all the tcsh modules
 */
#ifndef _h_tc_decls
#define _h_tc_decls

/*
 * tc.alloc.c
 */
extern void free();
extern memalign_t malloc();
extern memalign_t realloc();
extern void showall();

/*
 * tc.bind.c
 */
extern void dobindkey();
extern int parseescape();
extern unsigned char *unparsestring();
extern void dobind();


/*
 * tc.disc.c
 */
extern int setdisc();
extern int resetdisc();

/*
 * tc.func.c
 */
extern Char *expand_lex();
extern Char *sprlex();
extern void Itoa();
extern void get_tw_comm_list();
extern void dolist();
extern void dotelltc();
extern void doechotc();
extern void dosettc();
extern void dowhich();
extern struct process *find_stopped_editor();
extern void fg_a_proc_entry();
extern sigret_t alrmcatch();
extern void precmd();
extern void cwd_cmd();
extern void period_cmd();
extern void aliasrun();
extern void setalarm();
extern void rmstar();
extern void pgetty();
extern void continue_jobs();


/*
 * tc.os.c
 */
#ifdef MACH
extern void dosetpath();
#endif
#ifdef TCF
extern void dogetxvers();
extern void dosetxvers();
extern void dogetspath();
extern void dosetspath();
extern char *sitename();
extern void domigrate();
#endif
#ifdef WARP
extern void dowarp();
#endif
#ifdef masscomp
extern void douniverse();
#endif
#ifdef _SEQUENT_
extern void subtract_process_stats();
#endif
#ifdef tcgetpgrp
extern int xtcgetpgrp();
#endif
#ifdef YPBUGS
extern void fix_yp_bugs();
#endif
extern void osinit();
#ifdef getwd
extern char *xgetwd();
#endif
#ifdef iconuxv
extern int vfork();
#endif

/*
 * tc.printf.h
 */
extern void CSHprintf();
extern void CSHsprintf();

/*
 * tc.prompt.c
 */
extern void printprompt();

/*
 * tc.sched.c
 */
extern time_t sched_next();
extern void dosched();
extern void sched_run();

/*
 * tc.sig.c
 */
#ifndef BSDSIGS
# if SVID < 3 
#  ifdef UNIXPC
extern sigret_t (*sigset())();
extern void sigrelse();
extern void sighold();
extern void sigignore();
extern void sigpause();
#  endif /* UNIXPC */
extern int ourwait();
# endif
# ifdef SXA
extern void sigpause();
# endif
#endif
#if defined(hpux) || defined(aiws)
extern sigret_t (*signal())();
#endif
#ifdef _SEQUENT_
extern sigmask_t sigsetmask();
extern sigmask_t sigblock();
extern void bsd_sigpause();
#endif

#ifdef SIGSYNCH
extern sigret_t synch_handler();
#endif

#ifdef SAVESIGVEC
extern sigmask_t savesigvec();
extern void restoresigvec();
#endif

/*
 * tc.str.c:
 */
#ifdef SHORT_STRINGS
Char	*s_strchr();
Char	*s_strrchr();
Char	*s_strcat();
#ifdef notdef
Char	*s_strncat();
#endif
Char	*s_strcpy();
Char	*s_strncpy();
Char	*s_strspl();
int	 s_strlen();
int	 s_strcmp();
int	 s_strncmp();
Char	*s_strsave();
Char	*s_strend();
Char	*s_strspl();
Char	*s_strstr();
Char	*str2short();
Char   **blk2short();
char    *short2str();
char   **short2blk();
#endif 


/*
 * tc.vers.h:
 */
extern void fix_version();
extern Char *gethosttype();

/*
 * tc.who.c
 */
extern void initwatch();
extern void resetwatch();
extern void watch_login();
extern void dolog();

#endif /* _h_tc_decls */

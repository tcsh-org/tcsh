/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.decls.h,v 2.0 1991/03/26 02:59:29 christos Exp christos $ */
/*
 * sh.decls.h: Function 
 */
#ifndef _h_sh_decls
#define _h_sh_decls

/*
 * sh.c
 */
extern void rechist();
extern void goodbye();
extern void exitstat();
extern sigret_t pintr();
extern void pintr1();
extern void process();
extern void dosource();
extern int  gethdir();
extern void importpath();
extern void initdesc();
extern void untty();
#ifndef PROF
extern void xexit();
#else
extern void done();
#endif

/*
 * sh.dir.c
 */
extern void dinit();
extern void dodirs();
extern Char *dcanon();
extern void dtildepr();
extern void dtilde();
extern void dochngd();
extern void dopushd();
extern void dopopd();
extern void dfree();
extern void getstakd();
extern void dextract();
#ifdef CSHDIRS
extern void recdirs();
#endif

/*
 * sh.dol.c
 */
extern void Dfix();
extern Char * Dfix1();
extern void heredoc();

/*
 * sh.err.c
 */
extern void error();
extern void bferr();
extern void seterr();
extern void Perror();
extern void stdbferr();
extern void stderror();

/*
 * sh.exec.c
 */
extern void doexec();
extern void execash();
extern void xechoit();
extern void dohash();
extern void dounhash();
#ifdef VFORK
extern void hashstat();
#endif
extern int iscommand();
extern int executable();
extern void tellmewhat();

/*
 * sh.exp.c
 */
extern int exp();
extern int exp0();

/*
 * sh.time.c
 */
extern void settimes();

/*
 * sh.file.c
 */
#ifdef FILEC
extern int tenex();
#endif

/*
 * sh.func.c
 */
extern struct biltins * isbfunc();
extern void func();
extern void doonintr();
extern void donohup();
extern void dozip();
extern void prvars();
extern void doalias();
extern void doaliases();
extern void unalias();
extern void dologout();
extern void dologin();
#ifdef NEWGRP
extern void donewgrp();
#endif
extern void doif();
extern void doelse();
extern void dogoto();
extern void doswitch();
extern void dobreak();
extern void doexit();
extern void doforeach();
extern void dowhile();
extern void doend();
extern void docontin();
extern void dorepeat();
extern void doswbrk();
extern int srchx();
extern void search();
extern void wfree();
extern void doecho();
extern void doglob();
extern void dosetenv();
extern void dounsetenv();
extern void Setenv();
extern void doumask();
extern void dolimit();
extern void dounlimit();
extern void dosuspend();
extern void doeval();

/*
 * sh.glob.c
 */
extern Char **globall();
extern void ginit();
extern void trim();
extern int Gmatch();
extern void Gcat();
extern void rscan();
extern void tglob();
extern Char *globone();
extern Char **dobackp();


/*
 * sh.hist.c
 */
extern void savehist();
extern struct Hist *enthist();
extern void dohist();

/*
 * sh.lex.c
 */
extern int lex();
extern void prlex();
extern void copylex();
extern void freelex();
extern void addla();
extern Char * domod();
extern void unreadc();
extern int readc();
extern void bseek();
#ifndef btell
extern off_t btell();
#endif
extern void btoeof();
#ifdef TELL
extern void settell();
#endif

/*
 * sh.misc.c
 */
extern int any();
extern int onlyread();
extern void xfree();
extern char *strsave();
extern memalign_t calloc();
extern memalign_t nomem();
extern Char **blkend();
extern void blkpr();
extern int number();
extern int blklen();
extern Char **blkcpy();
extern Char **blkcat();
extern void blkfree();
extern Char **saveblk();
extern void setzero();
#ifndef POSIX
extern char *strstr();
#endif
extern char *strspl();
extern Char **blkspl();
extern Char lastchr();
extern void closem();
#ifndef FIOCLEX
extern void closech();
#endif
extern void donefds();
extern int dmove();
extern int dcopy();
#ifndef copy
extern void copy();
#endif
extern void lshift();
extern int number();
extern Char **copyblk();
#ifndef SHORT_STRINGS
extern char *strend();
#endif
extern Char *strip();
extern void udvar();
extern int prefix();

/*
 * sh.parse.c
 */
extern void alias();
extern struct command *syntax();
extern void freesyn();

/*
 * sh.print.c
 */
#ifdef RLIMIT_CPU
extern void psecs();
#endif
extern void pcsecs();
extern void CSHputchar();
extern int putraw();
extern int putpure();
extern void draino();
extern void flush();

/*
 * sh.proc.c
 */
extern sigret_t pchild();
extern void pnote();
extern void pwait();
extern void pjwait();
extern void dowait();
extern void palloc();
extern void psavejob();
extern void prestjob();
extern void pendjob();
extern void dojobs();
extern void dofg();
extern void dofg1();
extern void dobg();
extern void dobg1();
extern void dokill();
extern void dostop();
extern void pstart();
extern void panystop();
extern struct process *pfind();
extern void donotify();
extern int pfork();

/*
 * sh.sem.c
 */
extern void execute();
extern void mypipe();

/*
 * sh.set.c
 */
extern void doset();
extern void dolet();
extern Char *putn();
extern int getn();
extern Char *value1();
extern struct varent *adrof1();
extern void set();
extern void set1();
extern void setq();
extern void unset();
extern void unset1();
extern void unsetv();
extern void setNS();
extern void shift();
extern void plist();

/*
 * sh.time.c
 */
extern void settimes();
extern void dotime();
extern void donice();
#if defined(BSDTIMES) || defined(_SEQUENT_)
extern void ruadd();
extern void tvadd();
extern void tvsub();
#endif
extern void prusage();


#endif /* _h_sh_decls */

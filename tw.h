/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/tw.h,v 1.2 1990/11/25 10:13:36 christos Exp $ */
/*
 * tw.h: TwENEX functions headers
 */
#ifndef _h_tw
#define _h_tw

#ifdef BSDSIGS

#define FREE_ITEMS(items,num)\
{\
    sigmask_t omask;\
    omask = sighold (SIGINT);\
    free_items (items,num);\
    items = NULL;\
    (void) sigsetmask(omask);\
}

#define FREE_DIR(fd)\
{\
    sigmask_t omask;\
    omask = sighold (SIGINT);\
    (void) closedir (fd);\
    fd = NULL;\
    (void) sigsetmask(omask);\
}

#else

#define FREE_ITEMS(items,num)\
{\
    sighold (SIGINT);\
    free_items (items,num);\
    items = NULL;\
    sigrelse (SIGINT);\
}

#define FREE_DIR(fd)\
{\
    sighold (SIGINT);\
    (void) closedir (fd);\
    fd = NULL;\
    sigrelse (SIGINT);\
}

#endif

#ifndef TRUE
# define TRUE		1
#endif 
#ifndef FALSE
# define FALSE		0
#endif 
#define ON		1
#define OFF		0
#define FILSIZ		512		/* Max reasonable file name length */
#define ESC		'\033'
#define equal(a, b)	(strcmp(a, b) == 0)

#define is_set(var)	adrof(var)

#define BUILTINS	"/usr/local/lib/builtins/" /* fake builtin bin */
#define SEARCHLIST "HPATH"	/* Env. param for helpfile searchlist */
#define DEFAULTLIST ":/usr/man/cat1:/usr/man/cat8:/usr/man/cat6:/usr/local/man/cat1:/usr/local/man/cat8:/usr/local/man/cat6"    /* if no HPATH */

extern Char  PromptBuf[];

typedef enum {LIST, RECOGNIZE, PRINT_HELP, SPELL, GLOB, GLOB_EXPAND, 
	      VARS_EXPAND} COMMAND;


#define NUMCMDS_START 512	/* was 800 */
#define NUMCMDS_INCR 256
#define ITEMS_START 512
#define ITEMS_INCR 256

#ifndef DONT_EXTERN

extern Char **command_list;  /* the pre-digested list of commands
					 for speed and general usefullness */
extern int numcommands;
extern int have_sorted;

extern Char dirflag[5];		/*  ' nn\0' - dir #s -  . 1 2 ... */


#endif
#endif /* _h_tw */

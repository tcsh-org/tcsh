/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/sh.os.c,v 1.3 1991/01/30 18:14:03 christos Exp $ */
/*
 * tc.os.c: OS Dependent builtin functions
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id: sh.os.c,v 1.3 1991/01/30 18:14:03 christos Exp $";

#endif
#include "sh.h"
#include "sh.proc.h"
#include "sh.dir.h"
#include "ed.h"
#include "ed.defns.h"		/* for the function names */

/***
 *** MACH
 ***/

#ifdef MACH
/* dosetpath -- setpath built-in command
 *
 **********************************************************************
 * HISNODE_ORY
 * 08-May-88  Richard Draves (rpd) at Carnegie-Mellon University
 *	Major changes to remove artificial limits on sizes and numbers
 *	of paths.
 *
 **********************************************************************
 */

#ifdef MACH
static Char STRCPATH[]		= { 'C', 'P', 'A', 'T', 'H', '\0' };
static Char STRLPATH[]		= { 'L', 'P', 'A', 'T', 'H', '\0' };
static Char STRMPATH[]		= { 'M', 'P', 'A', 'T', 'H', '\0' };
static Char STREPATH[]		= { 'E', 'P', 'A', 'T', 'H', '\0' };
#endif /* MACH */
static Char *syspaths[] =
{STRPATH, STRCPATH, STRLPATH, STRMPATH, STREPATH, 0};

/*#ifdef CMUCS */
#define LOCALSYSPATH	"/usr/cs"
/*#endif */

void 
dosetpath(arglist)
Char  **arglist;
{
    extern char *getenv();

    Char  **pathvars,
          **cmdargs;
    Char  **paths;
    char  **spaths,
          **cpaths,
          **cmds;
    char   *tcp;
    unsigned int npaths,
            ncmds;
    int     i,
            sysflag;

    for (i = 1; arglist[i] && (arglist[i][0] != '-'); i++);
    npaths = i - 1;

    cmdargs = &arglist[i];
    for (; arglist[i]; i++);
    ncmds = i - npaths - 1;

    if (npaths) {
	sysflag = 0;
	pathvars = &arglist[1];
    }
    else {
	sysflag = 1;
	npaths = (sizeof syspaths / sizeof *syspaths) - 1;
	pathvars = syspaths;
    }

    /* note that npaths != 0 */

    spaths = (char **) xalloc(npaths * sizeof *spaths);
    setzero((char *) spaths, npaths * sizeof *spaths);
    paths = (Char **) xalloc((npaths + 1) * sizeof *paths);
    setzero((char *) paths, (npaths + 1) * sizeof *paths);
    cpaths = (char **) xalloc((npaths + 1) * sizeof *cpaths);
    setzero((char *) cpaths, (npaths + 1) * sizeof *cpaths);
    cmds = (char **) xalloc((ncmds + 1) * sizeof *cmds);
    setzero((char *) cmds, (ncmds + 1) * sizeof *cmds);
    for (i = 0; i < npaths; i++) {
	char   *val = getenv(short2str(pathvars[i]));

	if (val == (char *) 0)
	    val = "";

	spaths[i] = xalloc((Strlen(pathvars[i]) + strlen(val)
			    + 2) * sizeof **spaths);
	(void) strcpy(spaths[i], short2str(pathvars[i]));
	(void) strcat(spaths[i], "=");
	(void) strcat(spaths[i], val);
	cpaths[i] = spaths[i];
    }

    for (i = 0; i < ncmds; i++) {
	Char   *val = globone(cmdargs[i], G_ERROR);

	if (val == NOSTR)
	    goto abortpath;
	cmds[i] = xalloc(Strlen(val) + 1);
	(void) strcpy(cmds[i], short2str(val));
    }

    if (setpath(cpaths, cmds, LOCALSYSPATH, sysflag, 1) < 0) {
abortpath:
	if (spaths) {
	    for (i = 0; i < npaths; i++)
		if (spaths[i])
		    xfree((ptr_t) spaths[i]);
	    xfree((ptr_t) spaths);
	}
	if (paths) {
	    for (i = 0; i < npaths; i++)
		if (paths[i])
		    xfree((ptr_t) paths[i]);
	    xfree((ptr_t) paths);
	}
	if (cpaths)
	    xfree((ptr_t) cpaths);
	if (cmds) {
	    for (i = 0; i < ncmds; i++)
		if (cmds[i])
		    xfree((ptr_t) cmds[i]);
	    xfree((ptr_t) cmds);
	}

	for (i = 0; i < npaths; i++) {
	    paths[i] = SAVE(cpaths[i]);
	    xfree((ptr_t) cpaths[i]);
	}
	return;
    }

    for (i = 0; i < npaths; i++) {
	Char   *val;

	for (val = paths[i]; val && *val && *val != '='; val++);
	if (val && *val == '=') {
	    *val++ = '\0';
	    setenv(paths[i], val);
	    if (Strcmp(paths[i], STRPATH) == 0) {
		importpath(val);
		if (havhash)
		    dohash();
	    }
	    *--val = '=';
	}
    }
}

#endif

/***
 *** AIX
 ***/
#ifdef TCF
 /*VARARGS*//* ARGSUSED */
void
dogetxvers(v)
Char  **v;
{
    char    xvers[MAXPATHLEN];

    if (getxvers(xvers, MAXPATHLEN) == -1)
	Perror("getxvers");
    CSHprintf("%s\n", xvers);
    flush();
}

void
dosetxvers(v)
Char  **v;
{
    char   *xvers;

    ++v;
    if (!*v || *v[0] == '\0')
	xvers = "";
    else
	xvers = short2str(*v);
    if (setxvers(xvers) == -1)
	Perror("setxvers");
}

#include <sf.h>
#include <sys/x.out.h>

static struct xc_cpu_t {
    short   xc_id;
    char   *xc_name;
}       xcpu[] =
{ { XC_PDP11, "pdp11"   },
  { XC_23, "i370"       },
  { XC_Z8K, "z8000"     },
  { XC_8086, "i86"      },
  { XC_68K, "mc68000"   },
  { XC_Z80, "x80"       },
  { XC_VAX, "vax"       },
  { XC_16032, "ns16032" },
  { XC_286, "i286"      },
  { XC_386, "i386"      },
  { XC_S370, "xa370"    },
  { 0, (char *) 0       } };

/*
 * our local hack table, stolen from x.out.h
 */
static char *
getxcode(xcid)
short   xcid;
{
    int     i;

    for (i = 0; xcpu[i].xc_name != (char *) 0; i++)
	if (xcpu[i].xc_id == xcid)
	    return (xcpu[i].xc_name);
    return ((char *) 0);
}

static short
getxid(xcname)
char   *xcname;
{
    int     i;

    for (i = 0; xcpu[i].xc_name != (char *) 0; i++)
	if (strcmp(xcpu[i].xc_name, xcname) == 0)
	    return (xcpu[i].xc_id);
    return ((short) -1);
}


void
dogetspath(v)
Char  **v;
{
    int     i,
            j;
    sitepath_t p[MAXSITE];
    struct sf *st;
    static char *local = "LOCAL ";

    if ((j = getspath(p, MAXSITE)) == -1)
	Perror("getspath");
    for (i = 0; i < j && (p[i] & SPATH_CPU) != NOSITE; i++) {
	if (p[i] & SPATH_CPU) {
	    if ((p[i] & SPATH_MASK) == NULLSITE)
		CSHprintf(local);
	    else if ((st = sfxcode((short) (p[i] & SPATH_MASK))) !=
		     (struct sf *) 0)
		CSHprintf("%s ", st->sf_ctype);
	    else {
		char   *xc = getxcode(p[i] & SPATH_MASK);

		if (xc != (char *) 0)
		    CSHprintf("%s ", xc);
		else
		    CSHprintf("*cpu %d* ", (int) (p[i] & SPATH_MASK));
		endsf();	/* BUG in the aix code... needs that cause if
				 * sfxcode fails once it fails for ever */
	    }
	}
	else {
	    if (p[i] == NULLSITE)
		CSHprintf(local);
	    else if ((st = sfnum(p[i])) != (struct sf *) 0)
		CSHprintf("%s ", st->sf_sname);
	    else
		CSHprintf("*site %d* ", (int) (p[i] & SPATH_MASK));
	}
    }
    CSHprintf("\n");
    flush();
}

void
dosetspath(v)
Char  **v;
{
    int     i;
    short   j;
    char   *s;
    sitepath_t p[MAXSITE];
    struct sf *st;

    for (i = 0, v++; *v && *v[0] != '\0'; v++, i++) {
	s = short2str(*v);
	if (isdigit(*s))
	    p[i] = atoi(s);
	else if (strcmp(s, "LOCAL") == 0)
	    p[i] = NULLSITE;
	else if ((st = sfctype(s)) != (struct sf *) 0)
	    p[i] = SPATH_CPU | st->sf_ccode;
	else if ((j = getxid(s)) != -1)
	    p[i] = SPATH_CPU | j;
	else if ((st = sfname(s)) != (struct sf *) 0)
	    p[i] = st->sf_id;
	else {
	    setname(s);
	    bferr("Bad cpu/site name");
	}
	if (i == MAXSITE - 1)
	    bferr("Site path too long");
    }
    if (setspath(p, i) == -1)
	Perror("setspath");
}

/* sitename():
 *	Return the site name where the process is running
 */
char   *
sitename(pid)
pid_t   pid;
{
    siteno_t ss;
    struct sf *st;

    if ((ss = site(pid)) == -1 || (st = sfnum(ss)) == (struct sf *) 0)
	return ("unknown");
    else
	return (st->sf_sname);
}

static int
migratepid(pid, new_site)
pid_t   pid;
siteno_t new_site;
{
    struct sf *st;
    int     need_local;

    need_local = (pid == 0) || (pid == getpid());

    if (kill3((pid_t) pid, SIGMIGRATE, new_site) < 0) {
	CSHprintf("%d: ", pid);
	CSHprintf("%s\n", strerror());
	return (-1);
    }

    if (need_local) {
	if ((new_site = site(0)) == -1) {
	    CSHprintf("site: ");
	    CSHprintf("%s\n", strerror());
	    return (-1);
	}
	if ((st = sfnum(new_site)) == (struct sf *) 0) {
	    CSHprintf("%d: Site not found\n", new_site);
	    return (-1);
	}
	if (setlocal(st->sf_local, strlen(st->sf_local)) == -1) {
	    CSHprintf("setlocal: %s: ", st->sf_local);
	    CSHprintf("%s\n", strerror());
	    return (-1);
	}
    }
    return (0);
}

void
domigrate(v)
Char  **v;
{
    struct sf *st;
    char   *s;
    Char   *cp;
    struct process *pp;
    int     pid,
            err1 = 0;
    siteno_t new_site = 0;
    sigmask_t omask;

#ifdef BSDSIGS
    omask = sigmask(SIGCHLD);
    if (setintr)
	omask |= sigmask(SIGINT);
    omask = sigblock(omask) & ~omask;
#else
    if (setintr)
	(void) sighold(SIGINT);
    (void) sighold(SIGCHLD);
#endif

    ++v;
    if (*v[0] == '-') {
	/*
	 * Do the -site.
	 */
	s = short2str(&v[0][1]);
	if ((st = sfname(s)) == (struct sf *) 0) {
	    setname(s);
	    bferr("Site not found");
	}
	new_site = st->sf_id;
	++v;
    }

    if (!*v || *v[0] == '\0') {
	if (migratepid(0, new_site) == -1)
	    err++;
    }
    else {
	gflag = 0, tglob(v);
	if (gflag) {
	    v = glob(v);
	    if (v == 0)
		stdbferr(ERR_NOMATCH);
	}
	else {
	    v = gargv = saveblk(v);
	    trim(v);
	}

	while (v && (cp = *v)) {
	    if (*cp == '%') {
		pp = pfind(cp);
		if (kill3((pid_t) - pp->p_jobid, SIGMIGRATE, new_site) < 0) {
		    CSHprintf("%s: ", short2str(cp));
		    CSHprintf("%s\n", strerror());
		    err1++;
		}
	    }
	    else if (!(isdigit(*cp) || *cp == '-'))
		bferr("Arguments should be jobs or process id's");
	    else {
		pid = atoi(short2str(cp));
		if (migratepid(pid, new_site) == -1)
		    err1++;
	    }
	    v++;
	}
	if (gargv)
	    blkfree(gargv), gargv = 0;
    }

  done:
#ifdef BSDSIGS
    (void) sigsetmask(omask);
#else
    sigrelse(SIGCHLD);
    if (setintr)
	sigrelse(SIGINT);
#endif
    if (err1)
	error((char *) 0);
}

#endif				/* TCF */

/***
 *** CONVEX Warps.
 ***/
#ifdef WARP
/*
 * handle the funky warping of symlinks
 */
#include <warpdb.h>
#include <sys/warp.h>

static jmp_buf sigsys_buf;

static sigret_t
catch_sigsys()
{
    longjmp(sigsys_buf, 1);
}


void
dowarp(v)
Char  **v;
{
    int     warp,
            oldwarp;
    struct warpent *we;
    void    (*old_sigsys_handler) () = 0;
    char   *newwarp;

    if (setjmp(sigsys_buf)) {
	signal(SIGSYS, old_sigsys_handler);
	bferr("You're trapped in a universe you never made");
	return;
    }
    old_sigsys_handler = signal(SIGSYS, catch_sigsys);

    warp = getwarp();

    v++;
    if (*v == 0) {		/* display warp value */
	if (warp < 0)
	    bferr("Getwarp failed");
	we = getwarpbyvalue(warp);
	if (we)
	    printf("%s\n", we->w_name);
	else
	    printf("%d\n", warp);
    }
    else {			/* set warp value */
	oldwarp = warp;
	newwarp = short2str(*v);
	if (isdigit(*v[0]))
	    warp = atoi(newwarp);
	else {
	    we = getwarpbyname(newwarp);
	    if (we)
		warp = we->w_value;
	    else
		warp = -1;
	}
	if ((warp < 0) || (warp >= WARP_MAXLINK))
	    bferr("Invalid warp");
	if ((setwarp(warp) < 0) || (getwarp() != warp)) {
	    (void) setwarp(oldwarp);
	    bferr("Setwarp failed");
	}
    }
    signal(SIGSYS, old_sigsys_handler);
    return;
}

#endif

/***
 *** Masscomp
 ***/
/* Added, DAS DEC-90. */
#ifdef masscomp
void
douniverse(v)
register Char **v;
{
    register Char *cp = v[1];
    char    ubuf[100];

    if (cp == 0) {
	(void) getuniverse(ubuf);
	CSHprintf("%s\n", ubuf);
    }
    else if (*cp == '\0' || setuniverse(short2str(cp)) != 0)
	bferr("Illegal universe");
}

#endif				/* masscomp */


#ifdef _SEQUENT_
/*
 * Compute the difference in process stats.
 */
void
subtract_process_stats(p2, p1, pr)
struct process_stats *p2,
       *p1,
       *pr;
{
    pr->ps_utime.tv_sec = p2->ps_utime.tv_sec - p1->ps_utime.tv_sec;
    pr->ps_utime.tv_usec = p2->ps_utime.tv_usec - p1->ps_utime.tv_usec;
    if (pr->ps_utime.tv_usec < 0) {
	pr->ps_utime.tv_sec -= 1;
	pr->ps_utime.tv_usec += 1000000;
    }
    pr->ps_stime.tv_sec = p2->ps_stime.tv_sec - p1->ps_stime.tv_sec;
    pr->ps_stime.tv_usec = p2->ps_stime.tv_usec - p1->ps_stime.tv_usec;
    if (pr->ps_stime.tv_usec < 0) {
	pr->ps_stime.tv_sec -= 1;
	pr->ps_stime.tv_usec += 1000000;
    }

    pr->ps_maxrss = p2->ps_maxrss - p1->ps_maxrss;
    pr->ps_pagein = p2->ps_pagein - p1->ps_pagein;
    pr->ps_reclaim = p2->ps_reclaim - p1->ps_reclaim;
    pr->ps_zerofill = p2->ps_zerofill - p1->ps_zerofill;
    pr->ps_pffincr = p2->ps_pffincr - p1->ps_pffincr;
    pr->ps_pffdecr = p2->ps_pffdecr - p1->ps_pffdecr;
    pr->ps_swap = p2->ps_swap - p1->ps_swap;
    pr->ps_syscall = p2->ps_syscall - p1->ps_syscall;
    pr->ps_volcsw = p2->ps_volcsw - p1->ps_volcsw;
    pr->ps_involcsw = p2->ps_involcsw - p1->ps_involcsw;
    pr->ps_signal = p2->ps_signal - p1->ps_signal;
    pr->ps_lread = p2->ps_lread - p1->ps_lread;
    pr->ps_lwrite = p2->ps_lwrite - p1->ps_lwrite;
    pr->ps_bread = p2->ps_bread - p1->ps_bread;
    pr->ps_bwrite = p2->ps_bwrite - p1->ps_bwrite;
    pr->ps_phread = p2->ps_phread - p1->ps_phread;
    pr->ps_phwrite = p2->ps_phwrite - p1->ps_phwrite;
}

#endif				/* _SEQUENT_ */


#ifdef tcgetpgrp
int
xtcgetpgrp(fd)
int     fd;
{
    int     pgrp;

    /* ioctl will handle setting errno correctly. */
    if (ioctl(fd, TIOCGPGRP, (ioctl_t) & pgrp) < 0)
	return (-1);
    return (pgrp);
}

#endif				/* tcgetpgrp */


#ifdef YPBUGS
void
fix_yp_bugs()
{
    char   *mydomain;

    /*
     * PWP: The previous version assumed that yp domain was the same as the
     * internet name domain.  This isn't allways true. (Thanks to Mat Landau
     * <mlandau@bbn.com> for the original version of this.)
     */
    if (yp_get_default_domain(&mydomain) == 0) {	/* if we got a name */
	extern void yp_unbind();

	yp_unbind(mydomain);
    }
}

#endif				/* YPBUGS */


void
osinit()
{

#ifdef OREO
    set42sig();
    sigignore(SIGIO);		/* ignore SIGIO */
#endif				/* OREO */

#ifdef aiws
    struct sigstack inst;

    inst.ss_sp = xalloc(4192) + 4192;
    inst.ss_onstack = 0;
    sigstack(&inst, (struct sigstack *) 0);
#endif				/* aiws */

#ifdef hpux
    (void) sigspace(4192);
#endif				/* hpux */
}

#ifdef gethostname
#include <sys/utsname.h>

xgethostname(name, namlen)
char   *name;
int     namlen;
{
    int     i;
    struct utsname uts;

    uname(&uts);

#ifdef DEBUG
    CSHprintf("sysname:  %s\n", uts.sysname);
    CSHprintf("nodename: %s\n", uts.nodename);
    CSHprintf("release:  %s\n", uts.release);
    CSHprintf("version:  %s\n", uts.version);
    CSHprintf("machine:  %s\n", uts.machine);
#endif				/* DEBUG */
    i = strlen(uts.nodename) + 1;
    (void) strncpy(name, uts.nodename, i < namlen ? i : namlen);
}				/* end gethostname */

#endif				/* gethostname */


#ifdef getwd
static char *strrcpy();

/* xgetwd():
 *	Return the pathname of the current directory, or return
 *	an error message in pathname.
 */
char   *
xgetwd(pathname)
char   *pathname;
{
    DIR    *dp;
    struct dirent *d;

    struct stat st_root,
            st_cur,
            st_next,
            st_dot;
    char    pathbuf[MAXPATHLEN],
            nextpathbuf[MAXPATHLEN * 2];
    char   *cur_name_add;
    char   *pathptr,
           *nextpathptr;

    /* find the inode of root */
    if (stat("/", &st_root) == -1) {
	(void) CSHsprintf(pathname, "getwd: Cannot stat \"/\" (%s)", strerror());
	return ((char *) 0);
    }
    pathbuf[MAXPATHLEN - 1] = '\0';
    pathptr = &pathbuf[MAXPATHLEN - 1];
    nextpathbuf[MAXPATHLEN - 1] = '\0';
    cur_name_add = nextpathptr = &nextpathbuf[MAXPATHLEN - 1];

    /* find the inode of the current directory */
    if (lstat("./", &st_cur) == -1) {
	(void) CSHsprintf(pathname, "getwd: Cannot stat \".\" (%s)", strerror());
	return ((char *) 0);
    }
    nextpathptr = strrcpy(nextpathptr, "../");

    /* Descend to root */
    for (;;) {

	/* look if we found root yet */
	if (st_cur.st_ino == st_root.st_ino &&
	    st_cur.st_dev == st_root.st_dev) {
	    if (*pathptr != '/')
		return ("/");
	    (void) strcpy(pathname, pathptr);
	    return (pathname);
	}

	/* open the parent directory */
	if ((dp = opendir(nextpathptr)) == (DIR *) 0) {
	    (void) CSHsprintf(pathname,
			      "getwd: Cannot open directory \"%s\" (%s)",
			      nextpathptr, strerror());
	    return ((char *) 0);
	}

	/* look in the parent for the entry with the same inode */
	for (d = readdir(dp);
	     d != (struct dirent *) 0; d = readdir(dp)) {
	    (void) strcpy(cur_name_add, d->d_name);
	    if (lstat(nextpathptr, &st_next) == -1) {
		(void) CSHsprintf(pathname, "getwd: Cannot stat \"%s\" (%s)",
				  d->d_name, strerror());
		return ((char *) 0);
	    }
	    if (d->d_name[0] == '.' && d->d_name[1] == '\0')
		st_dot = st_next;

	    /* check if we found it yet */
	    if (st_next.st_ino == st_cur.st_ino &&
		st_next.st_dev == st_cur.st_dev) {
		st_cur = st_dot;
		pathptr = strrcpy(pathptr, d->d_name);
		pathptr = strrcpy(pathptr, "/");
		nextpathptr = strrcpy(nextpathptr, "../");
		*cur_name_add = '\0';
		(void) closedir(dp);
		break;
	    }
	}
	if (d == (struct dirent *) 0) {
	    (void) CSHsprintf(pathname, "getwd: Cannot find \".\" in \"..\"");
	    return ((char *) 0);
	}
    }
}				/* end getwd */

/* strrcpy():
 *	Like strcpy, going backwards and returning the new pointer
 */
static char *
strrcpy(ptr, str)
register char *ptr,
       *str;
{
    register int len = strlen(str);

    while (len)
	*--ptr = str[--len];

    return (ptr);
}				/* end strrcpy */

#endif				/* getwd */
#ifdef iconuxv
#include <sys/vendor.h>
#include <sys/bsd_syscall.h>

int
vfork() {
	return sys_local(VEND_ICON_BSD, 66);
}
#endif /* iconuxv */

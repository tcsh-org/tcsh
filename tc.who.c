/* $Header$ */
/*
 * tc.who.c: Watch logins and logouts...
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id$";
#endif				/* !lint */

#include "sh.h"
#include "sh.local.h"

/*
 * kfk 26 Jan 1984 - for login watch functions.
 */
#include <utmp.h>

#ifndef BROKEN_CC
#define UTNAMLEN	sizeof(((struct utmp *) 0)->ut_name)
#define UTLINLEN	sizeof(((struct utmp *) 0)->ut_line)
#ifdef UTHOST
#define UTHOSTLEN	sizeof(((struct utmp *) 0)->ut_host)
#endif				/* UTHOST */
#else
/* give poor cc a little help if it needs it */
struct utmp __ut;

#define UTNAMLEN	sizeof(__ut.ut_name)
#define UTLINLEN	sizeof(__ut.ut_line)
#ifdef UTHOST
#define UTHOSTLEN	sizeof(__ut.ut_host)
#endif				/* UTHOST */
#endif				/* BROKEN_CC */

#ifndef _PATH_UTMP
#ifdef	UTMP_FILE
#define _PATH_UTMP UTMP_FILE
#else
#define _PATH_UTMP "/etc/utmp"
#endif				/* UTMP_FILE */
#endif				/* _PATH_UTMP */

#ifdef WHODEBUG
static void debugwholist();
#endif
static void print_who();

struct who {
    struct who *w_next;
    struct who *w_prev;
    char    w_name[UTNAMLEN + 1];
    char    w_new[UTNAMLEN + 1];
    char    w_tty[UTLINLEN + 1];
#ifdef UTHOST
    char    w_host[UTHOSTLEN + 1];
#endif				/* UTHOST */
    long    w_time;
    int     w_status;
};

static struct who *wholist = (struct who *) 0;
static int watch_period = 0;
static time_t stlast = 0;
extern char *month_list[];


#define ONLINE		01
#define OFFLINE		02
#define CHANGED		04
#define STMASK		07
#define ANNOUNCE	010

/*
 * Karl Kleinpaste, 26 Jan 1984.
 * Initialize the dummy tty list for login watch.
 * This dummy list eliminates boundary conditions
 * when doing pointer-chase searches.
 */
void
initwatch()
{
    register int i;

    wholist = (struct who *) calloc(1, sizeof *wholist);
    wholist->w_next = (struct who *) calloc(1, sizeof *wholist);
    wholist->w_next->w_prev = wholist;
    for (i = 0; i < UTLINLEN; i++) {
	wholist->w_tty[i] = '\01';
	wholist->w_next->w_tty[i] = '~';
    }
    wholist->w_tty[i] = '\0';
    wholist->w_next->w_tty[i] = '\0';

#ifdef WHODEBUG
    debugwholist((struct who *) 0, (struct who *) 0);
#endif				/* WHODEBUG */
}

void
resetwatch()
{
    watch_period = 0;
    stlast = 0;
}

/*
 * Karl Kleinpaste, 26 Jan 1984.
 * Watch /etc/utmp for login/logout changes.
 */
void
watch_login()
{
    int     utmpfd,
            comparison,
            alldone,
            cnt;
    sigmask_t omask;
    struct utmp utmp;
    struct who *wp,
           *wpnew;
    struct varent *v;
    Char  **vp;
    time_t  t,
            interval = MAILINTVL;
    struct stat sta;

    /* stop SIGINT, lest our login list get trashed. */
#ifdef BSDSIGS
    omask = sigblock(sigmask(SIGINT));
#else
    (void) sighold(SIGINT);
#endif

    v = adrof(STRwatch);
    if (v == (struct varent *) 0) {
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	sigrelse(SIGINT);
#endif
	return;			/* no names to watch */
    }
    vp = v->vec;
    cnt = blklen(vp);
    if (cnt % 2) {		/* odd # args: 1st == # minutes. */
	interval = (number(*vp)) ? getn(*vp++) : MAILINTVL;
	cnt--;
    }
    (void) time(&t);
    if (t - watch_period < interval * 60) {
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	sigrelse(SIGINT);
#endif
	return;			/* not long enough yet... */
    }
    watch_period = t;

    /*
     * From: Michael Schroeder <mlschroe@immd4.informatik.uni-erlangen.de>
     * Don't open utmp all the time, stat it first...
     */
    if (stat(_PATH_UTMP, &sta)) {
	CSHprintf("cannot stat %s.  Please \"unset watch\".\n",
		  _PATH_UTMP);
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	sigrelse(SIGINT);
#endif
	return;
    }
    if (stlast == sta.st_mtime) {
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	sigrelse(SIGINT);
#endif
	return;
    }
    stlast = sta.st_mtime;
    if ((utmpfd = open(_PATH_UTMP, 0)) < 0) {
	CSHprintf("%s cannot be opened.  Please \"unset watch\".\n",
		  _PATH_UTMP);
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	sigrelse(SIGINT);
#endif
	return;
    }

    /*
     * xterm clears the entire utmp entry - mark everyone on the status list
     * OFFLINE or we won't notice X "logouts"
     */
    for (wp = wholist; wp != (struct who *) 0; wp = wp->w_next) {
	wp->w_status = OFFLINE;
	wp->w_time = 0;
    }

    /*
     * Read in the utmp file, sort the entries, and update existing entries or
     * add new entries to the status list.
     */
    while (read(utmpfd, (char *) &utmp, sizeof utmp) == sizeof utmp) {
#ifdef DEAD_PROCESS
#ifndef IRIS4D
	if (utmp.ut_type == USER_PROCESS || utmp.ut_type == DEAD_PROCESS) {
#else
	if (utmp.ut_type == USER_PROCESS) {
#endif
#endif				/* DEAD_PROCESS */
	    if (utmp.ut_name[0] == '\0' && utmp.ut_line[0] == '\0')
		continue;	/* completely void entry */
#ifdef DEAD_PROCESS
	    if (utmp.ut_type == DEAD_PROCESS && utmp.ut_line[0] == '\0')
		continue;
#endif				/* DEAD_PROCESS */
	    wp = wholist;
	    while ((comparison = strncmp(wp->w_tty, utmp.ut_line,
					 UTLINLEN)) < 0)
		wp = wp->w_next;/* find that tty! */

	    if (comparison == 0) {	/* found the tty... */
#ifdef DEAD_PROCESS
		if (utmp.ut_type == DEAD_PROCESS) {
		    wp->w_time = utmp.ut_time;
		    wp->w_status = OFFLINE;
		}
		else
#endif				/* DEAD_PROCESS */
		if (utmp.ut_name[0] == '\0') {
		    wp->w_time = utmp.ut_time;
		    wp->w_status = OFFLINE;
		}
		else
		     /* someone is logged in */ if (strncmp(utmp.ut_name, wp->w_name,
							    UTNAMLEN) == 0) {
		    wp->w_time = utmp.ut_time;
		    wp->w_status = 0;	/* same guy */
		}
		else {
		    (void) strncpy(wp->w_new, utmp.ut_name,
				   UTNAMLEN);
#ifdef UTHOST
		    (void) strncpy(wp->w_host, utmp.ut_host,
				   UTHOSTLEN);
#endif				/* UTHOST */
		    wp->w_time = utmp.ut_time;
		    if (wp->w_name[0] == '\0')
			wp->w_status = ONLINE;
		    else
			wp->w_status = CHANGED;
		}
	    }
	    else {		/* new tty in utmp */
		wpnew = (struct who *) calloc(1, sizeof *wpnew);
		(void) strncpy(wpnew->w_tty, utmp.ut_line, UTLINLEN);
#ifdef UTHOST
		(void) strncpy(wpnew->w_host, utmp.ut_host, UTHOSTLEN);
#endif				/* UTHOST */
		wpnew->w_time = utmp.ut_time;
#ifdef DEAD_PROCESS
		if (utmp.ut_type == DEAD_PROCESS)
		    wpnew->w_status = OFFLINE;
		else
#endif				/* DEAD_PROCESS */
		if (utmp.ut_name[0] == '\0')
		    wpnew->w_status = OFFLINE;
		else {
		    (void) strncpy(wpnew->w_new, utmp.ut_name,
				   UTNAMLEN);
		    wpnew->w_status = ONLINE;
		}
#ifdef WHODEBUG
		debugwholist(wpnew, wp);
#endif				/* WHODEBUG */

		wpnew->w_next = wp;	/* link in a new 'who' */
		wpnew->w_prev = wp->w_prev;
		wpnew->w_prev->w_next = wpnew;
		wp->w_prev = wpnew;	/* linked in now */
	    }
#ifdef DEAD_PROCESS
	}
#endif				/* DEAD_PROCESS */
    }
    (void) close(utmpfd);

    /*
     * The state of all logins is now known, so we can search the user's list
     * of watchables to print the interesting ones.
     */
    for (alldone = 0; !alldone && *vp != (Char *) 0 && **vp != '\0' &&
	 *(vp + 1) != (Char *) 0 && **(vp + 1) != '\0';
	 vp += 2) {		/* args used in pairs... */

	if (eq(*vp, STRany) && eq(*(vp + 1), STRany))
	    alldone = 1;

	for (wp = wholist; wp != (struct who *) 0; wp = wp->w_next) {
	    if (wp->w_status & ANNOUNCE ||
		(!eq(*vp, STRany) &&
		 !eq(*vp, str2short(wp->w_name)) &&
		 !eq(*vp, str2short(wp->w_new))) ||
		(!eq(*(vp + 1), str2short(wp->w_tty)) &&
		 !eq(*(vp + 1), STRany)))
		continue;	/* entry doesn't qualify */
	    /* already printed or not right one to print */

	    if (wp->w_time == 0)/* utmp entry was cleared */
		wp->w_time = watch_period;

	    if ((wp->w_status & OFFLINE) &&
		(wp->w_name[0] != '\0')) {
		print_who(wp);
		wp->w_name[0] = '\0';
		wp->w_status |= ANNOUNCE;
		continue;
	    }
	    if (wp->w_status & ONLINE) {
		print_who(wp);
		(void) strcpy(wp->w_name, wp->w_new);
		wp->w_status |= ANNOUNCE;
		continue;
	    }
	    if (wp->w_status & CHANGED) {
		print_who(wp);
		(void) strcpy(wp->w_name, wp->w_new);
		wp->w_status |= ANNOUNCE;
		continue;
	    }
	}
    }
#ifdef BSDSIGS
    (void) sigsetmask(omask);
#else
    sigrelse(SIGINT);
#endif
}

#ifdef WHODEBUG
static oid
debugwholist(new, wp)
register struct who *new,
       *wp;
{
    register struct who *a;

    a = wholist;
    while (a != (struct who *) 0) {
	CSHprintf("%s/%s -> ", a->w_name, a->w_tty);
	a = a->w_next;
    }
    CSHprintf("NULL\n");
    a = wholist;
    CSHprintf("backward: ");
    while (a->w_next != (struct who *) 0)
	a = a->w_next;
    while (a != (struct who *) 0) {
	CSHprintf("%s/%s -> ", a->w_name, a->w_tty);
	a = a->w_prev;
    }
    CSHprintf("NULL\n");
    if (new)
	CSHprintf("new: %s/%s\n", new->w_name, new->w_tty);
    if (wp)
	CSHprintf("wp: %s/%s\n", wp->w_name, wp->w_tty);
}

#endif				/* WHODEBUG */


static void
print_who(wp)
struct who *wp;
{
#ifdef UTHOST
    char   *cp = "%n has %a %l from %m.";
    char   *ptr,
            flg;

#else
    char   *cp = "%n has %a %l.";

#endif				/* UTHOST */
    struct varent *vp = adrof(STRwho);
    struct tm *t;
    char    ampm = 'a';
    int     attributes = 0;

    t = localtime(&wp->w_time);
    if (vp && vp->vec[0])
	cp = short2str(vp->vec[0]);

    for (; *cp; cp++)
	if (*cp != '%')
	    CSHputchar(*cp | attributes);
	else
	    switch (*++cp) {
	    case 'n':		/* user name */
		switch (wp->w_status & STMASK) {
		case ONLINE:
		case CHANGED:
		    CSHprintf("%a%s", attributes, wp->w_new);
		    break;
		case OFFLINE:
		    CSHprintf("%a%s", attributes, wp->w_name);
		    break;
		}
		break;
	    case 'a':
		switch (wp->w_status & STMASK) {
		case ONLINE:
		    CSHprintf("%a%s", attributes, "logged on");
		    break;
		case OFFLINE:
		    CSHprintf("%a%s", attributes, "logged off");
		    break;
		case CHANGED:
		    CSHprintf("%areplaced %s on", attributes, wp->w_name);
		    break;
		}
		break;
	    case 'S':		/* start standout */
		attributes |= SNODE_ANDOUT;
		break;
	    case 'B':		/* start bold */
		attributes |= BOLD;
		break;
	    case 'U':		/* start underline */
		attributes |= UNDER;
		break;
	    case 's':		/* end standout */
		attributes &= ~SNODE_ANDOUT;
		break;
	    case 'b':		/* end bold */
		attributes &= ~BOLD;
		break;
	    case 'u':		/* end underline */
		attributes &= ~UNDER;
		break;
	    case 't':
	    case 'T':
	    case '@':
		if (adrof(STRampm) || *cp != 'T') {
		    int     hr = t->tm_hour;

		    if (hr >= 12) {
			if (hr > 12)
			    hr -= 12;
			ampm = 'p';
		    }
		    else if (hr == 0)
			hr = 12;
		    CSHprintf("%a%d:%02d%cm", attributes, hr, t->tm_min, ampm);
		}
		else
		    CSHprintf("%a%d:%02d", attributes, t->tm_hour, t->tm_min);
		break;
	    case 'w':
		CSHprintf("%a%s %d", attributes, month_list[t->tm_mon], 
			  t->tm_mday);
		break;
	    case 'W':
		CSHprintf("%a%02d/%02d/%02d", attributes,
			  t->tm_mon + 1, t->tm_mday, t->tm_year);
		break;
	    case 'D':
		CSHprintf("%a%02d-%02d-%02d", attributes,
			  t->tm_year, t->tm_mon + 1, t->tm_mday);
		break;
	    case 'l':
		CSHprintf("%a%s", attributes, wp->w_tty);
		break;
#ifdef UTHOST
	    case 'm':
		if (wp->w_host[0] == '\0')
		    CSHprintf("%alocal", attributes);
		else
		    /* the ':' stuff is for <host>:<display>.<screen> */
		    for (ptr = wp->w_host, flg = isdigit(*ptr) ? '\0' : '.';
			 *ptr != '\0' &&
			 (*ptr != flg || ((ptr = strchr(ptr, ':')) != 0));
			 ptr++) {
			if (*ptr == ':')
			    flg = '\0';
			CSHputchar((int)
				   ((isupper(*ptr) ? tolower(*ptr) : *ptr) |
				    attributes));
		    }
		break;
	    case 'M':
		if (wp->w_host[0] == '\0')
		    CSHprintf("%alocal", attributes);
		else
		    for (ptr = wp->w_host; *ptr != '\0'; ptr++)
			CSHputchar((int)
				   ((isupper(*ptr) ? tolower(*ptr) : *ptr) |
				    attributes));
		break;
#endif				/* UTHOST */
	    default:
		CSHputchar('%' | attributes);
		CSHputchar(*cp | attributes);
		break;
	    }
    CSHputchar('\n');
}				/* end print_who */

void
dolog()
{
    struct who *wp;
    struct varent *v;

    if ((v = adrof(STRwatch)) == (struct varent *) 0)
	error("No $watch variable set");
    blkpr(v->vec);
    CSHprintf("\n");
    watch_period = 0;
    stlast = 0;
    wp = wholist;
    while (wp != (struct who *) 0) {
	wp->w_name[0] = '\0';
	wp = wp->w_next;
    }
}

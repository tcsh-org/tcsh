/* $Header: /u/christos/src/beta-6.01/RCS/tc.who.c,v 3.11 1992/01/06 22:36:56 christos Exp $ */
/*
 * tc.who.c: Watch logins and logouts...
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
#include "sh.h"

RCSID("$Id: tc.who.c,v 3.11 1992/01/06 22:36:56 christos Exp $")

#include "tc.h"

/*
 * kfk 26 Jan 1984 - for login watch functions.
 */
#include <ctype.h>

#ifdef HAVEUTMPX
# include <utmpx.h>
/* I just redefine a few words here.  Changing every occurrence below
 * seems like too much of work.  All UTMP functions have equivalent
 * UTMPX counterparts, so they can be added all here when needed.
 * Kimmo Suominen, Oct 14 1991
 */
# ifndef _PATH_UTMP
#  define _PATH_UTMP UTMPX_FILE
# endif /* _PATH_UTMP */
# define utmp utmpx
# define ut_time ut_xtime
#else /* !HAVEUTMPX */
# include <utmp.h>
#endif /* HAVEUTMPX */

#ifndef BROKEN_CC
# define UTNAMLEN	sizeof(((struct utmp *) 0)->ut_name)
# define UTLINLEN	sizeof(((struct utmp *) 0)->ut_line)
# ifdef UTHOST
#  ifdef _SEQUENT_
#   define UTHOSTLEN	100
#  else
#   define UTHOSTLEN	sizeof(((struct utmp *) 0)->ut_host)
#  endif
# endif	/* UTHOST */
#else
/* give poor cc a little help if it needs it */
struct utmp __ut;

# define UTNAMLEN	sizeof(__ut.ut_name)
# define UTLINLEN	sizeof(__ut.ut_line)
# ifdef UTHOST
#  ifdef _SEQUENT_
#   define UTHOSTLEN	100
#  else
#   define UTHOSTLEN	sizeof(__ut.ut_host)
#  endif
# endif /* UTHOST */
#endif /* BROKEN_CC */

#ifndef _PATH_UTMP
# ifdef	UTMP_FILE
#  define _PATH_UTMP UTMP_FILE
# else
#  define _PATH_UTMP "/etc/utmp"
# endif /* UTMP_FILE */
#endif /* _PATH_UTMP */


struct who {
    struct who *who_next;
    struct who *who_prev;
    char    who_name[UTNAMLEN + 1];
    char    who_new[UTNAMLEN + 1];
    char    who_tty[UTLINLEN + 1];
#ifdef UTHOST
    char    who_host[UTHOSTLEN + 1];
#endif /* UTHOST */
    time_t  who_time;
    int     who_status;
};

static struct who whohead, whotail;
static int watch_period = 0;
static time_t stlast = 0;
extern char *month_list[];
extern char *day_list[];
#ifdef WHODEBUG
static	void	debugwholist	__P((struct who *, struct who *));
#endif
static	void	print_who	__P((struct who *));


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
    whohead.who_next = &whotail;
    whotail.who_prev = &whohead;
#ifdef WHODEBUG
    debugwholist(NULL, NULL);
#endif /* WHODEBUG */
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
    int     utmpfd, comp = -1, alldone;
#ifdef BSDSIGS
    sigmask_t omask;
#endif				/* BSDSIGS */
    struct utmp utmp;
    struct who *wp, *wpnew;
    struct varent *v;
    Char  **vp;
    time_t  t, interval = MAILINTVL;
    struct stat sta;
#if defined(UTHOST) && defined(_SEQUENT_)
    char   *host, *ut_find_host();
#endif

    /* stop SIGINT, lest our login list get trashed. */
#ifdef BSDSIGS
    omask = sigblock(sigmask(SIGINT));
#else
    (void) sighold(SIGINT);
#endif

    v = adrof(STRwatch);
    if (v == NULL) {
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	(void) sigrelse(SIGINT);
#endif
	return;			/* no names to watch */
    }
    trim(vp = v->vec);
    if (blklen(vp) % 2)		/* odd # args: 1st == # minutes. */
	interval = (number(*vp)) ? getn(*vp++) : MAILINTVL;
    (void) time(&t);
    if (t - watch_period < interval * 60) {
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	(void) sigrelse(SIGINT);
#endif
	return;			/* not long enough yet... */
    }
    watch_period = t;

    /*
     * From: Michael Schroeder <mlschroe@immd4.informatik.uni-erlangen.de>
     * Don't open utmp all the time, stat it first...
     */
    if (stat(_PATH_UTMP, &sta)) {
	xprintf("cannot stat %s.  Please \"unset watch\".\n", _PATH_UTMP);
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	(void) sigrelse(SIGINT);
#endif
	return;
    }
    if (stlast == sta.st_mtime) {
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	(void) sigrelse(SIGINT);
#endif
	return;
    }
    stlast = sta.st_mtime;
    if ((utmpfd = open(_PATH_UTMP, O_RDONLY)) < 0) {
	xprintf("%s cannot be opened.  Please \"unset watch\".\n", _PATH_UTMP);
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	(void) sigrelse(SIGINT);
#endif
	return;
    }

    /*
     * xterm clears the entire utmp entry - mark everyone on the status list
     * OFFLINE or we won't notice X "logouts"
     */
    for (wp = whohead.who_next; wp->who_next != NULL; wp = wp->who_next) {
	wp->who_status = OFFLINE;
	wp->who_time = 0;
    }

    /*
     * Read in the utmp file, sort the entries, and update existing entries or
     * add new entries to the status list.
     */
    while (read(utmpfd, (char *) &utmp, sizeof utmp) == sizeof utmp) {

#ifdef DEAD_PROCESS
# ifndef IRIS4D
	if (utmp.ut_type != USER_PROCESS)
	    continue;
# else
	/* Why is that? Cause the utmp file is always corrupted??? */
	if (utmp.ut_type != USER_PROCESS && utmp.ut_type != DEAD_PROCESS)
	    continue;
# endif /* IRIS4D */
#endif /* DEAD_PROCESS */

	if (utmp.ut_name[0] == '\0' && utmp.ut_line[0] == '\0')
	    continue;	/* completely void entry */
#ifdef DEAD_PROCESS
	if (utmp.ut_type == DEAD_PROCESS && utmp.ut_line[0] == '\0')
	    continue;
#endif /* DEAD_PROCESS */
	wp = whohead.who_next;
	while (wp->who_next && (comp = strncmp(wp->who_tty, utmp.ut_line, UTLINLEN)) < 0)
	    wp = wp->who_next;/* find that tty! */

	if (wp->who_next && comp == 0) {	/* found the tty... */
#ifdef DEAD_PROCESS
	    if (utmp.ut_type == DEAD_PROCESS) {
		wp->who_time = utmp.ut_time;
		wp->who_status = OFFLINE;
	    }
	    else
#endif /* DEAD_PROCESS */
	    if (utmp.ut_name[0] == '\0') {
		wp->who_time = utmp.ut_time;
		wp->who_status = OFFLINE;
	    }
	    else if (strncmp(utmp.ut_name, wp->who_name, UTNAMLEN) == 0) {
		/* someone is logged in */ 
		wp->who_time = utmp.ut_time;
		wp->who_status = 0;	/* same guy */
	    }
	    else {
		(void) strncpy(wp->who_new, utmp.ut_name, UTNAMLEN);
#ifdef UTHOST
# ifdef _SEQUENT_
		host = ut_find_host(wp->who_tty);
		if (host)
		    (void) strncpy(wp->who_host, host, UTHOSTLEN);
		else
		    wp->who_host[0] = 0;
# else
		(void) strncpy(wp->who_host, utmp.ut_host, UTHOSTLEN);
# endif
#endif /* UTHOST */
		wp->who_time = utmp.ut_time;
		if (wp->who_name[0] == '\0')
		    wp->who_status = ONLINE;
		else
		    wp->who_status = CHANGED;
	    }
	}
	else {		/* new tty in utmp */
	    wpnew = (struct who *) xcalloc(1, sizeof *wpnew);
	    (void) strncpy(wpnew->who_tty, utmp.ut_line, UTLINLEN);
#ifdef UTHOST
# ifdef _SEQUENT_
	    host = ut_find_host(wpnew->who_tty);
	    if (host)
		(void) strncpy(wpnew->who_host, host, UTHOSTLEN);
	    else
		wpnew->who_host[0] = 0;
# else
	    (void) strncpy(wpnew->who_host, utmp.ut_host, UTHOSTLEN);
# endif
#endif /* UTHOST */
	    wpnew->who_time = utmp.ut_time;
#ifdef DEAD_PROCESS
	    if (utmp.ut_type == DEAD_PROCESS)
		wpnew->who_status = OFFLINE;
	    else
#endif /* DEAD_PROCESS */
	    if (utmp.ut_name[0] == '\0')
		wpnew->who_status = OFFLINE;
	    else {
		(void) strncpy(wpnew->who_new, utmp.ut_name, UTNAMLEN);
		wpnew->who_status = ONLINE;
	    }
#ifdef WHODEBUG
	    debugwholist(wpnew, wp);
#endif /* WHODEBUG */

	    wpnew->who_next = wp;	/* link in a new 'who' */
	    wpnew->who_prev = wp->who_prev;
	    wpnew->who_prev->who_next = wpnew;
	    wp->who_prev = wpnew;	/* linked in now */
	}
    }
    (void) close(utmpfd);
#if defined(UTHOST) && defined(_SEQUENT_)
    endutent();
#endif

    /*
     * The state of all logins is now known, so we can search the user's list
     * of watchables to print the interesting ones.
     */
    for (alldone = 0; !alldone && *vp != NULL && **vp != '\0' &&
	 *(vp + 1) != NULL && **(vp + 1) != '\0';
	 vp += 2) {		/* args used in pairs... */

	if (eq(*vp, STRany) && eq(*(vp + 1), STRany))
	    alldone = 1;

	for (wp = whohead.who_next; wp->who_next != NULL; wp = wp->who_next) {
	    if (wp->who_status & ANNOUNCE ||
		(!eq(STRany, vp[0]) &&
		 !Gmatch(str2short(wp->who_name), vp[0]) &&
		 !Gmatch(str2short(wp->who_new),  vp[0])) ||
		(!Gmatch(str2short(wp->who_tty),  vp[1]) &&
		 !eq(STRany, vp[1])))
		continue;	/* entry doesn't qualify */
	    /* already printed or not right one to print */

	    if (wp->who_time == 0)/* utmp entry was cleared */
		wp->who_time = watch_period;

	    if ((wp->who_status & OFFLINE) &&
		(wp->who_name[0] != '\0')) {
		print_who(wp);
		wp->who_name[0] = '\0';
		wp->who_status |= ANNOUNCE;
		continue;
	    }
	    if (wp->who_status & ONLINE) {
		print_who(wp);
		(void) strcpy(wp->who_name, wp->who_new);
		wp->who_status |= ANNOUNCE;
		continue;
	    }
	    if (wp->who_status & CHANGED) {
		print_who(wp);
		(void) strcpy(wp->who_name, wp->who_new);
		wp->who_status |= ANNOUNCE;
		continue;
	    }
	}
    }
#ifdef BSDSIGS
    (void) sigsetmask(omask);
#else
    (void) sigrelse(SIGINT);
#endif
}

#ifdef WHODEBUG
static void
debugwholist(new, wp)
    register struct who *new, *wp;
{
    register struct who *a;

    a = whohead.who_next;
    while (a->who_next != NULL) {
	xprintf("%s/%s -> ", a->who_name, a->who_tty);
	a = a->who_next;
    }
    xprintf("TAIL\n");
    if (a != &whotail) {
	xprintf("BUG! last element is not whotail!\n");
	abort();
    }
    a = whotail.who_prev;
    xprintf("backward: ");
    while (a->who_prev != NULL) {
	xprintf("%s/%s -> ", a->who_name, a->who_tty);
	a = a->who_prev;
    }
    xprintf("HEAD\n");
    if (a != &whohead) {
	xprintf("BUG! first element is not whohead!\n");
	abort();
    }
    if (new)
	xprintf("new: %s/%s\n", new->who_name, new->who_tty);
    if (wp)
	xprintf("wp: %s/%s\n", wp->who_name, wp->who_tty);
}
#endif /* WHODEBUG */


static void
print_who(wp)
    struct who *wp;
{
#ifdef UTHOST
    char   *cp = "%n has %a %l from %m.";
    char   *ptr, flg;
#else
    char   *cp = "%n has %a %l.";
#endif /* UTHOST */

    struct varent *vp = adrof(STRwho);
    struct tm *t;
    char    ampm = 'a';
    int     attributes = 0;

    t = localtime(&wp->who_time);
    if (vp && vp->vec[0])
	cp = short2str(vp->vec[0]);

    for (; *cp; cp++)
	if (*cp != '%')
	    xputchar(*cp | attributes);
	else
	    switch (*++cp) {
	    case 'n':		/* user name */
		switch (wp->who_status & STMASK) {
		case ONLINE:
		case CHANGED:
		    xprintf("%a%s", attributes, wp->who_new);
		    break;
		case OFFLINE:
		    xprintf("%a%s", attributes, wp->who_name);
		    break;
		default:
		    break;
		}
		break;
	    case 'a':
		switch (wp->who_status & STMASK) {
		case ONLINE:
		    xprintf("%a%s", attributes, "logged on");
		    break;
		case OFFLINE:
		    xprintf("%a%s", attributes, "logged off");
		    break;
		case CHANGED:
		    xprintf("%areplaced %s on", attributes, wp->who_name);
		    break;
		default:
		    break;
		}
		break;
	    case 'S':		/* start standout */
		attributes |= STANDOUT;
		break;
	    case 'B':		/* start bold */
		attributes |= BOLD;
		break;
	    case 'U':		/* start underline */
		attributes |= UNDER;
		break;
	    case 's':		/* end standout */
		attributes &= ~STANDOUT;
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
		    xprintf("%a%d:%02d%cm", attributes, hr, t->tm_min, ampm);
		}
		else
		    xprintf("%a%d:%02d", attributes, t->tm_hour, t->tm_min);
		break;
	    case 'd':
		xprintf("%a%02d", attributes, day_list[t->tm_wday]);
		break;
	    case 'D':
		xprintf("%a%02d", attributes, t->tm_mday);
		break;
	    case 'w':
		xprintf("%a%s", attributes, month_list[t->tm_mon]);
		break;
	    case 'W':
		xprintf("%a%02d", attributes, t->tm_mon + 1);
		break;
	    case 'y':
		xprintf("%a%02d", attributes, t->tm_year);
		break;
	    case 'Y':
		xprintf("%a%04d", attributes, t->tm_year + 1900);
		break;
	    case 'l':
		xprintf("%a%s", attributes, wp->who_tty);
		break;
#ifdef UTHOST
	    case 'm':
		if (wp->who_host[0] == '\0')
		    xprintf("%alocal", attributes);
		else
		    /* the ':' stuff is for <host>:<display>.<screen> */
		    for (ptr = wp->who_host, flg = Isdigit(*ptr) ? '\0' : '.';
			 *ptr != '\0' &&
			 (*ptr != flg || ((ptr = strchr(ptr, ':')) != 0));
			 ptr++) {
			if (*ptr == ':')
			    flg = '\0';
			xputchar((int)
				 ((Isupper(*ptr) ? Tolower(*ptr) : *ptr) |
				  attributes));
		    }
		break;
	    case 'M':
		if (wp->who_host[0] == '\0')
		    xprintf("%alocal", attributes);
		else
		    for (ptr = wp->who_host; *ptr != '\0'; ptr++)
			xputchar((int)
				 ((Isupper(*ptr) ? Tolower(*ptr) : *ptr) |
				  attributes));
		break;
#endif /* UTHOST */
	    default:
		xputchar('%' | attributes);
		xputchar(*cp | attributes);
		break;
	    }
    xputchar('\n');
} /* end print_who */

void
/*ARGSUSED*/
dolog(v, c)
Char **v;
struct command *c;
{
    struct who *wp;
    struct varent *vp;

    if ((vp = adrof(STRwatch)) == NULL)
	stderror(ERR_NOWATCH);
    blkpr(vp->vec);
    xputchar('\n');
    resetwatch();
    wp = whohead.who_next;
    while (wp->who_next != NULL) {
	wp->who_name[0] = '\0';
	wp = wp->who_next;
    }
}

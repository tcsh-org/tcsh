/* $Header$ */
/*
 * tc.sched.c: Scheduled command execution
 *
 * Karl Kleinpaste: Computer Consoles Inc. 1984
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id$";
#endif /* !lint */

#include "sh.h"
#include "sh.local.h"



extern int just_signaled;

struct sched_event {
    struct sched_event *t_next;
    long    t_when;
    Char  **t_lex;
};
static struct sched_event *sched_ptr = (struct sched_event *) 0;


time_t
sched_next()
{
   if (sched_ptr)
	return(sched_ptr->t_when);
   return((time_t) -1);
}

void
dosched(v)
register Char **v;
{
    register struct sched_event *tp,
           *tp1,
           *tp2;
    long    cur_time;
    int     count,
            hours,
            minutes,
            dif_hour,
            dif_min;
    Char   *cp;
    bool    relative;		/* time specified as +hh:mm */
    struct tm *ltp;
    char   *timeline;
    char   *ctime();

    v++;
    cp = *v++;
    if (cp == NOSTR) {
	/* print list of scheduled events */
	for (count = 1, tp = sched_ptr; tp; count++, tp = tp->t_next) {
	    timeline = ctime(&tp->t_when);
	    timeline[16] = '\0';
	    CSHprintf("%6d\t%s\t", count, timeline);
	    blkpr(tp->t_lex);
	    CSHprintf("\n");
	}
	return;
    }

    if (*cp == '-') {
	/* remove item from list */
	if (!sched_ptr)
	    error("No scheduled events");
	if (*v)
	    error("Too many args for 'sched -<item#>'");
	count = atoi(short2str(++cp));
	if (count <= 0)
	    error("Usage to delete: sched -<item#>");
	tp = sched_ptr;
	tp1 = 0;
	while (--count) {
	    if (tp->t_next == 0)
		break;
	    else {
		tp1 = tp;
		tp = tp->t_next;
	    }
	}
	if (count)
	    error("Not that many scheduled events");
	if (tp1 == 0)
	    sched_ptr = tp->t_next;
	else
	    tp1->t_next = tp->t_next;
	blkfree(tp->t_lex);
	xfree((ptr_t) tp);
	return;
    }

    /* else, add an item to the list */
    if (!*v)
	error("No command to run");
    relative = 0;
    if (!isdigit(*cp)) {	/* not abs. time */
	if (*cp != '+')
	    error("Usage: sched [+]hh:mm <command>");
	cp++, relative++;
    }
    minutes = 0;
    hours = atoi(short2str(cp));
    while (*cp && *cp != ':' && *cp != 'a' && *cp != 'p')
	cp++;
    if (*cp && *cp == ':')
	minutes = atoi(short2str(++cp));
    if ((hours < 0) || (minutes < 0) ||
	(hours > 23) || (minutes > 59))
	error("Invalid time for event");
    while (*cp && *cp != 'p' && *cp != 'a')
	cp++;
    if (*cp && relative)
	error("Relative time inconsistent with am/pm");
    if (*cp == 'p')
	hours += 12;
    (void) time(&cur_time);
    ltp = localtime(&cur_time);
    if (relative) {
	dif_hour = hours;
	dif_min = minutes;
    }
    else {
	if ((dif_hour = hours - ltp->tm_hour) < 0)
	    dif_hour += 24;
	if ((dif_min = minutes - ltp->tm_min) < 0) {
	    dif_min += 60;
	    if ((--dif_hour) < 0)
		dif_hour = 23;
	}
    }
    tp = (struct sched_event *) calloc(1, sizeof *tp);
    tp->t_when = cur_time - ltp->tm_sec + dif_hour * 3600L + dif_min * 60L;
    /* use of tm_sec: get to beginning of minute. */
    if (!sched_ptr || tp->t_when < sched_ptr->t_when) {
	tp->t_next = sched_ptr;
	sched_ptr = tp;
    }
    else {
	tp1 = sched_ptr->t_next;
	tp2 = sched_ptr;
	while (tp1 && tp->t_when >= tp1->t_when) {
	    tp2 = tp1;
	    tp1 = tp1->t_next;
	}
	tp->t_next = tp1;
	tp2->t_next = tp;
    }
    tp->t_lex = saveblk(v);
}

/*
 * Execute scheduled events
 */
void
sched_run()
{
    long    cur_time;
    register struct sched_event *tp,
           *tp1;
    struct wordent cmd,
           *nextword,
           *lastword;
    struct command *t = (struct command *) 0;
    Char  **v,
           *cp;
    extern Char GettingInput;

#ifdef BSDSIGS
    sigmask_t omask;

    omask = sigblock(sigmask(SIGINT)) & ~sigmask(SIGINT);
#else
    (void) sighold(SIGINT);
#endif

    (void) time(&cur_time);
    tp = sched_ptr;

    /* bugfix by: Justin Bur at Universite de Montreal */
    /*
     * this test wouldn't be necessary if this routine were not called before
     * each prompt (in sh.c).  But it is, to catch missed alarms.  Someone
     * ought to fix it all up.  -jbb
     */
    if (!(tp && tp->t_when < cur_time)) {
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else
	(void) sigrelse(SIGINT);
#endif
	return;
    }

    if (GettingInput)
	(void) Cookedmode();

    while (tp && tp->t_when < cur_time) {
	err = (char *) 0;
	cmd.word = STRNULL;
	lastword = &cmd;
	v = tp->t_lex;
	for (cp = *v; cp; cp = *++v) {
	    nextword = (struct wordent *) calloc(1, sizeof cmd);
	    nextword->word = Strsave(cp);
	    lastword->next = nextword;
	    nextword->prev = lastword;
	    lastword = nextword;
	}
	lastword->next = &cmd;
	cmd.prev = lastword;
	tp1 = tp;
	sched_ptr = tp = tp1->t_next;	/* looping termination cond: */
	blkfree(tp1->t_lex);	/* straighten out in case of */
	xfree((ptr_t) tp1);	/* command blow-up. */

	/* expand aliases like process() does. */
	alias(&cmd);
	/* build a syntax tree for the command. */
	t = syntax(cmd.next, &cmd, 0);
	if (err)
	    error(err);
	/* execute the parse tree. */
	execute(t, -1);
	/* done. free the lex list and parse tree. */
	freelex(&cmd), freesyn(t);
    }
    if (GettingInput && !just_signaled) {	/* PWP */
	(void) Rawmode();
	ClearLines();		/* do a real refresh since something may */
	ClearDisp();		/* have printed to the screen */
	Refresh();
    }
    just_signaled = 0;

#ifdef BSDSIGS
    (void) sigsetmask(omask);
#else
    (void) sigrelse(SIGINT);
#endif
}

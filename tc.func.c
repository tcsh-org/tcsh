/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/sh.nfunc.c,v 1.15 1991/01/30 18:14:03 christos Exp $ */
/*
 * tc.func.c: New tcsh builtins.
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id: sh.nfunc.c,v 1.15 1991/01/30 18:14:03 christos Exp $";

#endif
#include "sh.h"
#include "sh.proc.h"
#include "sh.dir.h"
#include "ed.h"
#include "ed.defns.h"		/* for the function names */
#define MAKE_TWENEX
#include "tw.h"
#include "sh.char.h"

extern time_t t_period;
static bool	precmd_active = 0;
static bool	periodic_active = 0;
static bool	cwdcmd_active = 0;	/* PWP: for cwd_cmd */

static void Reverse();
static void auto_logout();
static void insert();
static void insert_we();
static int inlist();


/*
 * Tops-C shell
 */

/*
 * expand_lex: Take the given lex and put an expanded version of it in the
 * string buf. First guy in lex list is ignored; last guy is ^J which we
 * ignore Only take lex'es from position from to position to inclusive Note:
 * csh sometimes sets bit 8 in characters which causes all kinds of problems
 * if we don't mask it here. Note: excl's in lexes have been un-back-slashed
 * and must be re-back-slashed
 * (PWP: NOTE: this returns a pointer to the END of the string expanded
 *             (in other words, where the NUL is).)
 */
/* PWP: this is a combination of the old sprlex() and the expand_lex from
   the magic-space stuff */

Char   *
expand_lex(buf, bufsiz, sp0, from, to)
Char   *buf;
int     bufsiz;
struct wordent *sp0;
int     from,
        to;
{
    register struct wordent *sp;
    register Char *s,
           *d,
           *e;
    register Char prev_c;
    register int i;

    buf[0] = '\0';
    prev_c = '\0';
    d = buf;
    e = &buf[bufsiz];		/* for bounds checking */

    if (!sp0)
	return (buf);		/* null lex */
    if ((sp = sp0->next) == sp0)
	return (buf);		/* nada */
    if (sp == (sp0 = sp0->prev))
	return (buf);		/* nada */

    for (i = 0; i < NCARGS; i++) {
	if ((i >= from) && (i <= to)) {	/* if in range */
	    for (s = sp->word; *s && d < e; s++) {
		/*
		 * bugfix by Michael Bloom: anything but the current history
		 * character {(PWP) and backslash} seem to be dealt with
		 * elsewhere.
		 */
		if ((*s & QUOTE)
		    && (((*s & TRIM) == HIST) ||
			((*s & TRIM) == '\\') && (prev_c != '\\'))) {
		    *d++ = '\\';
		}
		*d++ = (*s & TRIM);
		prev_c = *s;
	    }
	    if (d < e)
		*d++ = ' ';
	}
	sp = sp->next;
	if (sp == sp0)
	    break;
    }
    if (d > buf)
	d--;			/* get rid of trailing space */

    return (d);
}

Char   *
sprlex(buf, sp0)
Char   *buf;
struct wordent *sp0;
{
    Char   *cp;

    cp = expand_lex(buf, INBUFSIZ, sp0, 0, NCARGS);
    *cp = '\0';
    return (buf);
}

void
Itoa(n, s)			/* convert n to characters in s */
int     n;
Char   *s;
{
    int     i,
            sign;

    if ((sign = n) < 0)		/* record sign */
	n = -n;
    i = 0;
    do {
	s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
	s[i++] = '-';
    s[i] = '\0';
    Reverse(s);
}

static void
Reverse(s)
Char   *s;
{
    int     c,
            i,
            j;

    for (i = 0, j = Strlen(s) - 1; i < j; i++, j--) {
	c = s[i];
	s[i] = s[j];
	s[j] = c;
    }
}


void
dolist(v)
register Char **v;
{
    int     i,
            k;
    struct stat st;

    if (*++v == NULL) {
	(void) t_search(STRNULL, (Char *) 0, LIST, 0, 0, 0);
	return;
    }
    gflag = 0;
    tglob(v);
    if (gflag) {
	v = globall(v);
	if (v == 0)
	    stdbferr(ERR_NOMATCH);
    }
    else
	v = gargv = saveblk(v);
    trim(v);
    for (k = 0; v[k] != NULL && v[k][0] != '-'; k++);
    if (v[k]) {
	/*
	 * We cannot process a flag therefore we let ls doit it right.
	 */
	static Char STRls[] =
	{'l', 's', '\0'};
	static Char STRmCF[] =
	{'-', 'C', 'F', '\0'};
	struct command *t;
	struct wordent cmd,
	       *nextword,
	       *lastword;
	Char   *cp;

#ifdef BSDSIGS
	sigmask_t omask = 0;

	if (setintr)
	    omask = sigblock(sigmask(SIGINT)) & ~sigmask(SIGINT);
#else
	sighold(SIGINT);
#endif
	err = (char *) 0;
	cmd.word = STRNULL;
	lastword = &cmd;
	nextword = (struct wordent *) calloc(1, sizeof cmd);
	nextword->word = Strsave(STRls);
	lastword->next = nextword;
	nextword->prev = lastword;
	lastword = nextword;
	nextword = (struct wordent *) calloc(1, sizeof cmd);
	nextword->word = Strsave(STRmCF);
	lastword->next = nextword;
	nextword->prev = lastword;
	lastword = nextword;
	for (cp = *v; cp; cp = *++v) {
	    nextword = (struct wordent *) calloc(1, sizeof cmd);
	    nextword->word = Strsave(cp);
	    lastword->next = nextword;
	    nextword->prev = lastword;
	    lastword = nextword;
	}
	lastword->next = &cmd;
	cmd.prev = lastword;

	/* build a syntax tree for the command. */
	t = syntax(cmd.next, &cmd, 0);
	if (err)
	    error(err);
	/* expand aliases like process() does */
	/* alias(&cmd); */
	/* execute the parse tree. */
	execute(t, tpgrp > 0 ? tpgrp : -1);
	/* done. free the lex list and parse tree. */
	freelex(&cmd), freesyn(t);
	if (setintr)
#ifdef BSDSIGS
	    (void) sigsetmask(omask);
#else
	    (void) sigrelse(SIGINT);
#endif
    }
    else {
	Char    tmp[MAXPATHLEN];
	int     p;

	for (k = 0, i = 0; v[k] != NULL; k++) {
	    (void) Strncpy(tmp, v[k], MAXPATHLEN);
	    tmp[MAXPATHLEN - 1] = '\0';
	    if (tmp[p = Strlen(tmp) - 1] == '/' && p != 0)
		tmp[p] = '\0';
	    else
		p++;
	    if (stat(short2str(tmp), &st) == -1) {
		if (k != i) {
		    if (i != 0)
			CSHputchar('\n');
		    print_by_column(STRNULL, &v[i], k - i, FALSE);
		}
		CSHprintf("%s: %s.\n", short2str(tmp), strerror());
		i = k + 1;
	    }
	    else if (isdir(st)) {
		Char   *cp;

		if (k != i) {
		    if (i != 0)
			CSHputchar('\n');
		    print_by_column(STRNULL, &v[i], k - i, FALSE);
		}
		if (k != 0 && v[1] != NULL)
		    CSHputchar('\n');
		CSHprintf("%s:\n", short2str(tmp));
		for (cp = tmp; *cp; cp++)
		    *cp |= QUOTE;
		tmp[p] = '/';
		(void) t_search(tmp, (Char *) 0, LIST, 0, 0, 0);
		i = k + 1;
	    }
	}
	if (k != i) {
	    if (i != 0)
		CSHputchar('\n');
	    print_by_column(STRNULL, &v[i], k - i, FALSE);
	}
    }

    if (gargv) {
	blkfree(gargv);
	gargv = 0;
    }
}

static char *defaulttell = "ALL";
extern bool GotTermCaps;

void
dotelltc(v)
register Char **v;
{

    if (!GotTermCaps)
	GetTermCaps();

    TellTC(v[1] ? short2str(v[1]) : defaulttell);
}

void
doechotc(v)
register Char **v;
{
    if (!GotTermCaps)
	GetTermCaps();
    EchoTC(++v);
}

void
dosettc(v)
Char  **v;
{
    char    tv[2][BUFSIZ];

    if (!GotTermCaps)
	GetTermCaps();

    (void) strcpy(tv[0], short2str(v[1]));
    (void) strcpy(tv[1], short2str(v[2]));
    SetTC(tv[0], tv[1]);
}

/* The dowhich() is by:
 *  Andreas Luik <luik@isaak.isa.de>
 *  I S A  GmbH - Informationssysteme fuer computerintegrierte Automatisierung
 *  Azenberstr. 35
 *  D-7000 Stuttgart 1
 *  West-Germany
 * Thanks!!
 */

void
dowhich(v)
register Char **v;
{
    struct wordent lex[3];
    struct varent *vp;

    lex[0].next = &lex[1];
    lex[1].next = &lex[2];
    lex[2].next = &lex[0];

    lex[0].prev = &lex[2];
    lex[1].prev = &lex[0];
    lex[2].prev = &lex[1];

    lex[0].word = STRNULL;
    lex[2].word = STRret;

    while (*++v) {
	if (vp = adrof1(*v, &aliases)) {
	    CSHprintf("%s: \t aliased to ", short2str(*v));
	    blkpr(vp->vec);
	    CSHprintf("\n");
	}
	else {
	    lex[1].word = *v;
	    tellmewhat(lex);
	}
    }
}

/* PWP: a hack to start up your stopped editor on a single keystroke */
/* jbs - fixed hack so it worked :-) 3/28/89 */

struct process *
find_stopped_editor()
{
    register struct process *pp;
    register char *ep,
           *vp,
           *p;
    int     epl,
            vpl;

    if ((ep = getenv("EDINODE_OR")) != NULL) {	/* if we have a value */
	if ((p = strrchr(ep, '/')) != NULL) {	/* if it has a path */
	    ep = p + 1;		/* then we want only the last part */
	}
    }
    else {
	ep = "ed";
    }
    if ((vp = getenv("VISUAL")) != NULL) {	/* if we have a value */
	if ((p = strrchr(vp, '/')) != NULL) {	/* and it has a path */
	    vp = p + 1;		/* then we want only the last part */
	}
    }
    else {
	vp = "vi";
    }
    vpl = strlen(vp);
    epl = strlen(ep);

    if (pcurrent == PNULL)	/* see if we have any jobs */
	return PNULL;		/* nope */

    for (pp = proclist.p_next; pp; pp = pp->p_next) {
	if (pp->p_pid == pp->p_jobid) {
	    p = short2str(pp->p_command);
	    /* if we find either in the current name, fg it */
	    if (strncmp(ep, p, (size_t) epl) == 0 ||
		strncmp(vp, p, (size_t) vpl) == 0)
		return pp;
	}
    }
    return PNULL;		/* didn't find a job */
}

void
fg_a_proc_entry(pp)
register struct process *pp;
{
    sigmask_t omask;
    jmp_buf osetexit;
    bool    ohaderr;

    getexit(osetexit);

#ifdef BSDSIGS
    omask = sigblock(sigmask(SIGINT));
#else
    (void) sighold(SIGINT);
#endif

    ohaderr = haderr;		/* we need to ignore setting of haderr due to
				 * process getting stopped by a signal */
    if (setexit() == 0) {	/* come back here after pjwait */
	pendjob();
	pstart(pp, 1);		/* found it. */
	pjwait(pp);
    }

    resexit(osetexit);
    haderr = ohaderr;

#ifdef BSDSIGS
    (void) sigsetmask(omask);
#else
    sigrelse(SIGINT);
#endif

}

static void
auto_logout()
{
    CSHprintf("auto-logout\n");
    /* Don't leave the tty in raw mode */
    if (editing)
	(void) Cookedmode();
    (void) close(SHIN);
    set(STRlogout, STRautomatic);
    child = 1;
#ifdef TESLA
    do_logout = 1;
#endif				/* TESLA */
    goodbye();
}

sigret_t
alrmcatch()
{
    time_t  cl, nl;

    if ((nl = sched_next()) == -1)
	auto_logout();		/* no other possibility - logout */
    (void) time(&cl);
    if (nl <= cl + 1)
	sched_run();
    else
	auto_logout();
    setalarm();
#ifndef SIGVOID
    return (0);
#endif
}

/*
 * Karl Kleinpaste, 21oct1983.
 * Added precmd(), which checks for the alias
 * precmd in aliases.  If it's there, the alias
 * is executed as a command.  This is done
 * after mailchk() and just before print-
 * ing the prompt.  Useful for things like printing
 * one's current directory just before each command.
 */
void
precmd()
{
    sigmask_t omask;

#ifdef BSDSIGS
    omask = sigblock(sigmask(SIGINT));
#else
    (void) sighold(SIGINT);
#endif
    if (precmd_active) {	/* an error must have been caught */
	aliasrun(2, STRunalias, STRprecmd);
	CSHprintf("Faulty alias 'precmd' removed.\n");
	goto leave;
    }
    precmd_active = 1;
    if (!whyles && adrof1(STRprecmd, &aliases))
	aliasrun(1, STRprecmd, (Char *) 0);
  leave:
    precmd_active = 0;
#ifdef BSDSIGS
    (void) sigsetmask(omask);
#else
    sigrelse(SIGINT);
#endif
}

/*
 * Paul Placeway  11/24/87  Added cwd_cmd by hacking precmd() into
 * submission...  Run every time $cwd is set (after it is set).  Useful
 * for putting your machine and cwd (or anything else) in an xterm title
 * space.
 */
void
cwd_cmd()
{
    sigmask_t omask;

#ifdef BSDSIGS
    omask = sigblock(sigmask(SIGINT));
#else
    (void) sighold(SIGINT);
#endif
    if (cwdcmd_active) {	/* an error must have been caught */
	aliasrun(2, STRunalias, STRcwdcmd);
	CSHprintf("Faulty alias 'cwdcmd' removed.\n");
	goto leave;
    }
    cwdcmd_active = 1;
    if (!whyles && adrof1(STRcwdcmd, &aliases))
	aliasrun(1, STRcwdcmd, (Char *) 0);
  leave:
    cwdcmd_active = 0;
#ifdef BSDSIGS
    (void) sigsetmask(omask);
#else
    sigrelse(SIGINT);
#endif
}


/*
 * Karl Kleinpaste, 18 Jan 1984.
 * Added period_cmd(), which executes the alias "periodic" every
 * $tperiod minutes.  Useful for occasional checking of msgs and such.
 */
void
period_cmd()
{
    register Char *vp;
    time_t  t,
            interval;
    sigmask_t omask;

#ifdef BSDSIGS
    omask = sigblock(sigmask(SIGINT));
#else
    (void) sighold(SIGINT);
#endif
    if (periodic_active) {	/* an error must have been caught */
	aliasrun(2, STRunalias, STRperiodic);
	CSHprintf("Faulty alias 'periodic' removed.\n");
	goto leave;
    }
    periodic_active = 1;
    if (!whyles && adrof1(STRperiodic, &aliases)) {
	vp = value(STRtperiod);
	if (vp == NOSTR)
	    return;
	interval = getn(vp);
	(void) time(&t);
	if (t - t_period >= interval * 60) {
	    t_period = t;
	    aliasrun(1, STRperiodic, (Char *) 0);
	}
    }
  leave:
    periodic_active = 0;
#ifdef BSDSIGS
    (void) sigsetmask(omask);
#else
    sigrelse(SIGINT);
#endif
}

/*
 * Karl Kleinpaste, 21oct1983.
 * Set up a one-word alias command, for use for special things.
 * This code is based on the mainline of process().
 */
void
aliasrun(cnt, s1, s2)
int     cnt;
Char   *s1,
       *s2;
{
    struct wordent w,
           *new1,
           *new2;		/* for holding alias name */
    struct command *t = (struct command *) 0;
    jmp_buf osetexit;

    getexit(osetexit);
    err = (char *) 0;		/* don't repeatedly print err msg. */
    w.word = STRNULL;
    new1 = (struct wordent *) calloc(1, sizeof w);
    new1->word = Strsave(s1);
    if (cnt == 1) {
	/* build a lex list with one word. */
	w.next = w.prev = new1;
	new1->next = new1->prev = &w;
    }
    else {
	/* build a lex list with two words. */
	new2 = (struct wordent *) calloc(1, sizeof w);
	new2->word = Strsave(s2);
	w.next = new2->prev = new1;
	new1->next = w.prev = new2;
	new1->prev = new2->next = &w;
    }

    /* expand aliases like process() does. */
    alias(&w);
    /* build a syntax tree for the command. */
    t = syntax(w.next, &w, 0);
    if (err)
	error(err);

    psavejob();
    /* catch any errors here */
    if (setexit() == 0)
	/* execute the parse tree. */
	execute(t, tpgrp);	/* PWP: was (t, -1) */
    if (haderr) {
	haderr = 0;
	/*
	 * Either precmd, or cwdcmd, or periodic had an error. Call it again so
	 * that it is removed
	 */
	if (precmd_active)
	    precmd();
#ifdef notdef
	/*
	 * XXX: On the other hand, just interrupting them causes an error too.
	 * So if we hit ^C in the middle of cwdcmd or periodic the alias gets
	 * removed. We don't want that. Note that we want to remove precmd
	 * though, cause that could lead into an infinite loop. This should be
	 * fixed correctly, but then haderr should give us the whole exit
	 * status not just true or false.
	 */
	else if (cwdcmd_active)
	    cwd_cmd();
	else if (periodic_active)
	    period_cmd();
#endif
    }
    /* reset the error catcher to the old place */
    resexit(osetexit);
    prestjob();
    pendjob();
    /* done. free the lex list and parse tree. */
    freelex(&w), freesyn(t);
}

void
setalarm()
{
    struct varent *vp;
    Char   *cp;
    unsigned alrm_time = 0;
    long    cl, nl,
            sched_dif;

    if (vp = adrof(STRautologout)) {
	if (cp = vp->vec[0])
	    alrm_time = (atoi(short2str(cp)) * 60);
    }
    if ((nl = sched_next()) != -1) {
	(void) time(&cl);
	sched_dif = nl - cl;
	if ((alrm_time == 0) || (sched_dif < alrm_time))
	    alrm_time = ((int) sched_dif) + 1;
    }
    (void) alarm(alrm_time);	/* Autologout ON */
}

#define RMDEBUG			/* For now... */

void
rmstar(cp)
struct wordent *cp;
{
    struct wordent *we,
           *args;
    register struct wordent *tmp,
           *del;

#ifdef RMDEBUG
    static Char STRrmdebug[] =
    {'r', 'm', 'd', 'e', 'b', 'u', 'g', '\0'};
    Char   *tag;

#endif
    Char   *charac;
    char    c;
    int     ask = 0,
            star = 0,
            doit = 0,
            silent = 0;

    if (!adrof(STRrmstar))
	return;
#ifdef RMDEBUG
    tag = value(STRrmdebug);
#endif
    we = cp->next;
    while (*we->word == ';' && we != cp)
	we = we->next;
    while (we != cp) {
#ifdef RMDEBUG
	if (*tag)
	    CSHprintf("parsing command line\n");
#endif
	if (!Strcmp(we->word, STRrm)) {
	    args = we->next;
	    ask = (*args->word != '-');
	    while (*args->word == '-' && !silent) {	/* check options */
		for (charac = (args->word + 1); *charac && !silent; charac++)
		    silent = (*charac == 'i' || *charac == 'f');
		args = args->next;
	    }
	    ask = (ask || !ask && !silent);
	    if (ask) {
		for (; !star && *args->word != ';'
		     && args != cp; args = args->next)
		    if (!Strcmp(args->word, STRstar))
			star = 1;
		if (ask && star) {
		    CSHprintf("Do you really want to delete all files? [n/y] ");
		    flush();
		    (void) read(SHIN, &c, 1);
		    doit = (c == 'Y' || c == 'y');
		    while (c != '\n')
			(void) read(SHIN, &c, 1);
		    if (!doit) {
			/* remove the command instead */
			if (*tag)
			    CSHprintf("skipping deletion of files!\n");
			for (tmp = we;
			     *tmp->word != '\n' &&
			     *tmp->word != ';' && tmp != cp;) {
			    tmp->prev->next = tmp->next;
			    tmp->next->prev = tmp->prev;
			    xfree((ptr_t) tmp->word);
			    del = tmp;
			    tmp = tmp->next;
			    xfree((ptr_t) del);
			}
			if (*tmp->word == ';') {
			    tmp->prev->next = tmp->next;
			    tmp->next->prev = tmp->prev;
			    xfree((ptr_t) tmp->word);
			    del = tmp;
			    xfree((ptr_t) del);
			}
		    }
		}
	    }
	}
	for (we = we->next;
	     *we->word != ';' && we != cp;
	     we = we->next);
	if (*we->word == ';')
	    we = we->next;
    }
#ifdef RMDEBUG
    if (*tag) {
	CSHprintf("command line now is:\n");
	for (we = cp->next; we != cp; we = we->next)
	    CSHprintf("%s ", short2str(we->word));
    }
#endif
    return;
}

/*
 * if we don't have vfork(), things can still go in the wrong order
 * resulting in the famous 'Stopped (tty output)'. But some systems
 * don't permit the setpgid() call, (these are more recent secure
 * systems such as ibm's aix). Then we'd rather print an error message
 * than hang the shell!
 * I am open to suggestions how to fix that.
 */
void
pgetty(wanttty, pgrp)
int     wanttty,
        pgrp;
{
#ifdef BSDJOBS
#if defined(BSDSIGS) && defined(POSIXJOBS)
    sigmask_t omask = 0;

#endif				/* BSDSIGS && POSIXJOBS */

#ifdef JOBDEBUG
    CSHprintf("wanttty %d\n", wanttty);
#endif

#ifdef POSIXJOBS
    /*
     * christos: I am blocking the tty signals till I've set things
     * correctly....
     */
    if (wanttty > 0)
#ifdef BSDSIGS
	omask = sigblock(sigmask(SIGTSTP) | sigmask(SIGTTIN) | sigmask(SIGTTOU));
#else				/* BSDSIGS */
    {
	(void) sighold(SIGTSTP);
	(void) sighold(SIGTTIN);
	(void) sighold(SIGTTOU);
    }
#endif				/* BSDSIGS */

    if (wanttty >= 0 && tpgrp >= 0)
	if (setpgid(0, pgrp) == -1) {
#ifdef IBMAIX
	    CSHprintf("tcsh: setpgid error.\n");
	    xexit(0);
#endif				/* IBMAIX */
	}
#endif				/* POSIXJOBS */

    if (wanttty > 0)
	(void) tcsetpgrp(FSHTTY, pgrp);

#ifndef POSIXJOBS
    if (wanttty >= 0 && tpgrp >= 0)
	if (setpgid(0, pgrp) == -1) {
#ifdef IBMAIX
	    CSHprintf("tcsh: setpgid error.\n");
	    xexit(0);
#endif				/* IBMAIX */
	}
#else
    if (wanttty > 0)
#ifdef BSDSIGS
	(void) sigsetmask(omask);
#else				/* BSDSIGS */
    {
	(void) sigrelse(SIGTSTP);
	(void) sigrelse(SIGTTIN);
	(void) sigrelse(SIGTTOU);
    }
#endif				/* BSDSIGS */
#endif				/* POSIXJOBS */

    if (tpgrp > 0)
	tpgrp = 0;		/* gave tty away */
#endif				/* BSDJOBS */
}


#ifdef BSDJOBS
/* Check if command is in continue list
   and do a "aliasing" if it exists as a job in background */

#define CNDEBUG			/* For now */
void
continue_jobs(cp)
struct wordent *cp;
{
    struct wordent *we;
    register struct process *pp,
           *np;
    Char   *cmd,
           *continue_list,
           *continue_args_list;

#ifdef CNDEBUG
    Char   *tag;
    static Char STRcndebug[] =
    {'c', 'n', 'd', 'e', 'b', 'u', 'g', '\0'};

#endif
    bool    in_cont_list,
            in_cont_arg_list;


#ifdef CNDEBUG
    tag = value(STRcndebug);
#endif
    continue_list = value(STRcontinue);
    continue_args_list = value(STRcontinue_args);
    if (*continue_list == '\0' && *continue_args_list == '\0')
	return;

    we = cp->next;
    while (*we->word == ';' && we != cp)
	we = we->next;
    while (we != cp) {
#ifdef CNDEBUG
	if (*tag)
	    CSHprintf("parsing command line\n");
#endif
	cmd = we->word;
	in_cont_list = inlist(continue_list, cmd);
	in_cont_arg_list = inlist(continue_args_list, cmd);
	if (in_cont_list || in_cont_arg_list) {
#ifdef CNDEBUG
	    if (*tag)
		CSHprintf("in one of the lists\n");
#endif
	    np = PNULL;
	    for (pp = proclist.p_next; pp; pp = pp->p_next) {
		if (prefix(cmd, pp->p_command)) {
		    if (pp->p_index) {
			np = pp;
			break;
		    }
		}
	    }
	    if (np) {
		insert(we, in_cont_arg_list);
	    }
	}
	for (we = we->next;
	     *we->word != ';' && we != cp;
	     we = we->next);
	if (*we->word == ';')
	    we = we->next;
    }
#ifdef CNDEBUG
    if (*tag) {
	CSHprintf("command line now is:\n");
	for (we = cp->next; we != cp; we = we->next)
	    CSHprintf("%s ",
		      short2str(we->word));
    }
#endif
    return;
}

/* The actual "aliasing" of for backgrounds() is done here
   with the aid of insert_we().   */
static void
insert(plist, file_args)
struct wordent *plist;
bool    file_args;
{
    struct wordent *now,
           *last;
    Char   *cmd,
           *bcmd,
           *cp1,
           *cp2;
    int     cmd_len;
    Char   *pause = STRunderpause;
    int     p_len = Strlen(pause);

    cmd_len = Strlen(plist->word);
    cmd = (Char *) calloc(1, (size_t) ((cmd_len + 1) * sizeof(Char)));
    (void) Strcpy(cmd, plist->word);
/* Do insertions at beginning, first replace command word */

    if (file_args) {
	now = plist;
	xfree((ptr_t) now->word);
	now->word = (Char *) calloc(1, (size_t) (5 * sizeof(Char)));
	(void) Strcpy(now->word, STRecho);

	now = (struct wordent *) calloc(1, (size_t) sizeof(struct wordent));
	now->word = (Char *) calloc(1, (size_t) (6 * sizeof(Char)));
	(void) Strcpy(now->word, STRbackqpwd);
	insert_we(now, plist);

	for (last = now; *last->word != '\n' && *last->word != ';';
	     last = last->next);

	now = (struct wordent *) calloc(1, (size_t) sizeof(struct wordent));
	now->word = (Char *) calloc(1, (size_t) (2 * sizeof(Char)));
	(void) Strcpy(now->word, STRgt);
	insert_we(now, last->prev);

	now = (struct wordent *) calloc(1, (size_t) sizeof(struct wordent));
	now->word = (Char *) calloc(1, (size_t) (2 * sizeof(Char)));
	(void) Strcpy(now->word, STRbang);
	insert_we(now, last->prev);

	now = (struct wordent *) calloc(1, (size_t) sizeof(struct wordent));
	now->word = (Char *) calloc(1, (size_t) cmd_len + p_len + 4);
	cp1 = now->word;
	cp2 = cmd;
	*cp1++ = '~';
	*cp1++ = '/';
	*cp1++ = '.';
	while (*cp1++ = *cp2++);
	cp1--;
	cp2 = pause;
	while (*cp1++ = *cp2++);
	insert_we(now, last->prev);

	now = (struct wordent *) calloc(1, (size_t) sizeof(struct wordent));
	now->word = (Char *) calloc(1, (size_t) (2 * sizeof(Char)));
	(void) Strcpy(now->word, STRsemi);
	insert_we(now, last->prev);
	bcmd = (Char *) calloc(1, (size_t) ((cmd_len + 2) * sizeof(Char)));
	cp1 = bcmd;
	cp2 = cmd;
	*cp1++ = '%';
	while (*cp1++ = *cp2++);
	now = (struct wordent *) calloc(1, (size_t) (sizeof(struct wordent)));
	now->word = bcmd;
	insert_we(now, last->prev);
    }
    else {
	struct wordent *del;

	now = plist;
	xfree((ptr_t) now->word);
	now->word = (Char *) calloc(1, (size_t) ((cmd_len + 2) * sizeof(Char)));
	cp1 = now->word;
	cp2 = cmd;
	*cp1++ = '%';
	while (*cp1++ = *cp2++);
	for (now = now->next;
	     *now->word != '\n' && *now->word != ';' && now != plist;) {
	    now->prev->next = now->next;
	    now->next->prev = now->prev;
	    xfree((ptr_t) now->word);
	    del = now;
	    now = now->next;
	    xfree((ptr_t) del);
	}
    }
}

static void
insert_we(new, where)
struct wordent *new,
       *where;
{

    new->prev = where;
    new->next = where->next;
    where->next = new;
    new->next->prev = new;
}

static int
inlist(list, name)
Char   *list,
       *name;
{
    register Char *l,
           *n;

    l = list;
    n = name;

    while (*l && *n) {
	if (*l == *n) {
	    l++;
	    n++;
	    if (*n == '\0' && (*l == ' ' || *l == '\0'))
		return (1);
	    else
		continue;
	}
	else {
	    while (*l && *l != ' ')
		l++;		/* skip to blank */
	    while (*l && *l == ' ')
		l++;		/* and find first nonblank character */
	    n = name;
	}
    }
    return (0);
}

#endif				/* BSDJOBS */

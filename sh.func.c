/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.99/RCS/sh.func.c,v 2.1 1991/03/31 13:06:41 christos Exp christos $ */
/*
 * sh.func.c: csh builtin functions
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "config.h"

#ifdef notdef
static char *sccsid = "@(#)sh.func.c	5.3 (Berkeley) 5/13/86";
#endif
#ifndef lint
static char *rcsid = "$Id: sh.func.c,v 2.1 1991/03/31 13:06:41 christos Exp christos $";
#endif

#include "sh.h"

/*
 * C shell
 */

extern int just_signaled;
extern char **environ;

extern bool MapsAreInited;
extern bool NLSMapsAreInited;
extern bool NoNLSRebind;

static int zlast = -1;
static void islogin();
static void reexecute();
static void preread();
static void doagain();
static char *isrchx();
static int getword();
static int keyword();
static void Unsetenv();
static void toend();
static void xecho();

struct biltins *
isbfunc(t)
	struct command *t;
{
	register Char *cp = t->t_dcom[0];
	register struct biltins *bp, *bp1, *bp2;
	static struct biltins label = { "", dozip, 0, 0 };
	static struct biltins foregnd = { "%job", dofg1, 0, 0 };
	static struct biltins backgnd = { "%job &", dobg1, 0, 0 };

	if (lastchr(cp) == ':') {
		label.bname = short2str(cp);
		return (&label);
	}
	if (*cp == '%') {
		if (t->t_dflg & F_AMPERSAND) {
			t->t_dflg &= ~F_AMPERSAND;
			backgnd.bname = short2str(cp);
			return (&backgnd);
		}
		foregnd.bname = short2str(cp);
		return (&foregnd);
	}
#ifdef WARP
        /*
         * This is a perhaps kludgy way to determine if the warp
         * builtin is to be acknowledged or not.  If checkwarp()
         * fails, then we are to assume that the warp command is
         * invalid, and carry on as we would handle any other
         * non-builtin command.         -- JDK 2/4/88
         */
        if (eq(STRwarp, cp) && !checkwarp()) {
                return (0);     /* this builtin disabled */
        }
#endif 
	/*
	 * Binary search
	 * Bp1 is the beginning of the current search range.
	 * Bp2 is one past the end.
	 */
	for (bp1 = bfunc, bp2 = bfunc + nbfunc; bp1 < bp2;) {
		register i;

		bp = bp1 + ((bp2 - bp1) >> 1);
		if ((i = *cp - *bp->bname) == 0 &&
		    (i = Strcmp(cp, str2short(bp->bname))) == 0)
			return bp;
		if (i < 0)
			bp2 = bp;
		else
			bp1 = bp + 1;
	}
	return (0);
}

void
func(t, bp)
	register struct command *t;
	register struct biltins *bp;
{
	int i;

	xechoit(t->t_dcom);
	setname(bp->bname);
	i = blklen(t->t_dcom) - 1;
	if (i < bp->minargs)
		bferr("Too few arguments");
	if (i > bp->maxargs)
		bferr("Too many arguments");
	(*bp->bfunct)(t->t_dcom, t);
}

void
doonintr(v)
	Char **v;
{
	register Char *cp;
	register Char *vv = v[1];

	if (parintr == SIG_IGN)
		return;
	if (setintr && intty)
		stdbferr(ERR_TERMINAL);
	cp = gointr;
	gointr = 0;
	xfree((ptr_t) cp);
	if (vv == 0) {
#ifdef BSDSIGS
		if (setintr)
			(void) sigblock(sigmask(SIGINT));
		else
			(void) signal(SIGINT, SIG_DFL);
#else
		if (setintr)
			(void) sighold(SIGINT);
		else
			(void) sigset(SIGINT, SIG_DFL);
#endif
		gointr = 0;
	} else if (eq((vv = strip(vv)), STRminus)) {
#ifdef BSDSIGS
		(void) signal(SIGINT, SIG_IGN);
#else
		(void) sigset(SIGINT, SIG_IGN);
#endif
		gointr = STRminus;
	} else {
		gointr = Strsave(vv);
#ifdef BSDSIGS
		(void) signal(SIGINT, pintr);
#else
		(void) sigset(SIGINT, pintr);
#endif
	}
}

void
donohup()
{

	if (intty)
		stdbferr(ERR_TERMINAL);
	if (setintr == 0) {
		(void) signal(SIGHUP, SIG_IGN);
#ifdef CC
		submit(getpid());
#endif
	}
}

void
dozip()
{

	;
}

void
prvars()
{

	plist(&shvhed);
}

void
doalias(v)
	register Char **v;
{
	register struct varent *vp;
	register Char *p;

	v++;
	p = *v++;
	if (p == 0)
		plist(&aliases);
	else if (*v == 0) {
		vp = adrof1(strip(p), &aliases);
		if (vp)
			blkpr(vp->vec), CSHprintf("\n");
	} else {
		if (eq(p, STRalias) || eq(p, STRunalias)) {
			setname(short2str(p));
			bferr("Too dangerous to alias that");
		}
		set1(strip(p), saveblk(v), &aliases);
		tw_clear_comm_list();
	}
}

/*
 * PWP: read a bunch of aliases out of a file QUICKLY.  The format
 *  is almost the same as the result of saying "alias > FILE", except
 *  that saying "aliases > FILE" does not expand non-letters to printable
 *  sequences.
 */
void
doaliases(v)
	Char **v;
{
	jmp_buf oldexit;
	Char *p = (Char *) 0;
	int n = 0;
	Char **vec, *lp;
	int fd;
	Char buf[BUFSIZ], line[BUFSIZ];
	char tbuf[BUFSIZ+1];
	extern bool output_raw;	/* PWP: in sh.print.c */

	v++;
	if (*v == 0) {
		output_raw = 1;
		plist(&aliases);
		output_raw = 0;
		return;
	}

	gflag = 0, tglob(v);
	if (gflag) {
		v = globall(v);
		if (v == 0)
			stdbferr(ERR_NOMATCH);
	} else {
		v = gargv = saveblk(v);
		trim(v);
	}

	if ((fd = open(short2str(*v), 0)) < 0)
	    bferr("Cannot open file");

	getexit(oldexit);
	if (setexit() == 0) {
	    for (;;) {
		lp = line;
		for (;;) {
		    if (n <= 0) {
			int i;
			if ((n = read (fd, tbuf, BUFSIZ)) <= 0)
			    goto eof;
			for (i = 0; i < n; i++)
			    buf[i] = tbuf[i];
			p = buf;
		    }
		    n--;
		    if ((*lp++ = *p++) == '\n') {
			lp[-1] = '\0';
			break;
		    }
		}
		for (lp = line; *lp; lp++) {
		    if (isspc(*lp)) {
			*lp++ = '\0';
			while (isspc(*lp))
			    lp++;
			vec = (Char **) xalloc((size_t) 
					       (2 * sizeof (Char **)));
			vec[0] = Strsave(lp);
			vec[1] = NULL;
			setq(strip(line), vec, &aliases);
			break;
		    }
		}
	    }
	}

      eof:
	(void) close (fd);
	tw_clear_comm_list();
	if (gargv)
		blkfree(gargv), gargv = 0;
	resexit(oldexit);
}

void
unalias(v)
	Char **v;
{

	unset1(v, &aliases);
	tw_clear_comm_list();
}

void
dologout()
{

	islogin();
	goodbye();
}

void
dologin(v)
	Char **v;
{

	islogin();
	rechist();
	(void) signal(SIGTERM, parterm);
	(void) execl(_PATH_LOGIN, "login", short2str(v[1]), (Char *) 0);
	untty();
	xexit(1);
}


#ifdef NEWGRP
void
donewgrp(v)
	Char **v;
{

	if (chkstop == 0 && setintr)
		panystop(0);
	(void) signal(SIGTERM, parterm);
	(void) execl(_PATH_BIN_NEWGRP, "newgrp", short2str(v[1]),(char *)0);
	(void) execl(_PATH_USRBIN_NEWGRP, "newgrp", short2str(v[1]),(char *)0);
	untty();
	xexit(1);
}
#endif

static void
islogin()
{

	if (chkstop == 0 && setintr)
		panystop(0);
	if (loginsh)
		return;
	error("Not login shell");
}

void
doif(v, kp)
	Char **v;
	struct command *kp;
{
	register int i;
	register Char **vv;

	v++;
	i = exp(&v);
	vv = v;
	if (*vv == NOSTR)
		bferr("Empty if");
	if (eq(*vv, STRthen)) {
		if (*++vv)
			bferr("Improper then");
		setname(short2str(STRthen));
		/*
		 * If expression was zero, then scan to else,
		 * otherwise just fall into following code.
		 */
		if (!i)
			search(T_IF, 0);
		return;
	}
	/*
	 * Simple command attached to this if.
	 * Left shift the node in this tree, munging it
	 * so we can reexecute it.
	 */
	if (i) {
		lshift(kp->t_dcom, vv - kp->t_dcom);
		reexecute(kp);
		donefds();
	}
}

/*
 * Reexecute a command, being careful not
 * to redo i/o redirection, which is already set up.
 */
static void
reexecute(kp)
	register struct command *kp;
{

	kp->t_dflg &= F_SAVE;
	kp->t_dflg |= F_REPEAT;
	/*
	 * If tty is still ours to arbitrate, arbitrate it;
	 * otherwise dont even set pgrp's as the jobs would
	 * then have no way to get the tty (we can't give it
	 * to them, and our parent wouldn't know their pgrp, etc.
	 */
	execute(kp, (tpgrp > 0 ? tpgrp : -1),(int *)0,(int *)0);
}

void
doelse()
{

	search(T_ELSE, 0);
}

void
dogoto(v)
	Char **v;
{
	register struct whyle *wp;
	Char *lp;

	/*
	 * While we still can, locate any unknown ends of existing loops.
	 * This obscure code is the WORST result of the fact that we
	 * don't really parse.
	 */
	zlast = T_GOTO;
	for (wp = whyles; wp; wp = wp->w_next)
		if (wp->w_end == 0) {
			search(T_BREAK, 0);
			wp->w_end = btell();
		} else
			bseek(wp->w_end);
	search(T_GOTO, 0, lp = globone(v[1], G_ERROR));
	xfree((ptr_t) lp);
	/*
	 * Eliminate loops which were exited.
	 */
	wfree();
}

void
doswitch(v)
	register Char **v;
{
	register Char *cp = NULL, *lp;

	v++;
	if (!*v || *(*v++) != '(')
		stderror(ERR_SYNTAX);
	cp = **v == ')' ? STRNULL : *v++;
	if (*(*v++) != ')')
		v--;
	if (*v)
		stderror(ERR_SYNTAX);
	search(T_SWITCH, 0, lp = globone(cp, G_ERROR));
	xfree((ptr_t) lp);
}

void
dobreak()
{

	if (whyles)
		toend();
	else
		stdbferr(ERR_NOTWHILE);
}

void
doexit(v)
	Char **v;
{

	if (chkstop == 0)
		panystop(0);
	/*
	 * Don't DEMAND parentheses here either.
	 */
	v++;
	if (*v) {
		set(STRstatus, putn(exp(&v)));
		if (*v)
			stdbferr(ERR_EXPRESSION);
	}
	btoeof();
	if (intty)
		(void) close(SHIN);
}

void
doforeach(v)
	register Char **v;
{
	register Char *cp, *sp;
	register struct whyle *nwp;

	v++;
	sp = cp = strip(*v);
	if (!letter(*sp))
		stdbferr(ERR_VARBEGIN);
	while (*cp && alnum(*cp))
		cp++;
	if (*cp)
		stdbferr(ERR_VARALNUM);
	if ((cp - sp) > MAXVARLEN)
		stdbferr(ERR_VARTOOLONG);
	cp = *v++;
	if (v[0][0] != '(' || v[blklen(v) - 1][0] != ')')
		bferr("Words not ()'ed");
	v++;
	gflag = 0, tglob(v);
	v = globall(v);
	if (v == 0)
		stdbferr(ERR_NOMATCH);
	nwp = (struct whyle *) calloc(1, sizeof *nwp);
	nwp->w_fe = nwp->w_fe0 = v; gargv = 0;
	nwp->w_start = btell();
	nwp->w_fename = Strsave(cp);
	nwp->w_next = whyles;
	whyles = nwp;
	/*
	 * Pre-read the loop so as to be more
	 * comprehensible to a terminal user.
	 */
	zlast = T_FOREACH;
	if (intty)
		preread();
	doagain();
}

void
dowhile(v)
	Char **v;
{
	register int status;
	register bool again = whyles != 0 && whyles->w_start == lineloc &&
	    whyles->w_fename == 0;

	v++;
	/*
	 * Implement prereading here also, taking care not to
	 * evaluate the expression before the loop has been read up
	 * from a terminal.
	 */
	if (intty && !again)
		status = !exp0(&v, 1);
	else
		status = !exp(&v);
	if (*v)
		stdbferr(ERR_EXPRESSION);
	if (!again) {
		register struct whyle *nwp = 
			(struct whyle *) calloc(1, sizeof (*nwp));

		nwp->w_start = lineloc;
		nwp->w_end = 0;
		nwp->w_next = whyles;
		whyles = nwp;
		zlast = T_WHILE;
		if (intty) {
			/*
			 * The tty preread
			 */
			preread();
			doagain();
			return;
		}
	}
	if (status)
		/* We ain't gonna loop no more, no more! */
		toend();
}

static void
preread()
{

	whyles->w_end = -1;
	if (setintr)
#ifdef BSDSIGS
		(void) sigsetmask(sigblock(0) & ~sigmask(SIGINT));
#else
		(void) sigrelse(SIGINT);
#endif
	search(T_BREAK, 0);	/* read the expression in */
	if (setintr)
#ifdef BSDSIGS
		(void) sigblock(sigmask(SIGINT));
#else
		(void) sighold(SIGINT);
#endif
	whyles->w_end = btell();
}

void
doend()
{

	if (!whyles)
		stdbferr(ERR_NOTWHILE);
	whyles->w_end = btell();
	doagain();
}

void
docontin()
{

	if (!whyles)
		stdbferr(ERR_NOTWHILE);
	doagain();
}

static void
doagain()
{

	/* Repeating a while is simple */
	if (whyles->w_fename == 0) {
		bseek(whyles->w_start);
		return;
	}
	/*
	 * The foreach variable list actually has a spurious word
	 * ")" at the end of the w_fe list.  Thus we are at the
	 * of the list if one word beyond this is 0.
	 */
	if (!whyles->w_fe[1]) {
		dobreak();
		return;
	}
	set(whyles->w_fename, Strsave(*whyles->w_fe++));
	bseek(whyles->w_start);
}

void
dorepeat(v, kp)
	Char **v;
	struct command *kp;
{
	register int i;
#ifdef BSDSIGS
	register sigmask_t omask = 0;
#endif

	i = getn(v[1]);
	if (setintr)
#ifdef BSDSIGS
		omask = sigblock(sigmask(SIGINT)) & ~sigmask(SIGINT);
#else
		(void) sighold(SIGINT);
#endif
	lshift(v, 2);
	while (i > 0) {
		if (setintr)
#ifdef BSDSIGS
			(void) sigsetmask(omask);
#else
			sigrelse(SIGINT);
#endif
		reexecute(kp);
		--i;
	}
	donefds();
	if (setintr)
#ifdef BSDSIGS
		(void) sigsetmask(omask);
#else
		sigrelse(SIGINT);
#endif
}

void
doswbrk()
{

	search(T_BRKSW, 0);
}

int
srchx(cp)
	register Char *cp;
{
	register struct srch *sp, *sp1, *sp2;
	register i;

	/*
	 * Binary search
	 * Sp1 is the beginning of the current search range.
	 * Sp2 is one past the end.
	 */
	for (sp1 = srchn, sp2 = srchn + nsrchn; sp1 < sp2;) {
		sp = sp1 + ((sp2 - sp1) >> 1);
		if ((i = *cp - *sp->s_name) == 0 &&
		    (i = Strcmp(cp, str2short(sp->s_name))) == 0)
			return sp->s_value;
		if (i < 0)
			sp2 = sp;
		else
			sp1 = sp + 1;
	}
	return (-1);
}

static char *
isrchx(n)
register int n;
{
	register struct srch *sp, *sp2;

	for (sp = srchn, sp2 = srchn + nsrchn; sp < sp2; sp++)
	    if (sp->s_value == n)
		return(sp->s_name);
	return("");
}
		

static Char	Stype;
static Char	*Sgoal;

/*VARARGS2*/
void
search(type, level, goal)
	int type;
	register int level;
	Char *goal;
{
	Char wordbuf[BUFSIZ];
	register Char *aword = wordbuf;
	register Char *cp;

	Stype = type; Sgoal = goal;
	if (type == T_GOTO)
		bseek((off_t)0);
	do {
		if (intty && fseekp == feobp)
			printprompt(1, str2short(isrchx(type == T_BREAK ? 
							zlast : type))); 
			/* CSHprintf("? "), flush(); */
		aword[0] = 0;
		(void) getword(aword);
		switch (srchx(aword)) {

		case T_ELSE:
			if (level == 0 && type == T_IF)
				return;
			break;

		case T_IF:
			while (getword(aword))
				continue;
			if ((type == T_IF || type == T_ELSE) && 
			    eq(aword, STRthen))
				level++;
			break;

		case T_ENDIF:
			if (type == T_IF || type == T_ELSE)
				level--;
			break;

		case T_FOREACH:
		case T_WHILE:
			if (type == T_BREAK)
				level++;
			break;

		case T_END:
			if (type == T_BREAK)
				level--;
			break;

		case T_SWITCH:
			if (type == T_SWITCH || type == T_BRKSW)
				level++;
			break;

		case T_ENDSW:
			if (type == T_SWITCH || type == T_BRKSW)
				level--;
			break;

		case T_LABEL:
			if (type == T_GOTO && getword(aword) && eq(aword, goal))
				level = -1;
			break;

		default:
			if (type != T_GOTO && (type != T_SWITCH || level != 0))
				break;
			if (lastchr(aword) != ':')
				break;
			aword[Strlen(aword) - 1] = 0;
			if (type == T_GOTO && eq(aword, goal) || 
			    type == T_SWITCH && eq(aword, STRdefault))
				level = -1;
			break;

		case T_CASE:
			if (type != T_SWITCH || level != 0)
				break;
			(void) getword(aword);
			if (lastchr(aword) == ':')
				aword[Strlen(aword) - 1] = 0;
			cp = strip(Dfix1(aword));
			if (Gmatch(goal, cp))
				level = -1;
			xfree((ptr_t) cp);
			break;

		case T_DEFAULT:
			if (type == T_SWITCH && level == 0)
				level = -1;
			break;
		}
		(void) getword(NOSTR);
	} while (level >= 0);
}

static int
getword(wp)
	register Char *wp;
{
	register int found = 0;
	register int c, d;
	int kwd = 0;
	Char *owp = wp;
	static int keyword();

	c = readc(1);
	d = 0;
	do {
		while (c == ' ' || c == '\t')
			c = readc(1);
		if (c == '#')
			do
				c = readc(1);
			while (c >= 0 && c != '\n');
		if (c < 0)
			goto past;
		if (c == '\n') {
			if (wp)
				break;
			return (0);
		}
		unreadc(c);
		found = 1;
		do {
			c = readc(1);
			if (c == '\\' && (c = readc(1)) == '\n')
				c = ' ';
			if (c == '\'' || c == '"')
				if (d == 0)
					d = c;
				else if (d == c)
					d = 0;
			if (c < 0)
				goto past;
			if (wp) {
				*wp++ = c;
				*wp = 0;	/* end the string b4 test */
			}
		} while ((d || !(kwd = keyword(owp)) && c != ' ' 
						&& c != '\t') && c != '\n');
	} while (wp == 0);

	/*
	 * if we have read a keyword ( "if", "switch" or "while" )
	 * then we do not need to unreadc the look-ahead char
	 */
	if (!kwd) {
		unreadc(c);
		if (found)
			*--wp = 0;
	}

	return (found);

past:
	switch (Stype) {

	case T_IF:
		bferr("then/endif not found");

	case T_ELSE:
		bferr("endif not found");

	case T_BRKSW:
	case T_SWITCH:
		bferr("endsw not found");

	case T_BREAK:
		bferr("end not found");

	case T_GOTO:
		setname(short2str(Sgoal));
		bferr("label not found");
	}
	/*NOTREACHED*/
	return(0);
}

/*
 * keyword(wp) determines if wp is one of the built-n functions if,
 * switch or while. It seems that when an if statement looks like
 * "if(" then getword above sucks in the '(' and so the search routine
 * never finds what it is scanning for. Rather than rewrite doword, I hack
 * in a test to see if the string forms a keyword. Then doword stops
 * and returns the word "if" -strike
 */

static int
keyword(wp)
Char *wp;
{
	static Char STRif[] = { 'i', 'f', '\0' };
	static Char STRwhile[] = { 'w', 'h', 'i', 'l', 'e', '\0' };
	static Char STRswitch[] = { 's', 'w', 'i', 't', 'c', 'h', '\0' };
        if (!wp)
                return(0);
 
        if ((Strcmp(wp, STRif) == 0) || (Strcmp(wp, STRwhile) == 0)
            || (Strcmp(wp, STRswitch) == 0))
                return(1);
 
        return(0);
}

static void
toend()
{

	if (whyles->w_end == 0) {
		search(T_BREAK, 0, NOSTR);
		whyles->w_end = btell() - 1;
	} else
		bseek(whyles->w_end);
	wfree();
}

void
wfree()
{
	long o = btell();

	while (whyles) {
		register struct whyle *wp = whyles;
		register struct whyle *nwp = wp->w_next;

		if (o >= wp->w_start && (wp->w_end == 0 || o < wp->w_end))
			break;
		if (wp->w_fe0)
			blkfree(wp->w_fe0);
		if (wp->w_fename)
			xfree((ptr_t) wp->w_fename);
		xfree((ptr_t)wp);
		whyles = nwp;
	}
}

void
doecho(v)
	Char **v;
{

	xecho(' ', v);
}

void
doglob(v)
	Char **v;
{

	xecho(0, v);
	flush();
}

static void
xecho(sep, v)
	Char sep;
	register Char **v;
{
	register Char *cp;
	int nonl = 0;

	if (setintr)
#ifdef BSDSIGS
		(void) sigsetmask(sigblock(0) & ~sigmask(SIGINT));
#else
		(void) sigrelse(SIGINT);
#endif
	v++;
	if (*v == 0)
		return;
	gflag = 0, tglob(v);
	if (gflag) {
		v = globall(v);
		if (v == 0)
			stdbferr(ERR_NOMATCH);
	} else {
		v = gargv = saveblk(v);
		trim(v);
	}
	if (sep == ' ' && *v && eq(*v, STRmn))
		nonl++, v++;
	while (cp = *v++) {
		register int c;

		while (c = *cp++) {
#if SVID > 0
#ifndef OREO
		    if (c == '\\') {
			switch (c = *cp++) {
			    case 'b':   c = '\b';   break;
			    case 'c':   nonl = 1;   goto done;
			    case 'f':   c = '\f';   break;
			    case 'n':   c = '\n';   break;
			    case 'r':   c = '\r';   break;
			    case 't':   c = '\t';   break;
			    case 'v':   c = '\v';   break;
			    case '\\':  c = '\\';   break;
			    case '0':
				c = 0;
				if (*cp >= '0' && *cp < '8')
				    c = c * 8 + *cp++ - '0';
				if (*cp >= '0' && *cp < '8')
				    c = c * 8 + *cp++ - '0';
				if (*cp >= '0' && *cp < '8')
				    c = c * 8 + *cp++ - '0';
				break;
			    case '\0':
				c = *--cp;
				break;
			    default:
				CSHputchar('\\' | QUOTE);
				break;
			}
		    }
#endif /* OREO */
#endif /* SVID > 0 */
		    CSHputchar(c | QUOTE);

		}
		if (*v)
			CSHputchar(sep | QUOTE);
	}
#if SVID > 0
#ifndef OREO
done:
#endif /* OREO */
#endif /* SVID > 0 */
	if (sep && nonl == 0)
		CSHputchar('\n');
	else
		flush();
	if (setintr)
#ifdef BSDSIGS
		(void) sigblock(sigmask(SIGINT));
#else
		(void) sighold(SIGINT);
#endif
	if (gargv)
		blkfree(gargv), gargv = 0;
}

/* from "Karl Berry." <karl%mote.umb.edu@relay.cs.net> -- for NeXT things
   (and anything else with a modern compiler) */

void
dosetenv(v)
	register Char **v;
{
	Char *vp, *lp;

	v++;
	if ((vp = *v++) == 0) {
		register Char **ep;

		if (setintr)
#ifdef BSDSIGS
			(void) sigsetmask(sigblock(0) & ~ sigmask(SIGINT));
#else
			(void) sigrelse(SIGINT);
#endif
		for (ep = STR_environ; *ep; ep++)
			CSHprintf("%s\n", short2str(*ep));
		return;
	}
	if ((lp = *v++) == 0)
		lp = STRNULL;
	Setenv(vp, lp = globone(lp, G_ERROR));
	if (eq(vp, STRPATH)) {
		importpath(lp);
		dohash();
	}
	else if (eq(vp, STRLANG) || eq(vp, STRLC_CTYPE)) {
#ifdef NLS
		int k;

		(void) setlocale(LC_ALL,"");
		for (k=0200; k <= 0377 && ! isprint(k); k++);
		AsciiOnly = k > 0377;
#else
                AsciiOnly = 0;
#endif /* NLS */
		NLSMapsAreInited = 0;
		ed_Init();
                if (MapsAreInited && !NLSMapsAreInited)
                    (void) ed_InitNLSMaps();
	}
	else if (eq(vp, STRNOREBIND)) {
                NoNLSRebind = 1;
        }
#ifdef SIG_WINDOW
	else if ((eq(lp, STRNULL) && 
		 (eq(vp, STRLINES) || eq(vp, STRCOLUMNS))) || 
		  eq(vp, STRTERMCAP)) {
		check_window_size(1);
	}
#endif /* SIG_WINDOW */
	xfree((ptr_t) lp);
}

void
dounsetenv(v)
	register Char **v;
{

	v++;
	do {
		Unsetenv(*v);
                if (eq(*v, STRNOREBIND)) {
                    NoNLSRebind = 0;
                } else if (eq(*v, STRLANG) || eq(*v, STRLC_CTYPE)) {
#ifdef NLS
		    int k;

		    (void) setlocale(LC_ALL,"");
		    for (k=0200; k <= 0377 && ! isprint(k); k++);
		    AsciiOnly = k > 0377;
#else
		    AsciiOnly = getenv("LANG") == NULL && 
				getenv("LC_CTYPE") == NULL;
#endif	/* NLS */
		    NLSMapsAreInited = 0;
		    ed_Init();
		    if (MapsAreInited && !NLSMapsAreInited)
			(void) ed_InitNLSMaps();

                }
        }
	while (*++v);
}

void
Setenv(name, val)
	Char *name, *val;
{
#ifdef MACH
	char nameBuf[BUFSIZ];
	char *cname = short2str(name);

        if (cname == (char *) 0)
		return;
        (void) strcpy(nameBuf, cname);
	setenv(nameBuf, short2str(val), 1);
#else
	register Char **ep = STR_environ;
	register Char *cp, *dp;
	Char *blk[2];
	Char **oep = ep;

	
	for (; *ep; ep++) {
		for (cp = name, dp = *ep; *cp && *cp == *dp; cp++, dp++)
			continue;
		if (*cp != 0 || *dp != '=')
			continue;
		cp = Strspl(STRequal, val);
		xfree((ptr_t) *ep);
		*ep = Strspl(name, cp);
		xfree((ptr_t) cp);
		trim(ep);
		environ = short2blk(STR_environ);
		return;
	}
	blk[0] = Strspl(name, STRequal); blk[1] = 0;
	STR_environ = blkspl(STR_environ, blk);
	environ = short2blk(STR_environ);
	xfree((ptr_t)oep);
	Setenv(name, val);
#endif /* MACH */
}

static void
Unsetenv(name)
	Char *name;
{
	register Char **ep = STR_environ;
	register Char *cp, *dp;
	Char **oep = ep;

	for (; *ep; ep++) {
		for (cp = name, dp = *ep; *cp && *cp == *dp; cp++, dp++)
			continue;
		if (*cp != 0 || *dp != '=')
			continue;
		cp = *ep;
		*ep = 0;
		STR_environ = blkspl(STR_environ, ep+1);
		environ = short2blk(STR_environ);
		*ep = cp;
		xfree((ptr_t) cp);
		xfree((ptr_t) oep);
		return;
	}
}

void
doumask(v)
	register Char **v;
{
	register Char *cp = v[1];
	register int i;

	if (cp == 0) {
		i = umask(0);
		(void) umask(i);
		CSHprintf("%o\n", i);
		return;
	}
	i = 0;
	while (isdigit(*cp) && *cp != '8' && *cp != '9')
		i = i * 8 + *cp++ - '0';
	if (*cp || i < 0 || i > 0777)
		bferr("Improper mask");
	(void) umask(i);
}

#ifndef BSDTIMES
   typedef long RLIM_TYPE;
# ifndef RLIM_INFINITY
   extern RLIM_TYPE ulimit();
#  define RLIM_INFINITY 0x3fffff
#  define RLIMIT_FSIZE 1
# endif /* RLIM_INFINITY */
# ifdef aiws
#  define toset(a) (((a) == 3) ? 1004 : (a) + 1)
#  define RLIMIT_STACK 1005
# else /* aiws */
#  define toset(a) ((a) + 1)
# endif /* aiws */
#else /* BSDTIMES */
 typedef int RLIM_TYPE;
#endif /* BSDTIMES */


static struct limits {
	int	limconst;
	char	*limname;
	int	limdiv;
	char	*limscale;
} limits[] = {
#ifndef BSDTIMES
	RLIMIT_FSIZE,	"filesize",	1024,	"kbytes",
# ifdef aiws
	RLIMIT_STACK,	"stacksize",	1024,	"kbytes",
# endif /* aiws */
#else /* BSDTIMES */
	RLIMIT_CPU,	"cputime",	1,	"seconds",
	RLIMIT_FSIZE,	"filesize",	1024,	"kbytes",
	RLIMIT_DATA,	"datasize",	1024,	"kbytes",
	RLIMIT_STACK,	"stacksize",	1024,	"kbytes",
	RLIMIT_CORE,	"coredumpsize",	1024,	"kbytes",
	RLIMIT_RSS,	"memoryuse",	1024,	"kbytes",
# ifdef RLIMIT_NOFILE
	RLIMIT_NOFILE,	"descriptors",  1, 	"",
# endif
#if defined(convex) || defined(__convex__)
	RLIMIT_CONCUR,  "concurrency",  1,      "thread(s)",
#endif
#endif /* BSDTIMES */
	-1,		(char *) 0,	0,	(char *) 0
};

static struct limits *findlim();
static RLIM_TYPE getval();
static void limtail();
static void plim();
static int setlim();

#if defined(convex) || defined(__convex__)
static RLIM_TYPE 
restrict_limit(value)
double  value;
{
    /*
     * is f too large to cope with? return the maximum or minimum int
     */
    if (value > (double) INT_MAX)
	return (INT_MAX);
    else if (value < (double) INT_MIN)
	return (INT_MIN);
    else
	return ((int) value);
}

#endif
static struct limits *
findlim(cp)
	Char *cp;
{
	register struct limits *lp, *res;

	res = (struct limits *) NULL;
	for (lp = limits; lp->limconst >= 0; lp++)
		if (prefix(cp, str2short(lp->limname))) {
			if (res)
				stdbferr(ERR_AMBIG);
			res = lp;
		}
	if (res)
		return (res);
	bferr("No such limit");
	/*NOTREACHED*/
	return(0);
}

void
dolimit(v)
	register Char **v;
{
	register struct limits *lp;
	register RLIM_TYPE limit;
	char hard = 0;

	v++;
	if (*v && eq(*v, STRmh)) {
		hard = 1;
		v++;
	}
	if (*v == 0) {
		for (lp = limits; lp->limconst >= 0; lp++)
			plim(lp, hard);
		return;
	}
	lp = findlim(v[0]);
	if (v[1] == 0) {
		plim(lp,  hard);
		return;
	}
	limit = getval(lp, v+1);
	if (setlim(lp, hard, limit) < 0)
		error((char *) 0);
}

static RLIM_TYPE
getval(lp, v)
	register struct limits *lp;
	Char **v;
{
#if defined(convex) || defined(__convex__)
	RLIM_TYPE restrict_limit();
#endif
	register float f;
	double atof();
	Char *cp = *v++;

	f = atof(short2str(cp));

#if defined(convex)||defined(__convex__)
        /*
         * is f too large to cope with. limit f
         * to minint, maxint  - X-6768 by strike
         */
        if ((f < (double)INT_MIN) || (f > (double)INT_MAX)) {
                bferr("Argument too large");
        }
#endif

	while (isdigit(*cp) || *cp == '.' || *cp == 'e' || *cp == 'E')
		cp++;
	if (*cp == 0) {
		if (*v == 0)
#if defined(convex) || defined(__convex__)
		    return ((RLIM_TYPE)restrict_limit((f+0.5) * lp->limdiv));
#else
		    return ((RLIM_TYPE) ((f + 0.5) * lp->limdiv));
#endif
		cp = *v;
	}
	switch (*cp) {
#ifdef RLIMIT_CPU
	case ':':
		if (lp->limconst != RLIMIT_CPU)
			goto badscal;
#if defined(convex) || defined(__convex__)
		return ((RLIM_TYPE)
			restrict_limit((f * 60.0 + atof(short2str(cp+1)))));
#else
		return ((RLIM_TYPE)(f * 60.0 + atof(short2str(cp+1))));
#endif
	case 'h':
		if (lp->limconst != RLIMIT_CPU)
			goto badscal;
		limtail(cp, "hours");
		f *= 3600.0;
		break;
	case 'm':
		if (lp->limconst == RLIMIT_CPU) {
			limtail(cp, "minutes");
			f *= 60.0;
			break;
		}
		*cp = 'm';
		limtail(cp, "megabytes");
		f *= 1024.0 * 1024.0;
		break;
	case 's':
		if (lp->limconst != RLIMIT_CPU)
			goto badscal;
		limtail(cp, "seconds");
		break;
#endif /* RLIMIT_CPU */
	case 'M':
#ifdef RLIMIT_CPU
		if (lp->limconst == RLIMIT_CPU)
			goto badscal;
#endif /* RLIMIT_CPU */
		*cp = 'm';
		limtail(cp, "megabytes");
		f *= 1024.0 * 1024.0;
		break;
	case 'k':
#ifdef RLIMIT_CPU
		if (lp->limconst == RLIMIT_CPU)
			goto badscal;
#endif /* RLIMIT_CPU */
		limtail(cp, "kbytes");
		f *= 1024.0;
		break;
	case 'u':
		limtail(cp, "unlimited");
		return (RLIM_INFINITY);
	default:
#ifdef RLIMIT_CPU
badscal:
#endif /* RLIMIT_CPU */
		bferr("Improper or unknown scale factor");
	}
#if defined(convex) || defined(__convex__)
	return ((RLIM_TYPE)restrict_limit((f+0.5)));
#else
	return ((RLIM_TYPE) (f + 0.5));
#endif
}

static void
limtail(cp, str)
	Char *cp;
	char *str;
{
	while (*cp && *cp == *str)
		cp++, str++;
	if (*cp)
		error("Bad scaling; did you mean ``%s''?", str);
}


/*ARGSUSED*/
static void
plim(lp, hard)
	register struct limits *lp;
	Char hard;
{
#ifdef BSDTIMES
	struct rlimit rlim;
#endif /* BSDTIMES */ 
	RLIM_TYPE limit;

	CSHprintf("%s \t", lp->limname);

#ifndef BSDTIMES
	limit = ulimit(lp->limconst, 0);
	if (limit != RLIM_INFINITY && lp->limconst == RLIMIT_FSIZE)
		limit *= 512;
#else /* BSDTIMES */
	(void) getrlimit(lp->limconst, &rlim);
	limit = hard ? rlim.rlim_max : rlim.rlim_cur;
#endif /* BSDTIMES */

	if (limit == RLIM_INFINITY)
		CSHprintf("unlimited");
#ifdef RLIMIT_CPU
	else if (lp->limconst == RLIMIT_CPU)
		psecs((long)limit);
#endif /* RLIMIT_CPU */
	else
		CSHprintf("%ld %s", (long) (limit / lp->limdiv), lp->limscale);
	CSHprintf("\n");
}

void
dounlimit(v)
	register Char **v;
{
	register struct limits *lp;
	int lerr = 0;
	Char hard = 0;

	v++;
	if (*v && eq(*v, STRmh)) {
		hard = 1;
		v++;
	}
	if (*v == 0) {
		for (lp = limits; lp->limconst >= 0; lp++)
			if (setlim(lp, hard, (RLIM_TYPE)RLIM_INFINITY) < 0)
				lerr++;
		if (lerr)
			error((char *) 0);
		return;
	}
	while (*v) {
		lp = findlim(*v++);
		if (setlim(lp, hard, (RLIM_TYPE)RLIM_INFINITY) < 0)
			error((char *) 0);
	}
}

static int
setlim(lp, hard, limit)
	register struct limits *lp;
	Char hard;
	RLIM_TYPE limit;
{
#ifdef BSDTIMES
	struct rlimit rlim;

	(void) getrlimit(lp->limconst, &rlim);

	if (hard)
		rlim.rlim_max = limit;
  	else if (limit == RLIM_INFINITY && geteuid() != 0)
 		rlim.rlim_cur = rlim.rlim_max;
 	else
		rlim.rlim_cur = limit;

	if (setrlimit(lp->limconst, &rlim) < 0) {
#else /* BSDTIMES */
	if (limit != RLIM_INFINITY && lp->limconst == RLIMIT_FSIZE)
		limit /= 512;
	if (ulimit(toset(lp->limconst), limit) < 0) {
#endif /* BSDTIMES */
		CSHprintf("%s: %s: Can't %s%s limit\n", bname, lp->limname,
		    limit == RLIM_INFINITY ? "remove" : "set",
		    hard ? " hard" : "");
		return (-1);
	}
	return (0);
}

void
dosuspend()
{
	int ctpgrp;
	sigret_t (*old)();

	if (loginsh)
		error("Can't suspend a login shell (yet)");
	untty();

#ifdef BSDJOBS
	old = sigsys(SIGTSTP, SIG_DFL);
	(void) kill(0, SIGTSTP);
	/* the shell stops here */
	(void) sigsys(SIGTSTP, old);
#else 
	stderror(ERR_JOBCONTROL);
#endif /* BSDJOBS */

#ifdef BSDJOBS
	if (tpgrp != -1) {
retry:
		ctpgrp = tcgetpgrp(FSHTTY);
		if (ctpgrp != opgrp) {
			old = sigsys(SIGTTIN, SIG_DFL);
			(void) kill(0, SIGTTIN);
			(void) sigsys(SIGTTIN, old);
			goto retry;
		}
		(void) setpgid(0, shpgrp);
		(void) tcsetpgrp(FSHTTY, shpgrp);
	}
#endif /* BSDJOBS */
        (void) setdisc(FSHTTY);
}

/* This is the dreaded EVAL built-in.
 *   If you don't fiddle with file descriptors, and reset didfds,
 *   this command will either ignore redirection inside or outside
 *   its aguments, e.g. eval "date >x"  vs.  eval "date" >x
 *   The stuff here seems to work, but I did it by trial and error rather
 *   than really knowing what was going on.  If tpgrp is zero, we are
 *   probably a background eval, e.g. "eval date &", and we want to
 *   make sure that any processes we start stay in our pgrp.
 *   This is also the case for "time eval date" -- stay in same pgrp.
 *   Otherwise, under stty tostop, processes will stop in the wrong
 *   pgrp, with no way for the shell to get them going again.  -IAN!
 */
void
doeval(v)
	Char **v;
{
	Char **oevalvec;
	Char *oevalp;
	int odidfds;
#ifndef FIOCLEX
	int odidcch;
#endif /* FIOCLEX */
	int otpgrp;
	jmp_buf osetexit;
	int my_reenter;
	Char **gv;
	int saveIN;
        int saveOUT;
	int saveDIAG;
	int oSHIN;
	int oSHOUT;
	int oSHDIAG;

	oevalvec = evalvec;
	oevalp = evalp;
	odidfds = didfds;
#ifndef FIOCLEX
	odidcch = didcch;
#endif /* FIOCLEX */
	otpgrp = tpgrp;
	oSHIN = SHIN;
	oSHOUT = SHOUT;
	oSHDIAG = SHDIAG;

	v++;
	if (*v == 0)
		return;
	gflag = 0, tglob(v);
	if (gflag) {
		gv = v = globall(v);
		gargv = 0;
		if (v == 0)
			stderror(ERR_NOMATCH);
		v = copyblk(v);
	} else {
		gv = (Char **) 0;
		v = copyblk(v);
		trim(v);
	}

	saveIN = dcopy(SHIN, -1);
	saveOUT = dcopy(SHOUT, -1);
	saveDIAG = dcopy(SHDIAG, -1);

	getexit(osetexit);

	/* PWP: setjmp/longjmp bugfix for optimizing compilers */
	if ((my_reenter = setexit()) == 0) {
		evalvec = v;
		evalp = 0;
		if(tpgrp == 0)
			tpgrp = -1;     /* don't set pgrps (see note) */
		SHIN = dcopy(0, -1);
		SHOUT = dcopy(1, -1);
		SHDIAG = dcopy(2, -1);
#ifndef FIOCLEX
		didcch = 0;
#endif /* FIOCLEX */
		didfds = 0;
		process(0);
	}

	evalvec = oevalvec;
	evalp = oevalp;
	doneinp = 0;
#ifndef FIOCLEX
	didcch  = odidcch;
#endif /* FIOCLEX */
	didfds  = odidfds;
	tpgrp = otpgrp;
	(void) close(SHIN); 
	(void) close(SHOUT); 
	(void) close(SHDIAG);
	SHIN = dmove(saveIN, oSHIN);
	SHOUT = dmove(saveOUT, oSHOUT);
	SHDIAG = dmove(saveDIAG, oSHDIAG);

	if (gv)
		blkfree(gv);
	resexit(osetexit);
	if (my_reenter)
		error((char *) 0);
}

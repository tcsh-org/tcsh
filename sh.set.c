/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.set.c,v 2.0 1991/03/26 02:59:29 christos Exp $ */
/*
 * sh.set.c: Setting and Clearing of variables
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
static char *sccsid = "@(#)sh.set.c	5.2 (Berkeley) 6/6/85";
#endif
#ifndef lint
static char *rcsid = "$Id: sh.set.c,v 2.0 1991/03/26 02:59:29 christos Exp $";
#endif

#include "sh.h"

extern Char HistLit;
extern bool GotTermCaps;


static Char * getinx();
static void asx();
static struct varent * getvx();
static Char * xset();
static Char * operate();
static void putn1();
static struct varent *madrof();
static void unsetv1();
static void exportpath();
static void balance();


/*
 * C Shell
 */

void
doset(v)
	register Char **v;
{
	register Char *p;
	Char *vp, op;
	Char **vecp;
	bool hadsub;
	int subscr;

	v++;
	p = *v++;
	if (p == 0) {
		prvars();
		return;
	}
	do {
		hadsub = 0;
		vp = p;
		if (letter(*p))
		    for (; alnum(*p); p++)
			    continue;
		if (vp == p || !letter(*vp))
			stdbferr(ERR_VARBEGIN);
		if ((p-vp) > MAXVARLEN) {
			stdbferr(ERR_VARTOOLONG);
			return;
		}
		if (*p == '[') {
			hadsub++;
			p = getinx(p, &subscr);
		}
		if (op = *p) {
			*p++ = 0;
			if (*p == 0 && *v && **v == '(')
				p = *v++;
		} else if (*v && eq(*v, STRequal)) {
			op = '=', v++;
			if (*v)
				p = *v++;
		}
		if (op && op != '=')
			stdbferr(ERR_SYNTAX);
		if (eq(p, STRLparen)) {
			register Char **e = v;

			if (hadsub)
			    stdbferr(ERR_SYNTAX);
			for (;;) {
				if (!*e)
					bferr("Missing )");
				if (**e == ')')
					break;
				e++;
			}
			p = *e;
			*e = 0;
			vecp = saveblk(v);
			set1(vp, vecp, &shvhed);
			*e = p;
			v = e + 1;
		} else if (hadsub)
			asx(vp, subscr, Strsave(p));
		else
			set(vp, Strsave(p));
		if (eq(vp, STRpath)) {
			exportpath(adrof(STRpath)->vec);
			dohash();
		} else if (eq(vp, STRhistchars)) {
			register Char *pn = value(STRhistchars);

			HIST = *pn++;
			HISTSUB = *pn;
		} else if (eq(vp, STRhistlit)) {
			HistLit = 1;
		} else if (eq(vp, STRuser)) {
			Setenv(STRUSER, value(vp));
			Setenv(STRLOGNAME, value(vp));
		} else if (eq(vp, STRwordchars)) {
			word_chars = value(vp);
		} else if (eq(vp, STRterm)) {
#ifdef DOESNT_WORK_RIGHT
			register Char *cp;
#endif
			Setenv(STRTERM, value(vp));
#ifdef DOESNT_WORK_RIGHT
			cp = getenv("TERMCAP");
			if (cp && (*cp != '/')) /* if TERMCAP and not a path */
			    Unsetenv (STRTERMCAP);
#endif /* DOESNT_WORK_RIGHT */
			GotTermCaps = 0;
			ed_Init(); /* reset the editor */
		} else if (eq(vp, STRhome)) {
			register Char *cp;

			cp = Strsave(value(vp)); /* get the old value back */

			/*
			 * convert to cononical pathname (possibly
			 * resolving symlinks)
			 */
			cp = dcanon(cp, cp);

			set(vp, Strsave(cp)); /* have to save the new val */

			/* and now mirror home with HOME */
			Setenv(STRHOME, cp);
			/* fix directory stack for new tilde home */
			dtilde ();
			xfree((ptr_t) cp);
		} else if (eq(vp, STRedit)) {
			editing = 1;
			/* PWP: add more stuff in here later */
		} else if (eq(vp, STRshlvl)) {
			Setenv(STRSHLVL, value(vp));
		} else if (eq(vp, STRbackslash_quote)) {
			bslash_quote = 1;
		} else if (eq(vp, STRrecognize_only_executables)) {
			tw_clear_comm_list();
		} else if (eq(vp, STRwatch)) {
			resetwatch();
		}
	} while (p = *v++);
}

static Char *
getinx(cp, ip)
	register Char *cp;
	register int *ip;
{

	*ip = 0;
	*cp++ = 0;
	while (*cp && isdigit(*cp))
		*ip = *ip * 10 + *cp++ - '0';
	if (*cp++ != ']')
		bferr("Subscript Error");
	return (cp);
}

static void
asx(vp, subscr, p)
	Char *vp;
	int subscr;
	Char *p;
{
	register struct varent *v = getvx(vp, subscr);

	xfree((ptr_t) v->vec[subscr - 1]);
	v->vec[subscr - 1] = globone(p, G_APPEND);
}

static struct varent *
getvx(vp, subscr)
	Char *vp;
	int subscr;
{
	register struct varent *v = adrof(vp);

	if (v == 0)
		udvar(vp);
	if (subscr < 1 || subscr > blklen(v->vec))
		stdbferr(ERR_RANGE);
	return (v);
}

void
dolet(v)
	Char **v;
{
	register Char *p;
	Char *vp, c = 0, op = 0;
	bool hadsub;
	int subscr;

	v++;
	p = *v++;
	if (p == 0) {
		prvars();
		return;
	}
	do {
		hadsub = 0;
		vp = p;
		if (letter(*p))
		    for (; alnum(*p); p++)
			    continue;
		if (vp == p || !letter(*vp))
			stdbferr(ERR_VARBEGIN);
		if ((p-vp) > MAXVARLEN) 
			stdbferr(ERR_VARTOOLONG);
		if (*p == '[') {
			hadsub++;
			p = getinx(p, &subscr);
		}
		if (*p == 0 && *v)
			p = *v++;
		if (op = *p)
			*p++ = 0;
		else
			stdbferr(ERR_ASSIGN);

		/*
		 * if there is no expression after the '='
		 * then print a "Syntax Error" message - strike
		 */
		if (*p =='\0' && *v == NULL)
			stdbferr(ERR_ASSIGN);

		vp = Strsave(vp);
		if (op == '=') {
			c = '=';
			p = xset(p, &v);
		} else {
			c = *p++;
			if (any("+-", c)) {
				if (c != op || *p)
					stdbferr(ERR_UNKNOWNOP);
				p = STR1;
			} else {
				if (any("<>", op)) {
					if (c != op)
						stdbferr(ERR_UNKNOWNOP);
					c = *p++;
					stdbferr(ERR_SYNTAX);
				}
				if (c != '=')
					stdbferr(ERR_UNKNOWNOP);	
				p = xset(p, &v);
			}
		}
		if (op == '=')
			if (hadsub)
				asx(vp, subscr, p);
			else
				set(vp, p);
		else
			if (hadsub)
#ifndef V6
				/* avoid bug in vax CC */
				{
					struct varent *gv = getvx(vp, subscr);

					asx(vp, subscr, operate(op, gv->vec[subscr - 1], p));
				}
#else
				asx(vp, subscr, operate(op, getvx(vp, subscr)->vec[subscr - 1], p));
#endif
			else
				set(vp, operate(op, value(vp), p));
		if (eq(vp, STRpath)) {
			exportpath(adrof(STRpath)->vec);
			dohash();
		}
		xfree((ptr_t) vp);
		if (c != '=')
			xfree((ptr_t) p);
	} while (p = *v++);
}

static Char *
xset(cp, vp)
	Char *cp, ***vp;
{
	register Char *dp;

	if (*cp) {
		dp = Strsave(cp);
		--(*vp);
		xfree((ptr_t) **vp);
		**vp = dp;
	}
	return (putn(exp(vp)));
}

static Char *
operate(op, vp, p)
	Char op, *vp, *p;
{
	Char opr[2];
	Char *vec[5];
	register Char **v = vec;
	Char **vecp = v;
	register int i;

	if (op != '=') {
		if (*vp)
			*v++ = vp;
		opr[0] = op;
		opr[1] = 0;
		*v++ = opr;
		if (op == '<' || op == '>')
			*v++ = opr;
	}
	*v++ = p;
	*v++ = 0;
	i = exp(&vecp);
	if (*vecp)
		stdbferr(ERR_EXPRESSION);
	return (putn(i));
}

static	Char *putp;
 
Char *
putn(n)
	register int n; 
{
	int num;
	static Char number[15];

	putp = number;
	if (n < 0) {
		n = -n;
		*putp++ = '-';
	}
	num = 2;	/* comfuse lint */
	if (sizeof (int) == num && n == -32768) {
		*putp++ = '3';
		n = 2768;
#ifdef pdp11
	}
#else
	} else {
		num = 4;	/* comfuse lint */
		if (sizeof (int) == num && n == -2147483648) {
			*putp++ = '2';
			n = 147483648;
		}
	}
#endif
	putn1(n);
	*putp = 0;
	return (Strsave(number));
}

static void
putn1(n)
	register int n;
{
	if (n > 9)
		putn1(n / 10);
	*putp++ = n % 10 + '0';
}

int
getn(cp)
	register Char *cp;
{
	register int n;
	int sign;

	sign = 0;
	if (cp[0] == '+' && cp[1])
		cp++;
	if (*cp == '-') {
		sign++;
		cp++;
		if (!isdigit(*cp))
			goto badnum;
	}
	n = 0;
	while (isdigit(*cp))
		n = n * 10 + *cp++ - '0';
	if (*cp)
		goto badnum;
	return (sign ? -n : n);
badnum:
	bferr("Badly formed number");
	return (0);
}

Char *
value1(var, head)
	Char *var;
	struct varent *head;
{
	register struct varent *vp;

	vp = adrof1(var, head);
	return (vp == 0 || vp->vec[0] == 0 ? STRNULL : vp->vec[0]);
}

static struct varent *
madrof(pat, vp)
	Char *pat;
	register struct varent *vp;
{
	register struct varent *vp1;

	for (; vp; vp = vp->v_right) {
		if (vp->v_left && (vp1 = madrof(pat, vp->v_left)))
			return vp1;
		if (Gmatch(vp->v_name, pat))
			return vp;
	}
	return vp;
}

struct varent *
adrof1(name, v)
	register Char *name;
	register struct varent *v;
{
	register cmp;

	v = v->v_left;
	while (v && ((cmp = *name - *v->v_name) ||
		     (cmp = Strcmp(name, v->v_name))))
		if (cmp < 0)
			v = v->v_left;
		else
			v = v->v_right;
	return v;
}

/*
 * The caller is responsible for putting value in a safe place
 */
void
set(var, val)
	Char *var, *val;
{
	register Char **vec = (Char **) xalloc((size_t) 
					       (2 * sizeof (Char **)));

	vec[0] = onlyread(val) ? Strsave(val) : val;
	vec[1] = 0;
	set1(var, vec, &shvhed);
}

void
set1(var, vec, head)
	Char *var, **vec;
	struct varent *head;
{
	register Char **oldv = vec;

	gflag = 0; tglob(oldv);
	if (gflag) {
		vec = globall(oldv);
		if (vec == 0) {
			stdbferr(ERR_NOMATCH);
			blkfree(oldv);
			return;
		}
		blkfree(oldv);
		gargv = 0;
	}
	setq(var, vec, head);
}


void
setq(name, vec, p)
	Char *name, **vec;
	register struct varent *p;
{
	register struct varent *c;
	register f;

	f = 0;			/* tree hangs off the header's left link */
	while (c = p->v_link[f]) {
		if ((f = *name - *c->v_name) == 0 &&
		    (f = Strcmp(name, c->v_name)) == 0) {
			blkfree(c->vec);
			goto found;
		}
		p = c;
		f = f > 0;
	}
	p->v_link[f] = c = (struct varent *)xalloc((size_t) 
						   sizeof (struct varent));
	c->v_name = Strsave(name);
	c->v_bal = 0;
	c->v_left = c->v_right = 0;
	c->v_parent = p;
	balance(p, f, 0);
found:
	trim(c->vec = vec);
}

void
unset(v)
	Char *v[];
{
	register bool did_only;

	did_only = adrof(STRrecognize_only_executables) != 0;
	unset1(v, &shvhed);
	if (adrof(STRhistchars) == 0) {
		HIST = '!';
		HISTSUB = '^';
	}
	if (adrof(STRhistlit) == 0)
		HistLit = 0;
	if (adrof(STRwordchars) == 0)
		word_chars = STR_WORD_CHARS;
	if (adrof(STRedit) == 0)
		editing = 0;
	if (adrof(STRbackslash_quote) == 0)
		bslash_quote = 0;
	if (did_only && adrof(STRrecognize_only_executables) == 0)
		tw_clear_comm_list();
}

void
unset1(v, head)
	register Char *v[];
	struct varent *head;
{
	register struct varent *vp;
	register int cnt;

	while (*++v) {
		cnt = 0;
		while (vp = madrof(*v, head->v_left))
			unsetv1(vp), cnt++;
		if (cnt == 0)
			setname(short2str(*v));
	}
}

void
unsetv(var)
	Char *var;
{
	register struct varent *vp;

	if ((vp = adrof1(var, &shvhed)) == 0)
		udvar(var);
	unsetv1(vp);
}

static void
unsetv1(p)
	register struct varent *p;
{
	register struct varent *c, *pp;
	register f;

	/*
	 * Free associated memory first to avoid complications.
	 */
	blkfree(p->vec);
	xfree((ptr_t) p->v_name);
	/*
	 * If p is missing one child, then we can move the other
	 * into where p is.  Otherwise, we find the predecessor
	 * of p, which is guaranteed to have no right child, copy
	 * it into p, and move it's left child into it.
	 */
	if (p->v_right == 0)
		c = p->v_left;
	else if (p->v_left == 0)
		c = p->v_right;
	else {
		for (c = p->v_left; c->v_right; c = c->v_right)
			;
		p->v_name = c->v_name;
		p->vec = c->vec;
		p = c;
		c = p->v_left;
	}
	/*
	 * Move c into where p is.
	 */
	pp = p->v_parent;
	f = pp->v_right == p;
	if (pp->v_link[f] = c)
		c->v_parent = pp;
	/*
	 * Free the deleted node, and rebalance.
	 */
	xfree((ptr_t)p);
	balance(pp, f, 1);
}

void
setNS(cp)
	Char *cp;
{

	set(cp, STRNULL);
}

void
shift(v)
	register Char **v;
{
	register struct varent *argv;
	register Char *name;

	v++;
	name = *v;
	if (name == 0)
		name = STRargv;
	else
		(void) strip(name);
	argv = adrof(name);
	if (argv == 0)
		udvar(name);
	if (argv->vec[0] == 0)
		bferr("No more words");
	lshift(argv->vec, 1);
}

static void
exportpath(val)
Char **val;
{
	Char exppath[BUFSIZ];

	exppath[0] = 0;
	if (val)
		while (*val) {
			if (Strlen(*val) + Strlen(exppath) + 2 > BUFSIZ) {
				CSHprintf("Warning: ridiculously long PATH truncated\n");
				break;
			}
			(void) Strcat(exppath, *val++);
			if (*val == 0 || eq(*val, STRRparen))
				break;
			(void) Strcat(exppath, STRcolon);
		}
	Setenv(STRPATH, exppath);
}

#ifndef lint
	/*
	 * Lint thinks these have null effect
	 */
	/* macros to do single rotations on node p */
#define rright(p) (\
	t = (p)->v_left,\
	(t)->v_parent = (p)->v_parent,\
	((p)->v_left = t->v_right) ? (t->v_right->v_parent = (p)) : 0,\
	(t->v_right = (p))->v_parent = t,\
	(p) = t)
#define rleft(p) (\
	t = (p)->v_right,\
	(t)->v_parent = (p)->v_parent,\
	((p)->v_right = t->v_left) ? (t->v_left->v_parent = (p)) : 0,\
	(t->v_left = (p))->v_parent = t,\
	(p) = t)
#else
struct varent *rleft(p)  struct varent *p; { return(p); }
struct varent *rright(p) struct varent *p; { return(p); }
#endif /* ! lint */


/*
 * Rebalance a tree, starting at p and up.
 * F == 0 means we've come from p's left child.
 * D == 1 means we've just done a delete, otherwise an insert.
 */
static void
balance(p, f, d)
	register struct varent *p;
	register int f, d;
{
	register struct varent *pp;
#ifndef lint
	register struct varent *t;	/* used by the rotate macros */
#endif
	register ff;

	/*
	 * Ok, from here on, p is the node we're operating on;
	 * pp is it's parent; f is the branch of p from which we have come;
	 * ff is the branch of pp which is p.
	 */
	for (; pp = p->v_parent; p = pp, f = ff) {
		ff = pp->v_right == p;
		if (f ^ d) {		/* right heavy */
			switch (p->v_bal) {
			case -1:		/* was left heavy */
				p->v_bal = 0;
				break;
			case 0:			/* was balanced */
				p->v_bal = 1;
				break;
			case 1:			/* was already right heavy */
				switch (p->v_right->v_bal) {
				case 1:			/* sigle rotate */
					pp->v_link[ff] = rleft(p);
					p->v_left->v_bal = 0;
					p->v_bal = 0;
					break;
				case 0:			/* single rotate */
					pp->v_link[ff] = rleft(p);
					p->v_left->v_bal = 1;
					p->v_bal = -1;
					break;
				case -1:		/* double rotate */
					(void) rright(p->v_right);
					pp->v_link[ff] = rleft(p);
					p->v_left->v_bal =
						p->v_bal < 1 ? 0 : -1;
					p->v_right->v_bal =
						p->v_bal > -1 ? 0 : 1;
					p->v_bal = 0;
					break;
				}
				break;
			}
		} else {		/* left heavy */
			switch (p->v_bal) {
			case 1:			/* was right heavy */
				p->v_bal = 0;
				break;
			case 0:			/* was balanced */
				p->v_bal = -1;
				break;
			case -1:		/* was already left heavy */
				switch (p->v_left->v_bal) {
				case -1:		/* single rotate */
					pp->v_link[ff] = rright(p);
					p->v_right->v_bal = 0;
					p->v_bal = 0;
					break;
				case 0:			/* signle rotate */
					pp->v_link[ff] = rright(p);
					p->v_right->v_bal = -1;
					p->v_bal = 1;
					break;
				case 1:			/* double rotate */
					(void) rleft(p->v_left);
					pp->v_link[ff] = rright(p);
					p->v_left->v_bal =
						p->v_bal < 1 ? 0 : -1;
					p->v_right->v_bal =
						p->v_bal > -1 ? 0 : 1;
					p->v_bal = 0;
					break;
				}
				break;
			}
		}
		/*
		 * If from insert, then we terminate when p is balanced.
		 * If from delete, then we terminate when p is unbalanced.
		 */
		if ((p->v_bal == 0) ^ d)
			break;
	}
}

void
plist(p)
	register struct varent *p;
{
	register struct varent *c;
	register len;

	if (setintr) 

#ifdef BSDSIGS
		(void) sigsetmask(sigblock(0) & ~ sigmask(SIGINT));
#else /* BSDSIGS */
		sigrelse(SIGINT);
#endif /* BSDSIGS */

	for (;;) {
		while (p->v_left)
			p = p->v_left;
	x:
		if (p->v_parent == 0)		/* is it the header? */
			return;
		len = blklen(p->vec);
		CSHprintf(short2str(p->v_name));
		CSHputchar('\t');
		if (len != 1)
			CSHputchar('(');
		blkpr(p->vec);
		if (len != 1)
			CSHputchar(')');
		CSHputchar('\n');
		if (p->v_right) {
			p = p->v_right;
			continue;
		}
		do {
			c = p;
			p = p->v_parent;
		} while (p->v_right == c);
		goto x;
	}
}

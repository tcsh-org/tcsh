/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/ed.chared.c,v 1.7 1991/01/30 18:14:03 christos Exp $ */
/*
 * ed.chared.c: Character editing functions.
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id: ed.chared.c,v 1.7 1991/01/30 18:14:03 christos Exp $";
#endif

#include "sh.h"
#include "sh.dir.h"
#include "ed.h"
#include "ed.defns.h"

extern int GetNextChar();

/* all routines that start with c_ are private to this set of routines */

static void
c_alternativ_key_map(state)
    int state;
{
    AltKeyMap = state;
    if (state)
        CurrentKeyMap = CcAltMap;
    else
        CurrentKeyMap = CcKeyMap;
}

static void
c_insert(num)
register int num;
{
    register Char *cp;

    if (LastChar+num >= InputLim)
	return; /* can't go past end of buffer */

    if (Cursor < LastChar) {	/* if I must move chars */
	for (cp = LastChar; cp >= Cursor; cp--)
	    cp[num] = *cp;
    }
    LastChar += num;
}

static void
c_delafter(num)			/* delete after dot, with bounds checking */
register int num;
{
    register Char *cp;

    if (Cursor+num > LastChar)
	num = LastChar-Cursor; /* bounds check */

    if (num > 0) {	/* if I can delete anything */
	for (cp = Cursor; cp <= LastChar; cp++)
	    *cp = cp[num];
	LastChar -= num;
    }
    else
      replacemode = 0;
}

static void
c_delbefore(num)		/* delete before dot, with bounds checking */
register int num;
{
    register Char *cp;

    if (Cursor-num < InputBuf)
	num = Cursor-InputBuf; /* bounds check */

    if (num > 0) {	/* if I can delete anything */
	for (cp = Cursor-num; cp <= LastChar; cp++)
	    *cp = cp[num];
	LastChar -= num;
    }
}

static Char *
c_prev_word(p, low, n)
register Char *p, *low;
register int n;
{
    /* to the beginning of the PREVIOUS word, not this one */
    p--;

    while (n--) {
	while ((p >= low) && (!(isword(*p))))
	    p--;
	while ((p >= low) && (isword(*p)))
	    p--;
    }
    /* cp now points to one character before the word */
    p++;
    if (p < low)
	p = low;
    /* cp now points where we want it */
    return (p);
}

static Char *
c_next_word(p, high, n)
register Char *p, *high;
register int n;
{
    while (n--) {
	while ((p < high) && (!(isword(*p))))
	    p++;
	while ((p < high) && (isword(*p)))
	    p++;
    }
    if (p > high)
	p = high;
    /* p now points where we want it */
    return (p);
}

static Char *
c_beg_next_word(p, high, n)
register Char *p, *high;
register int n;
{
    while (n--) {
	while ((p < high) && (isword(*p)))
	    p++;
	while ((p < high) && (!(isword(*p))))
	    p++;
    }
    if (p > high)
	p = high;
    /* p now points where we want it */
    return (p);
}

/*
 * Expand-History (originally "Magic-Space") code added by
 * Ray Moody <ray@gibbs.physics.purdue.edu>
 * this is a neat, but odd, addition.
 */

/*
 * c_copy is sorta like bcopy() except that we handle overlap between
 * source and destination memory
 */

static void
c_copy(src, dst, length)
register Char *src, *dst;
register int length;
{
    if (src > dst) {
	while (length--) {
	    *dst++ = *src++;
	}
    } else {
	src += length;
	dst += length;
	while (length--) {
	    *--dst = *--src;
	}
    }
}

/*
 * c_number: Ignore character p points to, return number appearing after that.
 * A '$' by itself means a big number; "$-" is for negative; '^' means 1.
 * Return p pointing to last char used.
 */

/*
 * dval is the number to subtract from for things like $-3
 */

static Char *
c_number(p, num, dval)
register Char  *p;
register int   *num;
register int    dval;
{
    register int i;
    register int sign = 1;

    if (*++p == '^') {
	*num = 1;
	return (p);
    }
    if (*p == '$') {
	if (*++p != '-') {
	    *num = NCARGS;	/* Handle $ */
	    return (--p);
	}
	sign = -1;		/* Handle $- */
	++p;
    }
    for (i = 0; *p >= '0' && *p <= '9'; i = 10 * i + *p++ - '0');
    *num = (sign < 0 ? dval - i : i);
    return (--p);
}

/*
 * excl_expand: There is an excl to be expanded to p -- do the right thing
 * with it and return a version of p advanced over the expanded stuff.  Also,
 * update tsh_cur and related things as appropriate...
 */

static Char *
c_expand(p)
register Char  *p;
{
    register Char *q;
    register struct Hist *h = Histlist.Hnext;
    register struct wordent *l;
    int i, from, to, dval;
    bool all_dig;
    bool been_once = 0;
    Char *op = p;
    Char buf[INBUFSIZ];
    Char *bend = buf;
    Char *modbuf, *omodbuf;

    if (!h)
	goto excl_err;
excl_sw:
    switch (*(q = p + 1)) {

    case '^':
	bend = expand_lex(buf, INBUFSIZ, &h->Hlex, 1, 1);
	break;

    case '$':
	if ((l = (h->Hlex).prev))
	    bend = expand_lex(buf, INBUFSIZ, l->prev->prev, 0, 0);
	break;

    case '*':
	bend = expand_lex(buf, INBUFSIZ, &h->Hlex, 1, NCARGS);
	break;

    default:
	if (been_once) {	/* unknown argument */
	    /* assume it's a modifier, e.g. !foo:h, and get whole cmd */
	    bend = expand_lex(buf, INBUFSIZ, &h->Hlex, 0, NCARGS);
	    q -= 2;
	    break;
	}
	been_once = 1;

	if (*q == ':')		/* short form: !:arg */
	    --q;

	if (*q != HIST) {
	    /*
	     * Search for a space, tab, or colon.  See if we have a number
	     * (as in !1234:xyz).  Remember the number.
	     */
	    for (i = 0, all_dig = 1; *q != ' ' && *q != '\t' && *q != ':' && q < Cursor; q++) {
		/* PWP: !-4 is a valid history argument too, therefore
		   the test is if not a digit, or not a - as the first
		   character. */
		if ((*q < '0' || *q > '9') && (*q != '-' || q != p+1))
		    all_dig = 0;
		else
		    if (*q == '-')
			all_dig = 2; /* we are sneeky about this */
		    else
			i = 10 * i + *q - '0';
	    }
	    --q;

	    /*
	     * If we have a number, search for event i.  Otherwise,
	     * search for a named event (as in !foo).  (In this case, I
	     * is the length of the named event).
	     */
	    if (all_dig) {
		if (all_dig == 2)
		    i = -i; /* make it negitive */
		if (i < 0)	/* if !-4 (for example) */
		    i = eventno + 1 + i; /* remember: i is < 0 */
		for (; h; h = h->Hnext) {
		    if (h->Hnum == i)
			break;
		}
	    } else {
		for (i = q - p; h; h = h->Hnext) {
		    if ((l = &h->Hlex)) {
			if (!Strncmp(p + 1, l->next->word, i))
			    break;
		    }
		}
	    }
	}
	if (!h)
	    goto excl_err;
	if (q[1] == ':' || q[1] == '-' || q[1] == '*' ||
	    q[1] == '$' || q[1] == '^') { /* get some args */
	    p = q[1] == ':' ? ++q : q;
	    /*
	     * Go handle !foo:*
	     */
	    if ((q[1] < '0' || q[1] > '9') &&
		q[1] != '-' && q[1] != '$' && q[1] != '^')
		goto excl_sw;
	    /*
	     * Go handle !foo:$
	     */
	    if (q[1] == '$' && (q[2] != '-' || q[3] < '0' || q[3] > '9'))
		goto excl_sw;
	    /*
	     * Count up the number of words in this event.  Store it
	     * in dval.  Dval will be fed to number.
	     */
	    dval = 0;
	    if ((l = h->Hlex.prev)) {
		for (l = l->prev; l != h->Hlex.next; l = l->prev, dval++);
	    }
	    if (!dval)
		goto excl_err;
	    if (q[1] == '-')
		from = 0;
	    else
		q = c_number(q, &from, dval);
	    if (q[1] == '-') {
		++q;
		if ((q[1] < '0' || q[1] > '9') && q[1] != '$')
		    to = dval - 1;
		else
		    q = c_number(q, &to, dval);
	    } else if (q[1] == '*') {
		++q;
		to = NCARGS;
	    } else {
		to = from;
	    }
	    if (from < 0 || to < from)
		goto excl_err;
	    bend = expand_lex(buf, INBUFSIZ, &h->Hlex, from, to);
	} else {		/* get whole cmd */
	    bend = expand_lex(buf, INBUFSIZ, &h->Hlex, 0, NCARGS);
	}
	break;
    }

    /*
     * Apply modifiers, if any.
     */
    if (q[1] == ':') {
	*bend = '\0';
	omodbuf = buf;
	while (q[1] == ':' && (modbuf = domod(omodbuf, (int)q[2])) != NOSTR) {
	    if (omodbuf != buf)
		xfree((ptr_t) omodbuf);
	    omodbuf = modbuf;
	    q += 2;
	}
	if (omodbuf != buf) {
	    (void) Strcpy(buf, omodbuf);
	    xfree((ptr_t) omodbuf);
	    bend = Strend(buf);
	}
    }

    /*
     * Now replace the text from op to q inclusive with the text from buf to
     * bend.
     */
    q++;

    /*
     * Now replace text non-inclusively like a real CS major!
     */
    c_copy(q, q + (bend - buf) - (q - op), LastChar - q);
    LastChar += (bend - buf) - (q - op);
    Cursor += (bend - buf) - (q - op);
    c_copy(buf, op, (bend - buf)); 
    return (op + (bend - buf));
excl_err:
    Beep();
    return (op + 1);
}

/*
 * c_excl: An excl has been found at point p -- back up and find some white
 * space (or the beginning of the buffer) and properly expand all the excl's
 * from there up to the current cursor position. We also avoid (trying to)
 * expanding '>!'
 */

static void
c_excl(p)
register Char *p;
{
    register int i;
    register Char *q;

    /*
     * if />[SPC TAB]*![SPC TAB]/, back up p to just after the >.
     * otherwise, back p up to just before the current word.
     */
    if ((p[1] == ' ' || p[1] == '\t') &&
	(p[-1] == ' ' || p[-1] == '\t' || p[-1] == '>')) {
	for (q = p - 1; q > InputBuf && (*q == ' ' || *q == '\t'); --q);
	if (*q == '>')
	    ++p;
    } else {
	while (*p != ' ' && *p != '\t' && p > InputBuf)
	    --p;
    }

    /*
     * Forever:
     *     Look for history char.  (Stop looking when we find the cursor.)
     *     Count backslashes.  Of odd, skip history char.
     *     Return if all done.
     *     Expand if even number of backslashes.
     */
    for (;;) {
	while (*p != HIST && p < Cursor)
	    ++p;
	for (i = 1; (p - i) >= InputBuf && p[-i] == '\\'; i++);
	if (i % 2 == 0)
	    ++p;
	if (p >= Cursor)
	    return;
	if (i % 2 == 1)
	    p = c_expand(p);
    }
}
    

static void
c_substitute()
{
    register Char *p;

    /*
     * Start p out one character before the cursor.  Move it backwards
     * looking for white space, the beginning of the line, or a history
     * character.
     */
    for (p = Cursor - 1; p > InputBuf && *p != ' ' && *p != '\t' && *p != HIST; --p);

    /*
     * If we found a history character, go expand it.
     */
    if (*p == HIST)
	c_excl(p);
    Refresh();
}

/*
 * demi-PUBLIC routines.  Any routine that is of type CCRETVAL is an
 * entry point, called from the CcKeyMap indirected into the
 * CcFuncTbl array.
 */

/*VARARGS*/
CCRETVAL
v_cmd_mode()
{
    replacemode = 0;
    c_alternativ_key_map(1);
    if (Cursor > InputBuf)
	Cursor--;
    RefCursor();
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_unassigned()			/* bound to keys that arn't really assigned */
{
    Beep();
    flush();
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_insert(c)
register Char c;
{
#ifndef SHORT_STRINGS
    c &= ASCII;			/* no meta chars ever */
#endif

    if (!c) 
	return (CC_ERROR);	/* no NULs in the input ever!! */

    if (LastChar+Argument >= InputLim)
	return (CC_ERROR);	/* end of buffer space */

    if (Argument == 1) {	/* optimize */
	if (replacemode == 1)
	   c_delafter(1);
	else if (replacemode == 2)
	   c_delafter(1);
	c_insert(1);
	*Cursor++ = c;
	DoingArg = 0;		/* just in case */
	RefPlusOne();		/* fast refresh for one char. */
	if (replacemode == 2)
	   (void) v_cmd_mode();
    } else {
	if (replacemode == 1)
	   c_delafter(Argument);
	else if (replacemode == 2)
	   c_delafter(Argument);
	c_insert(Argument);
	while (Argument--)
	    *Cursor++ = c;
	Refresh();
	if (replacemode == 2)
	   (void) v_cmd_mode();
    }
    return (CC_NORM);
}

int
InsertStr(s)			/* insert ASCIZ s at cursor (for complete) */
Char *s;
{
    register int len;

    if ((len = Strlen(s)) <= 0)
	return -1;
    if (LastChar+len >= InputLim)
	return -1;		/* end of buffer space */

    c_insert(len);
    while (len--) 
	*Cursor++ = *s++;
    return 0;
}

void
DeleteBack(n)			/* delete the n characters before . */
int n;
{
    if (n <= 0) return;
    if (Cursor >= &InputBuf[n]) {
	c_delbefore(n);		/* delete before dot */
	Cursor -= n;
	if (Cursor < InputBuf) Cursor = InputBuf; /* bounds check */
    }
}

/*VARARGS*/
CCRETVAL
e_digit(c)			/* gray magic here */
register Char c;
{
    if (!isdigit(c)) 
	return (CC_ERROR);	/* no NULs in the input ever!! */

    if (DoingArg) {		/* if doing an arg, add this in... */
	if (LastCmd == F_ARGFOUR) /* if last command was ^U */
	    Argument = c - '0';
	else
	    Argument = (Argument * 10) + (c - '0');
	return (CC_ARGHACK);
    } else {
	if (LastChar+1 >= InputLim)
	    return CC_ERROR;		/* end of buffer space */

	c_insert(1);
	*Cursor++ = c;
	DoingArg = 0;		/* just in case */
	RefPlusOne();		/* fast refresh for one char. */
    }
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_argdigit(c)			/* for ESC-n */
register Char c;
{
    c &= ASCII;

    if (!isdigit(c)) 
	return (CC_ERROR);	/* no NULs in the input ever!! */

    if (DoingArg) {		/* if doing an arg, add this in... */
	Argument = (Argument * 10) + (c - '0');
    } else {			/* else starting an argument */
	Argument = c - '0';
	DoingArg = 1;
    }
    return (CC_ARGHACK);
}

/*VARARGS*/
CCRETVAL
v_zero(c)			/* command mode 0 for vi */
register Char c;
{
    if (DoingArg) {		/* if doing an arg, add this in... */
	Argument = (Argument * 10) + (c - '0');
	return (CC_ARGHACK);
    } else {			/* else starting an argument */
	Cursor = InputBuf;
	RefCursor();		/* move the cursor */
	return (CC_NORM);
    }
}

/*VARARGS*/
CCRETVAL
e_newline()			/* always ignore argument */
{
    PastBottom();
    *LastChar++ = '\n';		/* for the benifit of CSH */
    *LastChar = '\0';		/* just in case */
    return (CC_NEWLINE);	/* we must do a ResetInLine later */
}

/*VARARGS*/
CCRETVAL
e_send_eof()			/* for when ^D is ONLY send-eof */
{
    PastBottom();
    *LastChar = '\0';		/* just in case */
#ifdef notdef
    ResetInLine();		/* reset the input pointers */
#endif
    return (CC_EOF);
}

/*VARARGS*/
CCRETVAL
e_complete()
{
    *LastChar = '\0';		/* just in case */
    return (CC_COMPLETE);
}

/*VARARGS*/
CCRETVAL
v_cm_complete()
{
    if (Cursor < LastChar)
	Cursor++;
    *LastChar = '\0';		/* just in case */
    return (CC_COMPLETE);
}

/*VARARGS*/
CCRETVAL
e_toggle_hist()
{
    struct Hist *hp;
    int h;

    *LastChar = '\0';		/* just in case */

    if (Hist_num <= 0) {
	return CC_ERROR;
    }

    hp = Histlist.Hnext;
    if (hp == (struct Hist *)0) { /* this is only if no history */
	return (CC_ERROR);
    }

    for (h = 1; h < Hist_num; h++)
	hp = hp->Hnext;

    if (!CurrentHistLit) {
        if (hp->histline) {
            copyn(InputBuf,hp->histline,INBUFSIZ);
            CurrentHistLit = 1;
        } else {
	    return CC_ERROR;
	}
    } else {
        (void) sprlex (InputBuf, &hp->Hlex);
	CurrentHistLit = 0;
    }
    LastChar = InputBuf + Strlen(InputBuf);
    if (LastChar > InputBuf) {
	if (LastChar[-1] == '\n') LastChar--;
	if (LastChar[-1] == ' ') LastChar--;
	if (LastChar < InputBuf) LastChar = InputBuf;
    }
    Cursor = LastChar;

    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_up_hist()
{
    struct Hist *hp;
    int hnumcntr;
    Char beep = 0;

    *LastChar = '\0';		/* just in case */

    if (Hist_num == 0) {	/* save the current buffer away */
	copyn(HistBuf, InputBuf, INBUFSIZ);
	LastHist = HistBuf + (LastChar - InputBuf);
    }

    hp = Histlist.Hnext;
    if (hp == (struct Hist *)0) { /* this is only if no history */
	return (CC_ERROR);
    }

    Hist_num += Argument;
    for (hnumcntr = 1; hnumcntr < Hist_num; hnumcntr++) {
	if ((hp->Hnext) == (struct Hist *)0) {
	    Hist_num = hnumcntr;
	    beep = 1;
	    break;
	}
	hp = hp -> Hnext;
    }

    if (HistLit && hp->histline) {
	copyn(InputBuf,hp->histline,INBUFSIZ);
   	CurrentHistLit = 1;
    } else {
        (void) sprlex (InputBuf, &hp->Hlex);
   	CurrentHistLit = 0;
    }
    LastChar = InputBuf + Strlen(InputBuf);
    if (LastChar > InputBuf) {
	if (LastChar[-1] == '\n') LastChar--;
	if (LastChar[-1] == ' ') LastChar--;
	if (LastChar < InputBuf) LastChar = InputBuf;
    }
    Cursor = LastChar;

    Refresh();
    if (beep)
	return (CC_ERROR);
    else
	return (CC_NORM);		/* was CC_UP_HIST */
}

/*VARARGS*/
CCRETVAL
e_down_hist()
{
    struct Hist *hp;
    int hnumcntr;

    *LastChar = '\0';		/* just in case */

    Hist_num -= Argument;

    if (Hist_num < 0) {
	Hist_num = 0;
	return (CC_ERROR);	/* make it beep */
    }

    if (Hist_num == 0) {	/* if really the current line */
	copyn(InputBuf, HistBuf, INBUFSIZ);
	LastChar = InputBuf + (LastHist - HistBuf);
	Cursor = LastChar;
	return (CC_REFRESH);
    }

    hp = Histlist.Hnext;
    if (hp == (struct Hist *)0)
	return (CC_ERROR);

    for (hnumcntr = 1; hnumcntr < Hist_num; hnumcntr++) {
	if ((hp->Hnext) == (struct Hist *)0) {
	    Hist_num = hnumcntr;
	    return (CC_ERROR);
	}
	hp = hp -> Hnext;
    }

    if (HistLit && hp->histline) {
	copyn(InputBuf,hp->histline,INBUFSIZ);
   	CurrentHistLit = 1;
    } else {
        (void) sprlex (InputBuf, &hp->Hlex);
   	CurrentHistLit = 0;
    }
    LastChar = InputBuf + Strlen(InputBuf);
    if (LastChar > InputBuf) {
	if (LastChar[-1] == '\n') LastChar--;
	if (LastChar[-1] == ' ') LastChar--;
	if (LastChar < InputBuf) LastChar = InputBuf;
    }
    Cursor = LastChar;

    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_up_search_hist()
{
    struct Hist *hp;
    int h;
    Char *cp;
    bool found = 0;

    *LastChar = '\0';		/* just in case */
    if (Hist_num < 0) {
	CSHprintf ("tcsh: e_up_search_hist(): Hist_num < 0; resetting.\n");
	Hist_num = 0;
	return (CC_ERROR);
    }

    if (Hist_num == 0) {
	prefixlen = LastChar - InputBuf;
	copyn(HistBuf, InputBuf, INBUFSIZ);
	LastHist = HistBuf + prefixlen;
    }

    hp = Histlist.Hnext;
    if (hp == (struct Hist *)0)
	return (CC_ERROR);

    /* in case input line has been edited, recalculate prefixlen */
    if ((cp = Strchr(InputBuf, ' ')) == NULL) cp = LastChar;
    if ((h = (cp - InputBuf)) < prefixlen) prefixlen = h;
#ifdef SDEBUG
    CSHprintf("\nHist_num = %d\n", Hist_num);
    CSHprintf("prefixlen = %d\n", prefixlen);
    CSHprintf("prefix = \"%s\"\n", short2str(InputBuf));
#endif

    for (h = 1; h <= Hist_num; h++)
	hp = hp->Hnext;

    while (hp != (struct Hist *)0) {
#ifdef SDEBUG
	CSHprintf("Comparing with \"%s\"\n", short2str((hp->Hlex).next->word));
#endif
	if (Strncmp(InputBuf, (hp->Hlex).next->word, prefixlen) == 0) {
	    found++;
	    break;
	}
	h++;
	hp = hp->Hnext;
    }
    if (!found)
	return (CC_ERROR);

    Hist_num = h;

    if (HistLit && hp->histline) {
	copyn(InputBuf,hp->histline,INBUFSIZ);
   	CurrentHistLit = 1;
    } else {
        (void) sprlex (InputBuf, &hp->Hlex);
   	CurrentHistLit = 0;
    }
    LastChar = InputBuf + Strlen(InputBuf);
    if (LastChar > InputBuf) {
	if (LastChar[-1] == '\n') LastChar--;
	if (LastChar[-1] == ' ') LastChar--;
	if (LastChar < InputBuf) LastChar = InputBuf;
    }
    Cursor = LastChar;

    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_down_search_hist()
{
    struct Hist *hp, *hpt = NULL;
    int h;
    Char *cp;
    bool found = 0;

    *LastChar = '\0';		/* just in case */

    if (Hist_num == 0)
	return (CC_ERROR);

    hp = Histlist.Hnext;
    if (hp == 0)
	return (CC_ERROR);

    /* in case input line has been edited, recalculate prefixlen */
    if ((cp = Strchr(InputBuf, ' ')) == NULL) cp = LastChar;
    if ((h = (cp - InputBuf)) < prefixlen) prefixlen = h;
#ifdef SDEBUG
    CSHprintf("\nHist_num = %d\n", Hist_num);
    CSHprintf("prefixlen = %d\n", prefixlen);
    CSHprintf("prefix = \"%s\"\n", short2str(InputBuf));
#endif
    for (h = 1; h < Hist_num && hp; h++) {
#ifdef SDEBUG
	CSHprintf("Comparing with \"%s\"\n", short2str((hp->Hlex).next->word));
#endif
	if (Strncmp(InputBuf, (hp->Hlex).next->word, prefixlen) == 0) {
	    found = h;
	    hpt = hp;
	}
	hp = hp->Hnext;
    }
    if (!found) {		/* is it the current history number? */
	if (Strncmp(InputBuf, HistBuf, prefixlen) == 0) {
	    copyn(InputBuf, HistBuf, INBUFSIZ);
	    LastChar = InputBuf + (LastHist - HistBuf);
	    Cursor = LastChar;
	    Hist_num = 0;
	    return (CC_REFRESH);
	} else {
	    return (CC_ERROR);
	}
    }

    Hist_num = found;
    hp = hpt;

    if (HistLit && hp->histline) {
	copyn(InputBuf,hp->histline,INBUFSIZ);
   	CurrentHistLit = 1;
    } else {
        (void) sprlex (InputBuf, &hp->Hlex);
   	CurrentHistLit = 0;
    }
    LastChar = InputBuf + Strlen(InputBuf);
    if (LastChar > InputBuf) {
	if (LastChar[-1] == '\n') LastChar--;
	if (LastChar[-1] == ' ') LastChar--;
	if (LastChar < InputBuf) LastChar = InputBuf;
    }
    Cursor = LastChar;

    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_helpme()
{
    PastBottom();
    *LastChar = '\0';		/* just in case */
    return (CC_HELPME);
}

/*VARARGS*/
CCRETVAL
e_correct()
{
    *LastChar = '\0';		/* just in case */
    return (CC_CORRECT);
}

/*VARARGS*/
CCRETVAL
e_correctl()
{
    *LastChar = '\0';		/* just in case */
    return (CC_CORRECT_L);
}

#include "sh.proc.h"

/*VARARGS*/
CCRETVAL
e_run_fg_editor()
{
    register struct process *pp;
    struct process *find_stopped_editor();
    extern bool tellwhat;

    if ((pp = find_stopped_editor()) != PNULL) {
	/* save our editor state so we can restore it */
	tellwhat = 1;
	copyn(WhichBuf, InputBuf, INBUFSIZ);
	LastWhich = WhichBuf + (LastChar - InputBuf);
	CursWhich = WhichBuf + (Cursor - InputBuf);
	HistWhich = Hist_num;
	Hist_num = 0;	/* for the history commands */

	/* put the tty in a sane mode */
	PastBottom();
	(void) Cookedmode();	/* make sure the tty is set up correctly */

	/* do it! */
	fg_a_proc_entry(pp);

	(void) Rawmode();		/* go on */
	Refresh();
	tellwhat = 0;
    }
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_list_choices()
{
    PastBottom();
    *LastChar = '\0';		/* just in case */
    return (CC_LIST_CHOICES);
}

/*VARARGS*/
CCRETVAL
e_list_glob()
{
    PastBottom();
    *LastChar = '\0';		/* just in case */
    return (CC_LIST_GLOB);
}

/*VARARGS*/
CCRETVAL
e_expand_glob()
{
    *LastChar = '\0';		/* just in case */
    return (CC_EXPAND_GLOB);
}

/*VARARGS*/
CCRETVAL
e_expand_vars()
{
    *LastChar = '\0';		/* just in case */
    return (CC_EXPAND_VARS);
}

/*VARARGS*/
CCRETVAL
e_which()			/* do a fast command line which(1) */
{
    PastBottom();
    *LastChar = '\0';		/* just in case */
    return (CC_WHICH);
}

/*VARARGS*/
CCRETVAL
e_last_item()			/* insert the last element of the prev. cmd */
{
    register Char *cp;
    register struct Hist *hp;
    register struct wordent *wp, *firstp;
    register int i;

    if (Argument <= 0)
	return (CC_ERROR);

    hp = Histlist.Hnext;
    if (hp == (struct Hist *)0) { /* this is only if no history */
	return (CC_ERROR);
    }

    wp = (hp->Hlex).prev;

    if (wp->prev == (struct wordent *)NULL)
	return (CC_ERROR);	/* an empty history entry */

    firstp = (hp->Hlex).next;

    for (i = 0; i < Argument; i++) { /* back up arg words in lex */
	wp = wp->prev;
	if (wp == firstp) break;
    }

    while (i > 0) {
	cp = wp->word;

	if (!cp)
	    return (CC_ERROR);

	if (InsertStr(cp))
	    return (CC_ERROR);

	wp = wp->next;
	i--;
    }

    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_yank_kill()			/* almost like GnuEmacs */
{
    register Char *kp, *cp;

    if (LastKill == KillBuf)	/* if zero content */
	return (CC_ERROR);

    if (LastChar + (LastKill-KillBuf) >= InputLim)
	return (CC_ERROR);	/* end of buffer space */

    /* else */
    Mark = Cursor;		/* set the mark */
    cp = Cursor;		/* for speed */

    c_insert(LastKill-KillBuf);	/* open the space, */
    for (kp = KillBuf; kp < LastKill; kp++) /* copy the chars */
	*cp++ = *kp;

    if (Argument == 1)		/* if an arg, cursor at beginning */
	Cursor = cp;		/* else cursor at end */

    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_delprev()
{
    if (Cursor > InputBuf) {
	c_delbefore(Argument);		/* delete before dot */
	Cursor -= Argument;
	if (Cursor < InputBuf) Cursor = InputBuf; /* bounds check */
	return (CC_REFRESH);
    } else {
	return (CC_ERROR);
    }
}

/*VARARGS*/
CCRETVAL
e_delwordprev()
{
    register Char *cp, *p, *kp;

    if (Cursor == InputBuf)
	return (CC_ERROR);
    /* else */
    
    cp = c_prev_word(Cursor, InputBuf, Argument);

    for (p = cp, kp = KillBuf; p < Cursor; p++)	/* save the text */
	*kp++ = *p;
    LastKill = kp;

    c_delbefore(Cursor-cp);		/* delete before dot */
    Cursor = cp;
    if (Cursor < InputBuf)
	Cursor = InputBuf; /* bounds check */
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_delnext()
{
    if (Cursor == LastChar) {		/* if I'm at the end */
	if (Cursor == InputBuf && !VImode) { /* if I'm also at the beginning */
	    so_write (STReof, 4);	/* then do a EOF */
	    flush();
	    return (CC_EOF);
	} else {
	    return (CC_ERROR);
	}
    } else {
	c_delafter(Argument);		/* delete after dot */
	if (Cursor > LastChar) Cursor = LastChar; /* bounds check */
	return (CC_REFRESH);
    }
}

/*VARARGS*/
CCRETVAL
e_list_delnext()
{
    if (Cursor == LastChar) {		/* if I'm at the end */
	if (Cursor == InputBuf) {	/* if I'm also at the beginning */
	    so_write (STReof, 4);	/* then do a EOF */
	    flush();
	    return (CC_EOF);
	} else {
	    PastBottom();
	    *LastChar = '\0';		/* just in case */
	    return (CC_LIST_CHOICES);
	}
    } else {
	c_delafter(Argument);		/* delete after dot */
	if (Cursor > LastChar) Cursor = LastChar; /* bounds check */
	return (CC_REFRESH);
    }
}

CCRETVAL
e_list_eof()
{
    if (Cursor == LastChar && Cursor == InputBuf) {
	so_write (STReof, 4);	/* then do a EOF */
	flush();
	return (CC_EOF);
    } else {
	PastBottom();
	*LastChar = '\0';		/* just in case */
	return (CC_LIST_CHOICES);
    }
}

/*VARARGS*/
CCRETVAL
e_delwordnext()
{
    register Char *cp, *p, *kp;

    if (Cursor == LastChar)
	return (CC_ERROR);
    /* else */
    
    cp = c_next_word(Cursor, LastChar, Argument);

    for (p = Cursor, kp = KillBuf; p < cp; p++)	/* save the text */
	*kp++ = *p;
    LastKill = kp;

    c_delafter(cp-Cursor);		/* delete after dot */
    /* Cursor = Cursor; */
    if (Cursor > LastChar) Cursor = LastChar; /* bounds check */
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_toend()
{
    Cursor = LastChar;
    RefCursor();		/* move the cursor */
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_tobeg()
{
    Cursor = InputBuf;
    RefCursor();		/* move the cursor */
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_killend()
{
    register Char *kp, *cp;

    cp = Cursor;
    kp = KillBuf;
    while (cp < LastChar)
	*kp++ = *cp++;		/* copy it */
    LastKill = kp;
    LastChar = Cursor;		/* zap! -- delete to end */
    return (CC_REFRESH);
}


/*VARARGS*/
CCRETVAL
e_killbeg()
{
    register Char *kp, *cp;

    cp = InputBuf;
    kp = KillBuf;
    while (cp < Cursor)
	*kp++ = *cp++;		/* copy it */
    LastKill = kp;
    c_delbefore(Cursor-InputBuf);
    Cursor = InputBuf;		/* zap! */
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_killall()
{
    register Char *kp, *cp;

    cp = InputBuf;
    kp = KillBuf;
    while (cp < LastChar)
	*kp++ = *cp++;		/* copy it */
    LastKill = kp;
    LastChar = InputBuf;	/* zap! -- delete all of it */
    Cursor = InputBuf;
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_killregion()
{
    register Char *kp, *cp;

    if (!Mark)
	return (CC_ERROR);

    if (Mark > Cursor) {
	cp = Cursor;
	kp = KillBuf;
	while (cp < Mark)
	    *kp++ = *cp++;		/* copy it */
	LastKill = kp;
	c_delafter(cp-Cursor);	/* delete it */
    } else {			/* mark is before cursor */
	cp = Mark;
	kp = KillBuf;
	while (cp < Cursor)
	    *kp++ = *cp++;		/* copy it */
	LastKill = kp;
	c_delbefore(cp-Mark);
	Cursor = Mark;
    }
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_copyregion()
{
    register Char *kp, *cp;

    if (!Mark)
	return (CC_ERROR);

    if (Mark > Cursor) {
	cp = Cursor;
	kp = KillBuf;
	while (cp < Mark)
	    *kp++ = *cp++;		/* copy it */
	LastKill = kp;
    } else {			/* mark is before cursor */
	cp = Mark;
	kp = KillBuf;
	while (cp < Cursor)
	    *kp++ = *cp++;		/* copy it */
	LastKill = kp;
    }
    return (CC_NORM);		/* don't even need to Refresh() */
}

/*VARARGS*/
CCRETVAL
e_charswitch()
{
    register Char c;

    if (Cursor < LastChar) {
	if (LastChar <= &InputBuf[1]) {
	    return (CC_ERROR);
	} else {
	    Cursor++;
	}
    }
    if (Cursor > &InputBuf[1]) { /* must have at least two chars entered */
	c = Cursor[-2];
	Cursor[-2] = Cursor[-1];
	Cursor[-1] = c;
	return (CC_REFRESH);
    } else {
	return (CC_ERROR);
    }
}

/*VARARGS*/
CCRETVAL
e_gcharswitch()			/* gosmacs style ^T */
{
    register Char c;

    if (Cursor > &InputBuf[1]) { /* must have at least two chars entered */
	c = Cursor[-2];
	Cursor[-2] = Cursor[-1];
	Cursor[-1] = c;
	return (CC_REFRESH);
    } else {
	return (CC_ERROR);
    }
}

/*VARARGS*/
CCRETVAL
e_charback()
{
    if (Cursor > InputBuf) {
	Cursor -= Argument;
	if (Cursor < InputBuf) Cursor = InputBuf;
	RefCursor();
	return (CC_NORM);
    } else {
	return (CC_ERROR);
    }
}

/*VARARGS*/
CCRETVAL
e_wordback()
{
    if (Cursor == InputBuf)
	return (CC_ERROR);
    /* else */
    
    Cursor = c_prev_word(Cursor, InputBuf, Argument); /* does a bounds check */

    RefCursor();
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_charfwd()
{
    if (Cursor < LastChar) {
	Cursor += Argument;
	if (Cursor > LastChar) Cursor = LastChar;
	RefCursor();
	return (CC_NORM);
    } else {
	return (CC_ERROR);
    }
}

/*VARARGS*/
CCRETVAL
e_wordfwd()
{
    if (Cursor == LastChar)
	return (CC_ERROR);
    /* else */
    
    Cursor = c_next_word(Cursor, LastChar, Argument);

    RefCursor();
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
v_wordbegnext()
{
    if (Cursor == LastChar)
	return (CC_ERROR);
    /* else */
    
    Cursor = c_beg_next_word(Cursor, LastChar, Argument);

    RefCursor();
    return (CC_NORM);
}

#ifdef COMMENT
/* by: Brian Allison <uiucdcs!convex!allison@RUTGERS.EDU> */
static
get_word (begin, end)
Char ** begin;
Char ** end;
{
    Char * cp;
    cp = &Cursor[0];
    while (Argument--) {
	while ((cp <= LastChar) && (isword(*cp)))
	    cp++;
	*end = --cp;
	while ((cp >= InputBuf) && (isword(*cp)))
	    cp--;
	*begin = ++cp;
    }
}
#endif /* COMMENT */
/*VARARGS*/
CCRETVAL
e_uppercase()
{
    Char *cp, *end;

    end = c_next_word(Cursor, LastChar, Argument);

    for (cp = Cursor; cp < end; cp++) /* PWP: was cp=begin */
        if (islower(*cp))
            *cp = toupper(*cp);

    Cursor = end;
    if (Cursor > LastChar) Cursor = LastChar;
    return (CC_REFRESH);
}


/*VARARGS*/
CCRETVAL
e_capitolcase()
{
    Char *cp, *end;

    end = c_next_word(Cursor, LastChar, Argument);

    cp = Cursor;
    for (; cp < end; cp++) {
        if (isalpha(*cp)) {
	    if (islower(*cp))
		*cp = toupper(*cp);
	    cp++;
	    break;
	}
    }
    for (; cp < end; cp++)
        if (isupper(*cp))
            *cp = tolower(*cp);

    Cursor = end;
    if (Cursor > LastChar) Cursor = LastChar;
    return (CC_REFRESH);
}
    
/*VARARGS*/
CCRETVAL
e_lowercase()
{
    Char *cp, *end;

    end = c_next_word(Cursor, LastChar, Argument);

    for (cp = Cursor; cp < end; cp++)
        if (isupper(*cp))
            *cp = tolower(*cp);

    Cursor = end;
    if (Cursor > LastChar) Cursor = LastChar;
    return (CC_REFRESH);
}


/*VARARGS*/
CCRETVAL
e_set_mark()
{
    Mark = Cursor;
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_exchange_mark()
{
    register Char *cp;

    cp = Cursor;
    Cursor = Mark;
    Mark = cp;
    RefCursor();
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_argfour()			/* multiply current argument by 4 */
{
    DoingArg = 1;
    Argument *= 4;
    return (CC_ARGHACK);
}

/*VARARGS*/
CCRETVAL
e_quote()
{
    Char ch;
    int num;

    QuoteModeOn();
    num = GetNextChar(&ch);
    QuoteModeOff();
    if (num == 1)
        return e_insert(ch);
    else
        return e_send_eof();
}

/*VARARGS*/
CCRETVAL
e_metanext()
{
    MetaNext = 1;
    return (CC_ARGHACK);	/* preserve argument */
}

#ifdef notdef
/*VARARGS*/
CCRETVAL
e_extendnext()
{
    CurrentKeyMap = CcAltMap;
    return (CC_ARGHACK);	/* preserve argument */
}
#endif

/*VARARGS*/
CCRETVAL
v_insbeg()			/* move to beginning of line and start vi insert mode */
{
    Cursor = InputBuf;
    RefCursor();		/* move the cursor */
    c_alternativ_key_map(0);
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
v_replone()			/* vi mode overwrite one character */
{
    c_alternativ_key_map(0);
    replacemode = 2;
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
v_replmode()			/* vi mode start overwriting */
{
    c_alternativ_key_map(0);
    replacemode = 1;
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
v_substchar()			/* vi mode substitute for one char */
{
    c_delafter(Argument);
    c_alternativ_key_map(0);
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
v_substline()			/* vi mode replace whole line */
{
    (void) e_killall();
    c_alternativ_key_map(0);
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
v_chgtoend()			/* vi mode change to end of line */
{
    (void) e_killend();
    c_alternativ_key_map(0);
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
v_insert()			/* vi mode start inserting */
{
    c_alternativ_key_map(0);
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
v_add()				/* vi mode start adding */
{
    c_alternativ_key_map(0);
    if (Cursor < LastChar) {
	Cursor++;
	if (Cursor > LastChar) Cursor = LastChar;
	RefCursor();
    }
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
v_addend()			/* vi mode to add at end of line */
{
    c_alternativ_key_map(0);
    Cursor = LastChar;
    RefCursor();
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
v_change_case ()
{
    char c;

    if (Cursor < LastChar) {
	c = *Cursor;
	if (isupper (c))
	    *Cursor++ = tolower (c);
	else if (islower (c))
	    *Cursor++ = toupper (c);
	else
	    Cursor++;
	RefPlusOne ();			/* fast refresh for one char */
	return (CC_NORM);
    }
    return (CC_ERROR);
}

/*VARARGS*/
CCRETVAL
e_expand ()
{
    register Char *p;
    extern bool justpr;

    for (p = InputBuf; isspace (*p); p++);
    if (p == LastChar)
	return (CC_ERROR);

    justpr++;
    Expand++;
    return (e_newline ());
}

/*VARARGS*/
CCRETVAL
e_startover()			/* erase all of current line, start again */
{
    ResetInLine();		/* reset the input pointers */
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_redisp()
{
    ClearLines();
    ClearDisp();
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_cleardisp()
{
    ClearScreen();		/* clear the whole real screen */
    ClearDisp();		/* reset everything */
    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_tty_int()
{
    /* do no editing */
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_insovr()
{
    replacemode = !replacemode;
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_tty_dsusp()
{
    /* do no editing */
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_tty_flusho()
{
    /* do no editing */
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_tty_quit()
{
    /* do no editing */
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_tty_tsusp()
{
    /* do no editing */
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_tty_stopo()
{
    /* do no editing */
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_expand_history()
{
    *LastChar = '\0';		/* just in case */
    c_substitute();
    return (CC_NORM);
}

/*VARARGS*/
CCRETVAL
e_magic_space()
{
    *LastChar = '\0';		/* just in case */
    c_substitute();
    return (e_insert(' '));
}

/*VARARGS*/
CCRETVAL
e_copyprev()
{
    register Char *cp, *oldc, *dp;

    if (Cursor == InputBuf)
	return (CC_ERROR);
    /* else */
    
    oldc = Cursor;
    cp = c_prev_word(Cursor, InputBuf, Argument); /* does a bounds check */

    c_insert(oldc - cp);
    for (dp = oldc; cp < oldc && dp < LastChar; cp++)
	*dp++ = *cp;

    Cursor = dp;		/* put cursor at end */

    return (CC_REFRESH);
}

/*VARARGS*/
CCRETVAL
e_tty_starto()
{
    /* do no editing */
    return (CC_NORM);
}

#ifdef notdef
MoveCursor(n)			/* move cursor + right - left char */
int n;
{
    Cursor = Cursor + n;
    if (Cursor < InputBuf ) Cursor = InputBuf;
    if (Cursor > LastChar ) Cursor = LastChar;
    return;
}

Char *
GetCursor()
{
    return(Cursor);
}

PutCursor(p)
Char *p;
{
    if (p < InputBuf || p > LastChar) return 1;  /* Error */
    Cursor = p;
    return 0;
}
#endif

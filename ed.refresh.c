/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/ed.refresh.c,v 1.8 1990/11/25 10:11:36 christos Exp $ */
/*
 * ed.refresh.c: Lower level screen refreshing functions
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id: ed.refresh.c,v 1.8 1990/11/25 10:11:36 christos Exp $";
#endif

#include "sh.h"
#include "ed.h"
/* #define DEBUG_UPDATE   */
/* #define DEBUG_REFRESH */
/* #define DEBUG_LITERAL */

/* refresh.c -- refresh the current set of lines on the screen */

Char *litptr[256];

static int vcursor_h, vcursor_v;
static void Draw();
static void Vdraw();
static void Vnewline();
static  void update_line();
static int has_other_than_spaces();
static void str_insert();
static void str_delete();
static void str_cp();
static void PutPlusOne();

static void
Draw(c)					/* draw c, expand tabs, ctl chars */
register Char c;
{
    register Char ch = c & CHAR;

    if (isprint(ch)) {
	Vdraw(c);
	return;
    }
    /* from wolman%crltrx.DEC@decwrl.dec.com (Alec Wolman) */
    if (ch == '\n') {			/* expand the newline	*/
	Vdraw((Char) '\0');		/* assure end of line	*/
	vcursor_h = 0;			/* reset cursor pos	*/
	vcursor_v++;
	return;
    }
    if (ch == '\t') {			/* expand the tab 	*/
	for (;;) {
	    Vdraw((Char) ' ');
	    if ((vcursor_h & 07) == 0)
		break;			/* go until tab stop	*/
	}
    }
    else if (iscntrl(ch)) {
	Vdraw((Char) '^');
	if (ch == '\177') {
	    Vdraw((Char) '?');
	}
	else {
	    /* uncontrolify it; works only for iso8859-1 like sets */
	    Vdraw((Char) (c | 0100));	
	}
    }
    else {
	Vdraw((Char) '\\');
	Vdraw(((c >> 6) & 7) + '0');
	Vdraw(((c >> 3) & 7) + '0');
	Vdraw((c & 7) + '0');
    }
}

static void
Vdraw(c)					/* draw char c onto V lines */
register Char c;
{
#ifdef DEBUG_REFRESH
# ifdef SHORT_STRINGS
    CSHprintf("Vdrawing %6.6o '%c'\r\n", c, c & ASCII);
# else
    CSHprintf("Vdrawing %3.3o '%c'\r\n", c, c);
# endif
#endif

    Vdisplay[vcursor_v][vcursor_h] = c;
    vcursor_h++;		/* advance to next place */
    if (vcursor_h >= TermH) {
	Vdisplay[vcursor_v][TermH] = '\0';	/* assure end of line */
	vcursor_h = 0;		/* reset it. */
	vcursor_v++;
	if (vcursor_v >= TermV) {	/* should NEVER happen. */
#ifdef DEBUG_REFRESH
	    CSHprintf("\r\nVdraw: vcursor_v overflow! Vcursor_v == %d > %d\r\n",
		      vcursor_v, TermV);
	    abort();
#endif /* DEBUG_REFRESH */
	}
    }
}

static void
Vnewline()
{
    /* needs work. */
}

/*
 *  Refresh() 
 *	draws the new virtual screen image from the current input
 *  	line, then goes line-by-line changing the real image to the new 
 *	virtual image. The routine to re-draw a line can be replaced 
 *	easily in hopes of a smarter one being placed there.
 */
static int OldvcV = 0;
void
Refresh()
{
    register int cur_line;
    register Char *cp;
    int     cur_h, cur_v = 0, new_vcv;
    Char    oldgetting;
    int	    litnum = 0;

#ifdef DEBUG_REFRESH
    CSHprintf("PromptBuf = :%s:\r\n", short2str(PromptBuf));
    CSHprintf("InputBuf = :%s:\r\n", short2str(InputBuf));
#endif
    oldgetting = GettingInput;
    GettingInput = 0;		/* avoid re-entrance via SIGWINCH */

    /* reset the Vdraw cursor */
    vcursor_h = 0;
    vcursor_v = 0;

    /* draw prompt, we know it's ASCIZ */
    for (cp = PromptBuf; *cp; cp++) {
	if (*cp & LITERAL) {
	    if (litnum < (sizeof(litptr) / sizeof(litptr[0]))) {
		litptr[litnum] = cp;
#ifdef DEBUG_LITERAL
		CSHprintf("litnum = %d, litptr = %x:\r\n", 
			  litnum, litptr[litnum]);
#endif
	    }
	    while (*cp & LITERAL) cp++;
	    if (*cp) 
		Vdraw(litnum++ | LITERAL);
	    else {
		/* 
		 * XXX: This is a bug, we lose the last literal, if
		 * it is not followed by a normal character, but
		 * it is too hard to fix
		 */
		break;
	    }
	}
	else
	    Draw(*cp);
    }
    cur_h = -1;			/* set flag in case I'm not set */

    /* draw the current input buffer */
    for (cp = InputBuf; (cp < LastChar); cp++) {
	if (cp == Cursor) {
	    cur_h = vcursor_h;	/* save for later */
	    cur_v = vcursor_v;
	}
	Draw(*cp);
    }

    /* to next line and draw the current search prompt if searching */
    if (DoingSearch) {
	Vnewline();
	for (cp = SearchPrompt; *cp; cp++)
	    Draw(*cp);
	for (cp = InputBuf; (cp < LastChar); cp++) {
	    if (cp == Cursor) {
		cur_h = vcursor_h;	/* save for later */
		cur_v = vcursor_v;
	    }
	    Draw(*cp);
	}
    }

    if (cur_h == -1) {		/* if I havn't been set yet, I'm at the end */
	cur_h = vcursor_h;
	cur_v = vcursor_v;
    }
    new_vcv = vcursor_v;	/* must be done BEFORE the NUL is written */
    Vdraw((Char) '\0');		/* put NUL on end */

#ifdef DEBUG_REFRESH
    CSHprintf(
    "TermH=%d, vcursor_h=%d, vcursor_v=%d, Vdisplay[0]=\r\n:%80.80s:\r\n",
	      TermH, vcursor_h, vcursor_v, short2str(Vdisplay[0]));
#endif

    for (cur_line = 0; cur_line <= new_vcv; cur_line++) {
	/* NOTE THAT update_line MAY CHANGE Display[cur_line] */
	update_line(Display[cur_line], Vdisplay[cur_line], cur_line);
	(void) Strncpy(Display[cur_line], Vdisplay[cur_line], TermH);
	Display[cur_line][TermH] = '\0';	/* just in case */
    }
#ifdef DEBUG_REFRESH
    CSHprintf("\r\nvcursor_v = %d, OldvcV = %d, cur_line = %d\r\n",
	      vcursor_v, OldvcV, cur_line);
#endif
    if (OldvcV > new_vcv) {
	for (; cur_line <= OldvcV; cur_line++) {
	    MoveToLine(cur_line);
	    MoveToChar(0);
	    ClearEOL(Strlen(Display[cur_line]));
#ifdef DEBUG_REFRESH
	    so_write(str2short("C\b"), 2);
#endif
	    *Display[cur_line] = '\0';
	}
    }
    OldvcV = new_vcv;		/* set for next time */
#ifdef DEBUG_REFRESH
    CSHprintf("\r\nCursorH = %d, CursorV = %d, cur_h = %d, cur_v = %d\r\n",
	      CursorH, CursorV, cur_h, cur_v);
#endif
    MoveToLine(cur_v);		/* go to where the cursor is */
    MoveToChar(cur_h);
    SetAttributes(0);		/* Clear all attributes */
    flush();			/* send the output... */
    GettingInput = oldgetting;	/* reset to old value */
}

#ifdef notdef
GotoBottom()
{				/* used to go to last used screen line */
    MoveToLine(OldvcV);
}

#endif

void
PastBottom()
{				/* used to go to last used screen line */
    MoveToLine(OldvcV);
    (void) putraw('\r');
    (void) putraw('\n');
    ClearDisp();
    flush();
}

static int
has_other_than_spaces(s, n)
register Char *s;
register int n;
{
    if (n <= 0)
	return 0;

    while (n-- > 0) {
	if (*s != '\0' && *s != ' ')
	    return 1;
	s++;
    }
    return 0;
}

/* insert num characters of s into d (in front of the character) at dat,
   maximum length of d is dlen */
static void
str_insert(d, dat, dlen, s, num)
register Char *d, *s;
register int dat, dlen, num;
{
    register Char *a,
           *b;

    if (num <= 0)
	return;
    if (num > dlen - dat)
	num = dlen - dat;

#ifdef DEBUG_REFRESH
    CSHprintf("str_insert() starting: %d at %d max %d, d == \"%s\"\n",
	      num, dat, dlen, short2str(d));
    CSHprintf("s == \"%s\"n", short2str(s));
#endif

    /* open up the space for num chars */
    if (num > 0) {
	b = d + dlen - 1;
	a = b - num;
	while (a >= &d[dat])
	    *b-- = *a--;
	d[dlen] = '\0';		/* just in case */
    }
#ifdef DEBUG_REFRESH
    CSHprintf("str_insert() after insert: %d at %d max %d, d == \"%s\"\n",
	      num, dat, dlen, short2str(d));
    CSHprintf("s == \"%s\"n", short2str(s));
#endif

    /* copy the characters */
    for (a = d + dat; (a < d + dlen) && (num > 0); num--)
	*a++ = *s++;

#ifdef DEBUG_REFRESH
    CSHprintf("str_insert() after copy: %d at %d max %d, d == \"%s\"\n",
	      num, dat, dlen, d, short2str(s));
    CSHprintf("s == \"%s\"n", short2str(s));
#endif
}

/* delete num characters d at dat, maximum length of d is dlen */
static void
str_delete(d, dat, dlen, num)
register Char *d;
register int dat, dlen, num;
{
    register Char *a, *b;

    if (num <= 0)
	return;
    if (dat + num >= dlen) {
	d[dat] = '\0';
	return;
    }

#ifdef DEBUG_REFRESH
    CSHprintf("str_delete() starting: %d at %d max %d, d == \"%s\"\n",
	      num, dat, dlen, short2str(d));
#endif

    /* open up the space for num chars */
    if (num > 0) {
	b = d + dat;
	a = b + num;
	while (a < &d[dlen])
	    *b++ = *a++;
	d[dlen] = '\0';		/* just in case */
    }
#ifdef DEBUG_REFRESH
    CSHprintf("str_delete() after delete: %d at %d max %d, d == \"%s\"\n",
	      num, dat, dlen, short2str(d));
#endif
}

static void
str_cp(a, b, n)
register Char *a, *b;
register int n;
{
    while (n-- && *b)
	*a++ = *b++;
}



/* ****************************************************************
    update_line() is based on finding the middle difference of each line
    on the screen; vis:


			     /old first difference
	/beginning of line   |              /old last same       /old EOL
	v		     v              v                    v
old:	eddie> Oh, my little gruntle-buggy is to me, as lurgid as
new:	eddie> Oh, my little buggy says to me, as lurgid as
	^		     ^        ^			   ^
	\beginning of line   |        \new last same	   \new end of line
			     \new first difference

    all are character pointers for the sake of speed.  Special cases for
    no differences, as well as for end of line additions must be handled.
**************************************************************** */

/* Minimum at which doing an insert it "worth it".  This should be about
 * half the "cost" of going into insert mode, inserting a character, and
 * going back out.  This should really be calculated from the termcap
 * data...  For the moment, a good number for ANSI terminals.
 */
#define MIN_END_KEEP	4

static void			/* could be changed to make it smarter */
update_line(old, new, cur_line)
register Char *old, *new;
int     cur_line;
{
    register Char *o, *n, *p, c;
    Char   *ofd, *ols, *oe, *nfd, *nls, *ne;
    Char   *osb, *ose, *nsb, *nse; 
    int     fx, sx;

    /*
     * find first diff
     */
    for (o = old, n = new; *o && (*o == *n); o++, n++);
    ofd = o;
    nfd = n;

    /*
     * if no diff, continue to next line
     */
    if (*ofd == '\0' && *nfd == '\0') {
#ifdef DEBUG_UPDATE
	DEBUGPRINT("no difference.\r\n", 0);
#endif /* DEBUG_UPDATE */
	return;
    }

    /*
     * Find the end of both old and new
     */
    while (*o) o++;
    oe = o;

    while (*n) n++;
    ne = n;

    /*
     * find last same *
     */
    for (;(o > ofd) && (n > nfd) && (o[-1] == n[-1]); o--, n--);
    ols = o;
    nls = n;

    /*
     * find same begining and same end
     */
    osb = ols;
    nsb = nls;
    ose = ols;
    nse = nls;

    /*
     * case 1: insert: scan from nfd to nls looking for *ofd
     */
    if (*ofd) {
	for (c = *ofd, n = nfd; n < nls; n++) {
	    if (c == *n) {
		for (o = ofd, p = n; p < nls && o < ols && *o == *p; o++, p++);
		/*
		 * if the new match is longer and it's worth keeping, then we
		 * take it
		 */
		if (((nse - nsb) < (p - n)) && (2 * (p - n) > n - nfd)) {
		    nsb = n;
		    nse = p;
		    osb = ofd;
		    ose = o;
		}
	    }
	}
    }

    /*
     * case 2: delete: scan from ofd to ols looking for *nfd 
     */
    if (*nfd) {
	for (c = *nfd, o = ofd; o < ols; o++) {
	    if (c == *o) {
		for (n = nfd, p = o; p < ols && n < nls && *p == *n; p++, n++);
		/*
		 * if the new match is longer and it's worth keeping, then we
		 * take it
		 */
		if (((ose - osb) < (p - o)) && (2 * (p - o) > o - ofd)) {
		    nsb = nfd;
		    nse = n;
		    osb = o;
		    ose = p;
		}
	    }
	}
    }

    /*
     * Pragmatics I: If old trailing whitespace or not enough characters to
     * save to be worth it, then don't save the last same info.
     */
    if ((oe - ols) < MIN_END_KEEP) {
	ols = oe;
	nls = ne;
    }

    /*
     * Pragmatics II: if the terminal isn't smart enough, make the data dumber
     * so the smart update doesn't try anything fancy
     */

    /*
     * fx is the number of characters we need to insert/delete: in the
     * beginning to bring the two same begins together
     */
    fx = (nsb - nfd) - (osb - ofd);
    /*
     * sx is the number of characters we need to insert/delete: in the end to
     * bring the two same last parts together
     */
    sx = (nls - nse) - (ols - ose);

    if (!T_CanIns) {
	if (fx > 0) {
	    osb = ols;
	    ose = ols;
	    nsb = nls;
	    nse = nls;
	}
	if (sx > 0) {
	    ols = oe;
	    nls = ne;
	}
	if ((ols - ofd) < (nls - nfd)) {
	    ols = oe;
	    nls = ne;
	}
    }
    if (!T_CanDel) {
	if (fx < 0) {
	    osb = ols;
	    ose = ols;
	    nsb = nls;
	    nse = nls;
	}
	if (sx < 0) {
	    ols = oe;
	    nls = ne;
	}
	if ((ols - ofd) > (nls - nfd)) {
	    ols = oe;
	    nls = ne;
	}
    }

    /*
     * Pragmatics III: make sure the middle shifted pointers are correct if
     * they don't point to anything (we may have moved ols or nls).
     */
    if (osb == ose) {
	osb = ols;
	ose = ols;
	nsb = nls;
	nse = nls;
    }

    /*
     * Now that we are done with pragmatics we recompute fx, sx
     */
    fx = (nsb - nfd) - (osb - ofd);
    sx = (nls - nse) - (ols - ose);

#ifdef DEBUG_UPDATE
    CSHprintf("\n");
    CSHprintf("ofd %d, osb %d, ose %d, ols %d, oe %d\n",
	      ofd - old, osb - old, ose - old, ols - old, oe - old);
    CSHprintf("nfd %d, nsb %d, nse %d, nls %d, ne %d\n",
	      nfd - new, nsb - new, nse - new, nls - new, ne - new);
    CSHprintf("old:\"%s\"\r\n", short2str(old));
    CSHprintf("new:\"%s\"\r\n", short2str(new));
    CSHprintf("ofd:\"%s\"\r\n", short2str(ofd));
    CSHprintf("nfd:\"%s\"\r\n", short2str(nfd));
    CSHprintf("ols:\"%s\"\r\n", short2str(ols));
    CSHprintf("nls:\"%s\"\r\n", short2str(nls));
    CSHprintf("*oe:%o,*ne:%o\r\n", *oe, *ne);
    CSHprintf("osb:\"%s\"\r\n", short2str(osb));
    CSHprintf("ose:\"%s\"\r\n", short2str(ose));
    CSHprintf("nsb:\"%s\"\r\n", short2str(nsb));
    CSHprintf("nse:\"%s\"\r\n", short2str(nse));
#endif /* DEBUG_UPDATE */

    /*
     * CursorV to this line cur_line MUST be in this routine so that if we
     * don't have to change the line, we don't move to it. CursorH to first
     * diff char
     */
    MoveToLine(cur_line);

    /*
     * at this point we have something like this:
     * 
     * /old                  /ofd    /osb               /ose    /ols     /oe
     * v.....................v       v..................v       v........v
     * eddie> Oh, my fredded gruntle-buggy is to me, as foo var lurgid as
     * eddie> Oh, my fredded quiux buggy is to me, as gruntle-lurgid as
     * ^.....................^     ^..................^       ^........^ 
     * \new                  \nfd  \nsb               \nse    \nls     \ne
     * 
     */

    /*
     * if we have a net insert on the first difference, AND inserting the net
     * amount ((nsb-nfd) - (osb-ofd)) won't push the last useful character
     * (which is ne if nls != ne, otherwise is nse) off the edge of the screen
     * (TermH) else we do the deletes first so that we keep everything we need
     * to.
     */

    /*
     * if the last same is the same like the end, there is no last same part,
     * otherwise we want to keep the last same part set p to the last useful
     * old character
     */
    p = (ols != oe) ? oe : ose;

    /*
     * if (There is a diffence in the beginning) && (we need to insert
     * characters) && (the number of characters to insert is less than the term
     * width) We need to do an insert! else if (we need to delete characters)
     * We need to delete characters! else No insert or delete
     */
    if ((nsb != nfd) && fx > 0 && ((p - old) + fx <= TermH)) {
#ifdef DEBUG_UPDATE
	DEBUGPRINT("\nfirst diff insert at %d...\n", nfd - new);
#endif /* DEBUG_UPDATE */
	/*
	 * Move to the first char to insert, where the first diff is.
	 */
	MoveToChar(nfd - new);
	/*
	 * Check if we have stuff to keep at end
	 */
	if (nsb != ne) {
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nwith stuff to keep at end\n", 0);
#endif /* DEBUG_UPDATE */
	    /*
	     * insert fx chars of new starting at nfd
	     */
	    if (fx > 0) {
#ifdef DEBUG_UPDATE
		if (!T_CanIns) 
		    CSHprintf("   ERROR: cannot insert in early first diff\n");
#endif /* DEBUG_UPDATE */
		Insert_write(nfd, fx);
		str_insert(old, ofd - old, TermH, nfd, fx);
	    }
	    /* 
	     * write (nsb-nfd) - fx chars of new starting at (nfd + fx) 
	     */
	    so_write(nfd + fx, (nsb - nfd) - fx);
	    str_cp(ofd + fx, nfd + fx, (nsb - nfd) - fx);
	}
	else {
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nwithout anything to save\n", 0);
#endif /* DEBUG_UPDATE */
	    so_write(nfd, (nsb - nfd));
	    str_cp(ofd, nfd, (nsb - nfd));
	    /*
	     * Done
	     */
	    return;
	}
    }
    else if (fx < 0) {
#ifdef DEBUG_UPDATE
	DEBUGPRINT("\nfirst diff delete at %d...\n", ofd - old);
#endif /* DEBUG_UPDATE */
	/*
	 * move to the first char to delete where the first diff is
	 */
	MoveToChar(ofd - old);
	/*
	 * Check if we have stuff to save
	 */
	if (osb != oe) {	
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nwith stuff to save at end\n", 0);
#endif /* DEBUG_UPDATE */
	    /*
	     * fx is less than zero *always* here but we check for code
	     * symmetry
	     */
	    if (fx < 0) {
		if (!T_CanDel)
		    CSHprintf("   ERROR: cannot delete in first diff\n");
		DeleteChars(-fx);	
		str_delete(old, ofd - old, TermH, -fx);	
	    }
	    /* 
	     * write (nsb-nfd) chars of new starting at nfd 
	     */
	    so_write(nfd, (nsb - nfd));
	    str_cp(ofd, nfd, (nsb - nfd));

	}
	else {
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nbut with nothing left to save\n", 0);
#endif /* DEBUG_UPDATE */
	    /* 
	     * write (nsb-nfd) chars of new starting at nfd 
	     */
	    so_write(nfd, (nsb - nfd));
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("cleareol %d\n", (oe - old) - (ne - new));
#endif /* DEBUG_UPDATE */
	    ClearEOL((oe - old) - (ne - new));
	    /*
	     * Done
	     */
	    return;
	}
    }
    else
	fx = 0;

    if (sx < 0) {		
#ifdef DEBUG_UPDATE
	DEBUGPRINT("\nsecond diff delete at %d...\n", (ose - old) + fx);
#endif /* DEBUG_UPDATE */
	/*
	 * Check if we have stuff to delete
	 */
	/* 
	 * fx is the number of characters inserted (+) or deleted (-) 
	 */
	MoveToChar((ose - old) + fx);
	if (ols != oe) {
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nwith stuff to save at end\n", 0);
#endif /* DEBUG_UPDATE */
	    /*
	     * Again a duplicate test.
	     */
	    if (sx < 0) {
#ifdef DEBUG_UPDATE
		if (!T_CanDel) 
		   CSHprintf("   ERROR: cannot delete in second diff\n");
#endif /* DEBUG_UPDATE */
		DeleteChars(-sx);
	    }

	    /* 
	     * write (nls-nse) chars of new starting at nse 
	     */
	    so_write(nse, (nls - nse));
	}
	else {
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nbut with nothing left to save\n", 0);
#endif /* DEBUG_UPDATE */
	    so_write(nse, (nls - nse));
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("cleareol %d\n", (oe - old) - (ne - new));
#endif /* DEBUG_UPDATE */
	    ClearEOL((oe - old) - (ne - new));
	}
    }

    /*
     * if we have a first insert AND WE HAVEN'T ALREADY DONE IT...
     */
    if ((nsb != nfd) && (osb - ofd) <= (nsb - nfd) && (fx == 0)) {
#ifdef DEBUG_UPDATE
	DEBUGPRINT("\nlate first diff insert at %d...\n", nfd - new);
#endif /* DEBUG_UPDATE */
	MoveToChar(nfd - new);
	/*
	 * Check if we have stuff to keep at the end
	 */
	if (nsb != ne) {
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nwith stuff to keep at end\n", 0);
#endif /* DEBUG_UPDATE */
	    if (fx > 0) {
		/* 
		 * insert fx chars of new starting at nfd 
		 */
#ifdef DEBUG_UPDATE
		if (!T_CanIns)
		    CSHprintf("   ERROR: cannot insert in late first diff\n");
#endif /* DEBUG_UPDATE */
		Insert_write(nfd, fx);
		str_insert(old, ofd - old, TermH, nfd, fx);
	    }

	    /* 
	     * write (nsb-nfd) - fx chars of new starting at (nfd + fx) 
	     */
	    so_write(nfd + fx, (nsb - nfd) - fx);
	    str_cp(ofd + fx, nfd + fx, (nsb - nfd) - fx);
	}
	else {
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nwithout anything to save\n", 0);
#endif /* DEBUG_UPDATE */
	    so_write(nfd, (nsb - nfd));
	    str_cp(ofd, nfd, (nsb - nfd));
	}
    }

    /*
     * line is now NEW up to nse
     */
    if ((nls != nse) && has_other_than_spaces(nse, (nls - nse)) && sx >= 0) {
#ifdef DEBUG_UPDATE
	DEBUGPRINT("\nsecond diff insert at %d...\n", nse - new);
#endif /* DEBUG_UPDATE */
	MoveToChar(nse - new);
	if (nls != ne) {
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nwith stuff to keep at end\n", 0);
#endif /* DEBUG_UPDATE */
	    if (sx > 0) {
		/* insert sx chars of new starting at nse */
#ifdef DEBUG_UPDATE
		if (!T_CanIns) 
		   CSHprintf("   ERROR: cannot insert in second diff\n");
#endif /* DEBUG_UPDATE */
		Insert_write(nse, sx);
	    }

	    /*
	     * write (nls-nse) - sx chars of new starting at (nse + sx) 
	     */
	    so_write(nse + sx, (nls - nse) - sx);
	}
	else {
#ifdef DEBUG_UPDATE
	    DEBUGPRINT("\nwithout anything to save\n", 0);
#endif /* DEBUG_UPDATE */
	    so_write(nse, (nls - nse));
	}
    }
#ifdef DEBUG_UPDATE
    DEBUGPRINT("\ndone.\n", 0);
#endif /* DEBUG_UPDATE */
}

void
RefCursor()
{				/* only move to new cursor pos */
    register Char *cp,
            c;
    register int h,
            th,
            v;

    if (DoingSearch) {
	Refresh();
	return;
    }

    /* first we must find where the cursor is... */
    h = 0;
    v = 0;
    th = TermH;			/* optimize for speed */

    for (cp = PromptBuf; *cp; cp++) {	/* do prompt */
	if (*cp & LITERAL)
	    continue;
	c = *cp & CHAR;		/* extra speed plus strip the inverse */
	h++;			/* all chars at least this long */

	/* from wolman%crltrx.DEC@decwrl.dec.com (Alec Wolman) */
	/* lets handle newline as part of the prompt */

	if (c == '\n') {
	    h = 0;
	    v++;
	}
	else {
	    if (c == '\t') {	/* if a tab, to next tab stop */
		while (h & 07) {
		    h++;
		}
	    }
	    else if (iscntrl(c)) {	/* if control char */
		h++;
		if (h > th) {	/* if overflow, compensate */
		    h = 1;
		    v++;
		}
	    }
	    else if (!isprint(c)) {
		h += 3;
		if (h > th) {	/* if overflow, compensate */
		    h = h - th;
		    v++;
		}
	    }
	}

	if (h >= th) {		/* check, extra long tabs picked up here also */
	    h = 0;
	    v++;
	}
    }

    for (cp = InputBuf; cp < Cursor; cp++) {	/* do input buffer to Cursor */
	c = *cp & CHAR;		/* extra speed plus strip the inverse */
	h++;			/* all chars at least this long */

	if (c == '\n') {	/* handle newline in data part too */
	    h = 0;
	    v++;
	}
	else {
	    if (c == '\t') {	/* if a tab, to next tab stop */
		while (h & 07) {
		    h++;
		}
	    }
	    else if (iscntrl(c)) {	/* if control char */
		h++;
		if (h > th) {	/* if overflow, compensate */
		    h = 1;
		    v++;
		}
	    }
	    else if (!isprint(c)) {
		h += 3;
		if (h > th) {	/* if overflow, compensate */
		    h = h - th;
		    v++;
		}
	    }
	}

	if (h >= th) {		/* check, extra long tabs picked up here also */
	    h = 0;
	    v++;
	}
    }

    /* now go there */
    MoveToLine(v);
    MoveToChar(h);
    flush();
}

static void
PutPlusOne(c)
Char    c;
{
    (void) putraw(c);
    Display[CursorV][CursorH++] = c;
    if (CursorH >= TermH) {	/* if we must overflow */
	CursorH = 0;
	CursorV++;
	OldvcV++;
	(void) putraw('\r');
	(void) putraw('\n');
    }
}

void
RefPlusOne()
{				/* we added just one char, handle it fast *//* a
				 * ssumes that screen cursor == real cursor */
    register Char c,
            mc;

    c = Cursor[-1] & CHAR;	/* the char we just added */

    if (c == '\t' || Cursor != LastChar) {
	Refresh();		/* too hard to handle */
	return;
    }				/* else (only do at end of line, no TAB) */

    if (DoingSearch) {
	Refresh();
	return;
    }

    if (iscntrl(c)) {		/* if control char, do caret */
	mc = (c == '\177') ? '?' : (c | 0100);
	PutPlusOne('^');
	PutPlusOne(mc);
    }
    else if (isprint(c)) {	/* normal char */
	PutPlusOne(c);
    }
    else {
	PutPlusOne('\\');
	PutPlusOne(((c >> 6) & 7) + '0');
	PutPlusOne(((c >> 3) & 7) + '0');
	PutPlusOne((c & 7) + '0');
    }
    flush();
}

/* clear the screen buffers so that new new prompt starts fresh. */

void
ClearDisp()
{
    register int i;

    CursorV = 0;		/* clear the display buffer */
    CursorH = 0;
    for (i = 0; i < TermV; i++)
	Display[i][0] = '\0';
    OldvcV = 0;
}

void
ClearLines()
{				/* Make sure all lines are *really* blank */
    register int i;

    if (T_CanCEOL) {
	MoveToChar(0);
	for (i = 0; i <= OldvcV; i++) {	/* for each line on the screen */
	    MoveToLine(i);
	    ClearEOL(TermH);
	}
	MoveToLine(0);
    }
    else {
	MoveToLine(OldvcV);	/* go to last line */
	(void) putraw('\r');	/* go to BOL */
	(void) putraw('\n');	/* go to new line */
    }
}

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/ed.screen.c,v 1.19 1991/03/05 20:17:51 christos Exp $ */
/*
 * ed.screen.c: Editor/termcap-curses interface
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id: ed.screen.c,v 1.19 1991/03/05 20:17:51 christos Exp $";
#endif

#include "sh.h"
#include "ed.h"
#include "ed.defns.h"

extern char *tgoto();
extern char *tgetstr();
extern char *tputs();
extern int tgetent();
extern int tgetflag();
extern int tgetnum();

static void ReBufferDisplay();

/* #define DEBUG_LITERAL */

/*
 * IMPORTANT NOTE: these routines are allowed to look at the current screen
 * and the current possition assuming that it is correct.  If this is not
 * true, then the update will be WRONG!  This is (should be) a valid
 * assumption...
 */

#define TC_BUFSIZ 2048

#define GoodStr(a) (tstr[a].str != (char *) 0 && tstr[a].str[0] != '\0') 
#define Str(a) tstr[a].str
#define Val(a) tval[a].val

static struct {
    char   *b_name;
    int     b_rate;
} baud_rate[] = {
#ifdef B0
    { "0", B0 },
#endif 
#ifdef B50
    { "50", B50 },
#endif 
#ifdef B75
    { "75", B75 },
#endif
#ifdef B110
    { "110", B110 },
#endif
#ifdef B134
    { "134", B134 },
#endif
#ifdef B150
    { "150", B150 },
#endif
#ifdef B200
    { "200", B200 },
#endif
#ifdef B300
    { "300", B300 },
#endif
#ifdef B600
    { "600", B600 },
#endif
#ifdef B900
    { "900", B900 },
#endif
#ifdef B1200
    { "1200", B1200 },
#endif
#ifdef B1800
    { "1800", B1800 },
#endif
#ifdef B2400
    { "2400", B2400 },
#endif
#ifdef B3600
    { "3600", B3600 },
#endif
#ifdef B4800
    { "4800", B4800 },
#endif
#ifdef B7200
    { "7200", B7200 },
#endif
#ifdef B9600
    { "9600", B9600 },
#endif
#ifdef EXTA
    { "19200", EXTA },
#endif
#ifdef B19200
    { "19200", B19200 },
#endif
#ifdef EXTB
    { "38400", EXTB },
#endif
#ifdef B38400
    { "38400", B38400 },
#endif
    { (char *) 0, 0 }
};

static struct termcapstr {
    char   *name;
    char   *long_name;
    char   *str;
} tstr[] = {
#define T_al	0
    { "al", "add new blank line", (char *) 0 },
#define T_bl	1
    { "bl", "audible bell", (char *) 0 },
#define T_cd	2
    { "cd", "clear to bottom", (char *) 0 },
#define T_ce	3
    { "ce", "clear to end of line", (char *) 0 },
#define T_ch	4
    { "ch", "cursor to horiz pos", (char *) 0 },
#define T_cl	5
    { "cl", "clear screen", (char *) 0 },
#define	T_dc	6
    { "dc", "delete a character", (char *) 0 },
#define	T_dl	7
    { "dl", "delete a line", (char *) 0 },
#define	T_dm	8
    { "dm", "start delete mode", (char *) 0 },
#define	T_ed	9
    { "ed", "end delete mode", (char *) 0 },
#define	T_ei	10
    { "ei", "end insert mode", (char *) 0 },
#define	T_fs	11
    { "fs", "cursor from status line", (char *) 0 },
#define	T_ho	12
    { "ho", "home cursor", (char *) 0 },
#define	T_ic	13
    { "ic", "insert character", (char *) 0 },
#define	T_im	14
    { "im", "start insert mode", (char *) 0 },
#define	T_ip	15
    { "ip", "insert padding", (char *) 0 },
#define	T_kd	16
    { "kd", "sends cursor down", (char *) 0 },
#define	T_kl	17
    { "kl", "sends cursor left", (char *) 0 },
#define T_kr	18
    { "kr", "sends cursor right", (char *) 0 },
#define T_ku	19
    { "ku", "sends cursor up", (char *) 0 },
#define T_md	20
    { "md", "begin bold", (char *) 0 },
#define T_me	21
    { "me", "end attributes", (char *) 0 },
#define T_nd	22
    { "nd", "non destructive space", (char *) 0 },
#define T_se	23
    { "se", "end standout", (char *) 0 },
#define T_so	24
    { "so", "begin standout", (char *) 0 },
#define T_ts	25
    { "ts", "cursor to status line", (char *) 0 },
#define T_up	26
    { "up", "cursor up one", (char *) 0 },
#define T_us	27
    { "us", "begin underline", (char *) 0 },
#define T_ue	28
    { "ue", "end underline", (char *) 0 },
#define T_vb	29
    { "vb", "visible bell", (char *) 0 },
#define T_DC	30
    { "DC", "delete multiple chars", (char *) 0 },
#define T_DO	31
    { "DO", "cursor down multiple", (char *) 0 },
#define T_IC	32
    { "IC", "insert multiple chars", (char *) 0 },
#define T_LE	33
    { "LE", "cursor left multiple", (char *) 0 },
#define T_RI	34
    { "RI", "cursor right multiple", (char *) 0 },
#define T_UP	35
    { "UP", "cursor up multiple", (char *) 0 },
#define T_str	36
    { (char *) 0, (char *) 0, (char *) 0 }
};

static struct termcapval {
    char   *name;
    char   *long_name;
    int     val;
}  tval[] = {
#define T_pt	0
    { "pt", "can use physical tabs", 0 },
#define T_li	1
    { "li", "Number of lines", 0 },
#define T_co	2
    { "co", "Number of columns", 0 },
#define T_km	3
    { "km", "Has meta key", 0 },
#define T_val	4
    { (char *) 0, (char *) 0, 0 }
};

static bool me_all = 0;		/* does two or more of the attributes use me */


static void
TCalloc(t, cap)
struct termcapstr *t;
char *cap;
{
    static char termcap_alloc[TC_BUFSIZ];
    char termbuf[TC_BUFSIZ];
    struct termcapstr *ts;
    static int tloc = 0;
    int tlen, clen;

    if (cap == (char *) 0 || *cap == '\0') {
	t->str = (char *) 0;
	return;
    }
    else
	clen = strlen(cap);
	
    if (t->str == (char *) 0) 
	tlen = 0;
    else
	tlen = strlen(t->str);

    /*
     * New string is shorter; no need to allocate space
     */
    if (clen <= tlen) {
	(void) strcpy(t->str, cap);
	return;
    }

    /*
     * New string is longer; see if we have enough space 
     * to append
     */
    if (tloc + 3 < TC_BUFSIZ) {
	(void) strcpy(t->str = &termcap_alloc[tloc], cap);
	tloc += clen + 1;	/* one for \0 */
	return;
    }

    /*
     * Compact our buffer; no need to check compaction, cause we 
     * know it fits...
     */
    tlen = 0;
    for (ts = tstr; ts->name != (char *) 0; ts++)
	if (t != ts && ts->str != (char *) 0 && ts->str[0] != '\0') {
	    char *ptr;

	    for (ptr = ts->str; *ptr != '\0'; termbuf[tlen++] = *ptr++);
	    termbuf[tlen++] = '\0';
	}
    copy(termcap_alloc, termbuf, TC_BUFSIZ);
    tloc = tlen;
    if (tloc + 3 >= TC_BUFSIZ) {
	error("settc: Out of termcap string space");
	return;
    }
    (void) strcpy(t->str = &termcap_alloc[tloc], cap);
    tloc += clen + 1;	/* one for \0 */
    return;
} 


/*ARGSUSED*/
void
TellTC(what)
char   *what;
{
    struct termcapstr *t;

    CSHprintf("\n\tTcsh thinks your terminal has the\n");
    CSHprintf("\tfollowing characteristics:\n\n");
    CSHprintf("\tIt has %d columns and %d lines\n",
	      Val(T_co), Val(T_li));
    CSHprintf("\tIt has %s meta key\n", T_HasMeta ? "a" : "no");
    CSHprintf("\tIt can%suse tabs\n", T_Tabs ? " " : "not ");

    for (t = tstr; t->name != (char *) 0; t++)
	CSHprintf("\t%25s (%s) == %s\n", t->long_name, t->name,
		  t->str && *t->str ? t->str : "(empty)");
    CSHprintf("\n");
}


static void
ReBufferDisplay()
{
    register int i;
    Char  **b;
    Char  **bufp;

    b = Display;
    Display = NULL;
    if (b != NULL) {
	for (bufp = b; *bufp != NULL; bufp++)
	    xfree((ptr_t) * bufp);
	xfree((ptr_t) b);
    }
    b = Vdisplay;
    Vdisplay = NULL;
    if (b != NULL) {
	for (bufp = b; *bufp != NULL; bufp++)
	    xfree((ptr_t) * bufp);
	xfree((ptr_t) b);
    }
    /* make this public, -1 to avoid wraps */
    TermH = Val(T_co) - 1;
    TermV = (INBUFSIZ * 4) / TermH + 1;
    b = (Char **) xalloc((size_t) (sizeof(Char *) * (TermV + 1)));
    for (i = 0; i < TermV; i++)
	b[i] = (Char *) xalloc((size_t) (sizeof(Char) * (TermH + 1)));
    b[TermV] = NULL;
    Display = b;
    b = (Char **) xalloc((size_t) (sizeof(Char *) * (TermV + 1)));
    for (i = 0; i < TermV; i++)
	b[i] = (Char *) xalloc((size_t) (sizeof(Char) * (TermH + 1)));
    b[TermV] = NULL;
    Vdisplay = b;
}

void
SetTC(what, how)
char   *what, *how;
{
    struct termcapstr *ts;
    struct termcapval *tv;

    /*
     * Do the strings first
     */
    for (ts = tstr; ts->name != (char *) 0; ts++)
	if (strcmp(ts->name, what) == 0)
	    break;
    if (ts->name != (char *) 0) {
	TCalloc(ts, how);
	/*
	 * Reset variables
	 */
	if (GoodStr(T_me) && GoodStr(T_ue))
	    me_all = (strcmp(Str(T_me), Str(T_ue)) == 0);
	else
	    me_all = 0;
	if (GoodStr(T_me) && GoodStr(T_se))
	    me_all |= (strcmp(Str(T_me), Str(T_se)) == 0);

	T_CanCEOL = GoodStr(T_ce);
	T_CanDel = GoodStr(T_dc) || GoodStr(T_DC);
	T_CanIns = GoodStr(T_im) || GoodStr(T_ic) || GoodStr(T_IC);
	T_CanUP = GoodStr(T_up) || GoodStr(T_UP);
	return;
    }

    /*
     * Do the numeric ones second
     */
    for (tv = tval; tv->name != (char *) 0; tv++)
	if (strcmp(tv->name, what) == 0)
	    break;

    if (tv->name != (char *) 0) {
	if (tv == &tval[T_pt] || tv == &tval[T_km]) {
	    if (strcmp(how, "yes") == 0)
		tv->val = 1;
	    else if (strcmp(how, "no") == 0)
		tv->val = 0;
	    else {
		error("Usage: settc %s [yes|no]", tv->name);
		return;
	    }
	    T_Tabs = Val(T_pt);
	    T_HasMeta = Val(T_km);
	    return;
	}
	else {
	    tv->val = atoi(how);
	    T_Cols = Val(T_co);
	    T_Lines = Val(T_li);
	    if (tv == &tval[T_co])
		ReBufferDisplay();
	    return;
	}
    }
    error("settc: Unknown capability %s", what);
    return;
}


/*
 * Print the termcap string out with variable substitution
 */
void
EchoTC(v)
Char  **v;
{
    char   *cap, *scap, cv[BUFSIZ];
    int     arg_need, arg_cols, arg_rows;
    int     verbose = 0;
    char   *area;
    static char *fmts = "%s\n", *fmtd = "%d\n";
    char   buf[TC_BUFSIZ];

    area = buf;

    if (!*v || *v[0] == '\0')
	return;
    if (v[0][0] == '-' && v[0][1] == 'v') {
	verbose = 1;
	v++;
    }
    (void) strcpy(cv, short2str(*v));
    if (strcmp(cv, "tabs") == 0) {
	CSHprintf(fmts, T_Tabs ? "yes" : "no");
	flush();
	return;
    }
    else if (strcmp(cv, "meta") == 0) {
	CSHprintf(fmts, Val(T_km) ? "yes" : "no");
	flush();
	return;
    }
    else if (strcmp(cv, "baud") == 0) {
	int     i;

	for (i = 0; baud_rate[i].b_name != (char *) 0; i++)
	    if (T_Speed == baud_rate[i].b_rate) {
		CSHprintf(fmts, baud_rate[i].b_name);
		flush();
		return;
	    }
	CSHprintf(fmtd, 0);
	flush();
	return;
    }
    else if (strcmp(cv, "rows") == 0 || strcmp(cv, "lines") == 0) {
	CSHprintf(fmtd, Val(T_li));
	flush();
	return;
    }
    else if (strcmp(cv, "cols") == 0) {
	CSHprintf(fmtd, Val(T_co));
	flush();
	return;
    }

    /*
     * Count home many values we need for this capability.
     */
    scap = tgetstr(cv, &area);
    if (!scap || scap[0] == '\0')
	error("No capability called `%s'", cv);

    for (cap = scap, arg_need = 0; *cap; cap++)
	if (*cap == '%')
	    switch (*++cap) {
	    case 'd':
	    case '2':
	    case '3':
	    case '.':
	    case '+':
		arg_need++;
		break;
	    case '%':
	    case '>':
	    case 'i':
	    case 'r':
	    case 'n':
	    case 'B':
	    case 'D':
		break;
	    default:
		/*
		 * hpux has lot's of them...
		 */
		if (verbose)
		    error("Weird param `%%%c'", *cap);
		/* This is bad, but I won't complain */
		break;
	    }

    arg_cols = arg_rows = 0;
    switch (arg_need) {
	static char *err_1 = "Too many arguments for `%s'",
		    *err_2 = "`%s' needs two arguments";
    case 0:
	v++;
	if (*v && *v[0])
	    error(err_1, cv);
	(void) tputs(scap, 1, putraw);
	break;
    case 1:
	v++;
	if (!*v || *v[0] == '\0')
	    error("`%s' needs one arg", cv);
	arg_cols = atoi(short2str(*v));
	v++;
	if (*v && *v[0])
	    error("Too many arguments for `%s'", cv);
	(void) tputs(tgoto(scap, arg_cols, arg_rows), 1, putraw);
	break;
    default:
	/* This is wrong, but I will ignore it... */
	if (verbose)
	    error("Too many args needed");
    case 2:
	v++;
	if (!*v || *v[0] == '\0')
	    error(err_2, cv);
	arg_cols = atoi(short2str(*v));
	v++;
	if (!*v || *v[0] == '\0')
	    error(err_2, cv);
	arg_rows = atoi(short2str(*v));
	v++;
	if (*v && *v[0])
	    error(err_1, cv);
	(void) tputs(tgoto(scap, arg_cols, arg_rows), arg_rows, putraw);
	break;
    }
    flush();
}

bool    GotTermCaps = 0;

void 
BindArrowKeys()
{
    KEYCMD *map;
    int i;
    char *p;
    static struct { 
	int key, fun; 
    } ar[] = 
    { { T_kd, F_DOWN_HIST }, 
      { T_ku, F_UP_HIST   },
      { T_kl, F_CHARBACK },
      { T_kr, F_CHARFWD  } };

    if (!GotTermCaps)
	return;
    map = VImode ? CcAltMap : CcKeyMap;
    
    for (i = 0; i < 4; i++) {
	p = tstr[ar[i].key].str;
	if (p && *p) {
	    if (p[1]) {
		AddXkeyCmd(str2short(p), ar[i].fun);
		map[(unsigned char) *p] = F_XKEY;
	    }
	    else if (map[(unsigned char) *p] == F_UNASSIGNED) {
		ClearXkey(map, str2short(p));
		map[(unsigned char) *p] = ar[i].fun;
	    }
	}
    }
}

static Char cur_atr = 0;	/* current attributes */

void
SetAttributes(atr)
int     atr;
{
    atr &= ATTRIBUTES;
    if (atr != cur_atr) {
	if (me_all && GoodStr(T_me)) {
	    if ((cur_atr & BOLD) && !(atr & BOLD) ||
		(cur_atr & UNDER) && !(atr & UNDER) ||
		(cur_atr & SNODE_ANDOUT) && !(atr & SNODE_ANDOUT)) {
		(void) tputs(Str(T_me), 1, putpure);
		cur_atr = 0;
	    }
	}
	if ((atr & BOLD) != (cur_atr & BOLD)) {
	    if (atr & BOLD) {
		if (GoodStr(T_md) && GoodStr(T_me)) {
		    (void) tputs(Str(T_md), 1, putpure);
		    cur_atr |= BOLD;
		}
	    }
	    else {
		if (GoodStr(T_md) && GoodStr(T_me)) {
		    (void) tputs(Str(T_me), 1, putpure);
		    if ((cur_atr & SNODE_ANDOUT) && GoodStr(T_se)) {
			(void) tputs(Str(T_se), 1, putpure);
			cur_atr &= ~SNODE_ANDOUT;
		    }
		    if ((cur_atr & UNDER) && GoodStr(T_ue)) {
			(void) tputs(Str(T_ue), 1, putpure);
			cur_atr &= ~UNDER;
		    }
		    cur_atr &= ~BOLD;
		}
	    }
	}
	if ((atr & SNODE_ANDOUT) != (cur_atr & SNODE_ANDOUT)) {
	    if (atr & SNODE_ANDOUT) {
		if (GoodStr(T_so) && GoodStr(T_se)) {
		    (void) tputs(Str(T_so), 1, putpure);
		    cur_atr |= SNODE_ANDOUT;
		}
	    }
	    else {
		if (GoodStr(T_se)) {
		    (void) tputs(Str(T_se), 1, putpure);
		    cur_atr &= ~SNODE_ANDOUT;
		}
	    }
	}
	if ((atr & UNDER) != (cur_atr & UNDER)) {
	    if (atr & UNDER) {
		if (GoodStr(T_us) && GoodStr(T_ue)) {
		    (void) tputs(Str(T_us), 1, putpure);
		    cur_atr |= UNDER;
		}
	    }
	    else {
		if (GoodStr(T_ue)) {
		    (void) tputs(Str(T_ue), 1, putpure);
		    cur_atr &= ~UNDER;
		}
	    }
	}
    }
}

/* PWP 6-27-88 -- if the tty driver thinks that we can tab, we ask termcap */
int
CanWeTab()
{
    return (Val(T_pt));
}

void
MoveToLine(where)		/* move to line <where> (first line == 0) */
int     where;			/* as efficiently as possible; */
{
    int     del, i;

    if (where == CursorV)
	return;

    if (where > TermV) {
#ifdef DEBUG_SCREEN
	CSHprintf("MoveToLine: where is ridiculous: %d\r\n", where);
	flush();
#endif
	return;
    }

    if ((del = where - CursorV) > 0) {
	if ((del > 1) && GoodStr(T_DO)) 
	    (void) tputs(tgoto(Str(T_DO), del, del), del, putpure);
	else {
	    for (i = 0; i < del; i++)
		(void) putraw('\n');
	    CursorH = 0;	/* because the \n will become \r\n */
	}
    }
    else {			/* del < 0 */
	if (GoodStr(T_UP) && (-del > 1 || !GoodStr(T_up))) 
	    (void) tputs(tgoto(Str(T_UP), -del, -del), -del, putpure);
	else {
	    if (GoodStr(T_up))
		for (i = 0; i < -del; i++)
		    (void) tputs(Str(T_up), 1, putpure);
	}
    }
    CursorV = where;		/* now where is here */
}

void
MoveToChar(where)		/* move to character position (where) */
int     where;
{				/* as efficiently as possible */
    int     del, i;

  mc_again:
    if (where == CursorH)
	return;

    if (where > (TermH + 1)) {
#ifdef DEBUG_SCREEN
	CSHprintf("MoveToChar: where is riduculous: %d\r\n", where);
	flush();
#endif
	return;
    }

    if (!where) {		/* if where is first column */
	(void) putraw('\r');		/* do a CR */
	CursorH = 0;
	return;
    }

    del = where - CursorH;

    if ((del < -4 || del > 4) && GoodStr(T_ch)) 
	/* go there directly */
	(void) tputs(tgoto(Str(T_ch), where, where), where, putpure);	
    else {
	if (del > 0) {	/* moving forward */
	    if ((del > 4) && GoodStr(T_RI)) 
		(void) tputs(tgoto(Str(T_RI), del, del), del, putpure);
	    else {
		if (T_Tabs) {	/* if I can do tabs, use them */
		    if ((CursorH & 0370) != (where & 0370)) {
			/* if not within tab stop */
			for (i = (CursorH & 0370); i < (where & 0370); i += 8) 
			    (void) putraw('\t');	/* then tab over */
			CursorH = where & 0370;
		    }
		}
		/* it's usually cheaper to just write the chars, so we do. */

		/* NOTE THAT so_write() WILL CHANGE CursorH!!! */
		so_write(&Display[CursorV][CursorH], where - CursorH);

	    }
	}
	else {			/* del < 0 := moving backward */
	    if ((-del > 4) && GoodStr(T_LE)) 
		(void) tputs(tgoto(Str(T_LE), -del, -del), -del, putpure);
	    else {		/* can't go directly there */
		/* if the "cost" is greater than the "cost" from col 0 */
		if (T_Tabs ? (-del > ((where >> 3) + (where & 07)))
		    : (-del > where)) {
		    (void) putraw('\r');	/* do a CR */
		    CursorH = 0;
		    goto mc_again;	/* and try again */
		}
		for (i = 0; i < -del; i++)
		    (void) putraw('\b');
	    }
	}
    }
    CursorH = where;		/* now where is here */
}

void
so_write(cp, n)
register Char *cp;
register int n;
{
    if (n <= 0)
	return;			/* catch bugs */

    if (n > (TermH + 1)) {
#ifdef DEBUG_SCREEN
	CSHprintf("so_write: n is riduculous: %d\r\n", n);
	flush();
#endif
	return;
    }

    do {
	if (*cp & LITERAL) {
	    extern Char *litptr[];
	    Char *d;
#ifdef DEBUG_LITERAL
	    CSHprintf("so: litnum %d, litptr %x\r\n",
		      *cp & CHAR, litptr[*cp & CHAR]);
#endif
	    for (d = litptr[*cp++ & CHAR]; *d & LITERAL; d++)
		(void) putraw(*d & CHAR);
	    (void) putraw(*d);

	}
	else
	    (void) putraw(*cp++);
	CursorH++;
    } while (--n);
}


void
DeleteChars(num)		/* deletes <num> characters */
int     num;
{
    if (num <= 0)
	return;

    if (!T_CanDel) {
#ifdef DEBUG_EDIT
	CSHprintf("   ERROR: cannot delete   \n");
#endif
	flush();
	return;
    }

    if (num > TermH) {
#ifdef DEBUG_SCREEN
	CSHprintf("DeleteChars: num is riduculous: %d\r\n", num);
	flush();
#endif
	return;
    }

    if (GoodStr(T_DC)) 			/* if I have multiple delete */
	if ((num > 1) || !GoodStr(T_dc)) {	/* if dc would be more expen. */
	    (void) tputs(tgoto(Str(T_DC), num, num), num, putpure);
	    return;
	}

    if (GoodStr(T_dm)) 			/* if I have delete mode */
	(void) tputs(Str(T_dm), 1, putpure);

    if (GoodStr(T_dc)) 			/* else do one at a time */
	while (num--)
	    (void) tputs(Str(T_dc), 1, putpure);

    if (GoodStr(T_ed)) 			/* if I have delete mode */
	(void) tputs(Str(T_ed), 1, putpure);
}

void
Insert_write(cp, num)		/* Puts terminal in insert character mode, */
register Char *cp;
register int num;		/* or inserts num characters in the line */
{
    if (num <= 0)
	return;

    if (!T_CanIns) {
#ifdef DEBUG_EDIT
	CSHprintf("   ERROR: cannot insert   \n");
#endif
	flush();
	return;
    }

    if (num > TermH) {
#ifdef DEBUG_SCREEN
	CSHprintf("StartInsert: num is riduculous: %d\r\n", num);
	flush();
#endif
	return;
    }

    if (GoodStr(T_IC)) 		/* if I have multiple insert */
	if ((num > 1) || !GoodStr(T_ic)) {	/* if ic would be more expen. */
	    (void) tputs(tgoto(Str(T_IC), num, num), num, putpure);
	    so_write(cp, num);	/* this updates CursorH */
	    return;
	}

    if (GoodStr(T_im)) 		/* if I have insert mode */
	(void) tputs(Str(T_im), 1, putpure);

    do {
	if (GoodStr(T_ic))		/* have to make num chars insert */
	    (void) tputs(Str(T_ic), 1, putpure);	/* insert a char */

	(void) putraw(*cp++);

	CursorH++;

	if (GoodStr(T_ip))		/* have to make num chars insert */
	    (void) tputs(Str(T_ip), 1, putpure); /* pad the inserted char */

    } while (--num);

    if (GoodStr(T_ei))
	(void) tputs(Str(T_ei), 1, putpure);
}

void
ClearEOL(num)			/* clear to end of line.  There are num */
int     num;			/* characters to clear */
{
    register int i;

    if (T_CanCEOL && GoodStr(T_ce)) 
	(void) tputs(Str(T_ce), 1, putpure);
    else {
	for (i = 0; i < num; i++)
	    (void) putraw(' ');
	CursorH += num;		/* have written num spaces */
    }
}

void
ClearScreen()
{				/* clear the whole screen and home */
    if (GoodStr(T_cl)) 
	/* send the clear screen code */
	(void) tputs(Str(T_cl), Val(T_li), putpure);	
    else if (GoodStr(T_ho) && GoodStr(T_cd)) {
	(void) tputs(Str(T_ho), Val(T_li), putpure);	/* home */
	/* clear to bottom of screen */
	(void) tputs(Str(T_cd), Val(T_li), putpure);	
    }
    else {
	(void) putraw('\r');
	(void) putraw('\n');
    }
}

void
Beep()
{				/* produce a sound */
    if (adrof(STRnobeep))
	return;

    if (GoodStr(T_vb) && adrof(STRvisiblebell)) 
	(void) tputs(Str(T_vb), 1, putpure);	/* visible bell */
    else if (GoodStr(T_bl)) 
	/* what termcap says we should use */
	(void) tputs(Str(T_bl), 1, putpure);
    else 
	(void) putraw('\007');		/* an ASCII bell; ^G */
}

void
ClearToBottom()               /* clear to the bottom of the screen */
{
    if (GoodStr(T_cd)) 
	(void) tputs(Str(T_cd), Val(T_li), putpure);	
    else if (GoodStr(T_ce)) 
	(void) tputs(Str(T_ce), Val(T_li), putpure);	
}

void
GetTermCaps()
{				/* read in the needed terminal capabilites */
    register int i;
    char   *ptr;
    char   buf[TC_BUFSIZ];
    static char bp[TC_BUFSIZ];
    char   *area;
    extern char   *getenv();
    struct termcapstr *t;


#ifdef SIG_WINDOW
    sigmask_t omask;
    int lins, cols;

    /* don't want to confuse things here */
# ifdef BSDSIGS
    omask = sigblock(sigmask(SIG_WINDOW)) & ~sigmask(SIG_WINDOW);
# else /* BSDSIGS */
    (void) sighold(SIG_WINDOW);
# endif /* BSDSIGS */
#endif /* SIG_WINDOW */
    area = buf;

    GotTermCaps = 1;

    ptr = getenv("TERM");
    if (!ptr || !ptr[0])
	ptr = "dumb";

    setzero(bp, TC_BUFSIZ);

    i = tgetent(bp, ptr);
    if (i <= 0) {
	if (i == -1) {
#if SVID == 0
	    CSHprintf("tcsh: Cannot open /etc/termcap.\n");
	}
	else if (i == 0) {
#endif				/* SVID */
	    CSHprintf("tcsh: No entry for terminal type \"%s\"\n",
		      getenv("TERM"));
	}
	CSHprintf("tcsh: using dumb terminal settings.\n");
	Val(T_co) = 80;		/* do a dumb terminal */
	Val(T_pt) = Val(T_km) = Val(T_li) = 0;
	for (t = tstr; t->name != (char *) 0; t++) 
	     TCalloc(t, (char *) 0);
    }
    else {
	/* Can we tab */
	Val(T_pt) = tgetflag("pt") && !tgetflag("xt");
	/* do we have a meta? */
	Val(T_km) = (tgetflag("km") || tgetflag("MT"));	
	Val(T_co) = tgetnum("co");
	Val(T_li) = tgetnum("li");
	for (t = tstr; t->name != (char *) 0; t++) 
	    TCalloc(t, tgetstr(t->name, &area));
    }
    if (Val(T_co) < 2)
	Val(T_co) = 80;		/* just in case */
    if (Val(T_li) < 1)
	Val(T_li) = 24;
    
    T_Cols = Val(T_co);
    T_Lines = Val(T_li);
    if (T_Tabs)
    	T_Tabs = Val(T_pt);
    T_HasMeta = Val(T_km);
    T_CanCEOL = GoodStr(T_ce);
    T_CanDel = GoodStr(T_dc) || GoodStr(T_DC);
    T_CanIns = GoodStr(T_im) || GoodStr(T_ic) || GoodStr(T_IC);
    T_CanUP = GoodStr(T_up) || GoodStr(T_UP);
    if (GoodStr(T_me) && GoodStr(T_ue))
	me_all = (strcmp(Str(T_me), Str(T_ue)) == 0);
    else
	me_all = 0;
    if (GoodStr(T_me) && GoodStr(T_se))
	me_all |= (strcmp(Str(T_me), Str(T_se)) == 0);


#ifdef DEBUG_SCREEN
    if (!T_CanUP) {
	CSHprintf("tcsh: WARNING: Your terminal cannot move up.\n");
	CSHprintf("Editing may be odd for long lines.\n");
    }
    if (!T_CanCEOL) 
	CSHprintf("no clear EOL capability.\n");
    if (!T_CanDel)
	CSHprintf("no delete char capability.\n");
    if (!T_CanIns)
	CSHprintf("no insert char capability.\n");
#endif /* DEBUG_SCREEN */



#ifdef SIG_WINDOW
    (void) GetSize(&lins, &cols); /* get the correct window size for sure */
    ChangeSize(lins, cols);	

# ifdef BSDSIGS
    (void) sigsetmask(omask);	/* can change it again */
# else	/* BSDSIGS */
    sigrelse(SIG_WINDOW);
# endif	/* BSDSIGS */ 
#else /* SIG_WINDOW */
    ChangeSize(Val(T_li), Val(T_co));
#endif /* SIG_WINDOW */

    BindArrowKeys();
}

#ifdef SIG_WINDOW
/* GetSize():
 *	Return the new window size in lines and cols, and
 *	true if the size was changed.
 */
int
GetSize(lins, cols)
int *lins, *cols;
{
    *cols = Val(T_co);
    *lins = Val(T_li);

#ifdef TIOCGWINSZ
#ifndef lint
    {
	struct winsize ws;		/* from 4.3 */
	if (ioctl(SHOUT, TIOCGWINSZ, (ioctl_t) &ws) < 0)
	    return(0);

	if (ws.ws_col)
	    *cols = ws.ws_col;
	if (ws.ws_row)
	    *lins = ws.ws_row;
    }
#endif
#else /* TIOCGWINSZ */
# ifdef TIOCGSIZE
    {
	struct ttysize ts;		/* from Sun */

	if (ioctl(SHOUT, TIOCGSIZE, (ioctl_t) & ts) < 0)
	    return;

	if (ts.ts_cols)
	    *cols = ts.ts_cols;
	if (ts.ts_lines)
	    *lins = ts.ts_lines;
    }
# endif	/* TIOCGSIZE */
#endif /* TIOCGWINSZ */

    if (*cols < 2)
	*cols = 80;		/* just in case */
    if (*lins < 1)
	*lins = 24;

    return(Val(T_co) != *cols || Val(T_li) != *lins);
}
#endif /* SIGWINDOW */

void
ChangeSize(lins, cols)
int lins, cols;
{

    Val(T_co) = cols;
    Val(T_li) = lins;
    
#ifdef SIG_WINDOW
    {
	Char buf[10];
	char *tptr;

	if (getenv ("COLUMNS")) {
	    Itoa(Val(T_co), buf);
	    Setenv(STRCOLUMNS, Strsave(buf));
	}

	if (getenv("LINES")) {
	    Itoa(Val(T_li), buf);
	    Setenv(STRLINES, Strsave(buf));
	}

	if (tptr = getenv ("TERMCAP")) {
	    Char termcap[1024], backup[1024], *ptr;
	    int i;

	    ptr = str2short (tptr);
	    (void) Strncpy (termcap, ptr, 1024);
	    termcap[1023] = '\0';

	    /* update termcap string; first do columns */
	    buf[0] = 'c'; buf[1] = 'o'; buf[2] = '#'; buf[3] = '\0';
	    if ((ptr = Strstr (termcap, buf)) == NULL) {
		(void) Strcpy (backup, termcap);
	    } else {
		i = ptr - termcap + Strlen(buf);
		(void) Strncpy (backup, termcap, i);
		backup[i] = '\0';
		Itoa(Val(T_co), buf);
		(void) Strcat (backup + i, buf);
		ptr = Strchr (ptr, ':');
		(void) Strcat (backup, ptr);
	    }

	    /* now do lines */
	    buf[0] = 'l'; buf[1] = 'i'; buf[2] = '#'; buf[3] = '\0';
	    if ((ptr = Strstr (backup, buf)) == NULL) {
		(void) Strcpy (termcap, backup);
	    } else {
		i = ptr - backup + Strlen(buf);
		(void) Strncpy (termcap, backup, i);
		termcap[i] = '\0';
		Itoa(Val(T_li), buf);
		(void) Strcat (termcap, buf);
		ptr = Strchr (ptr, ':');
		(void) Strcat (termcap, ptr);
	    }
	    Setenv (STRTERMCAP, Strsave(termcap));
	}
    }
#endif /* SIG_WINDOW */

    ReBufferDisplay();		/* re-make display buffers */
    ClearDisp();
}

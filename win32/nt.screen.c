/*$Header: /src/pub/tcsh/win32/nt.screen.c,v 1.1 2002/09/13 05:27:10 amold Exp $*/
/*
 * ed.screen.c: Editor/termcap-curses interface
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
 * 3. Neither the name of the University nor the names of its contributors
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


#include "ed.h"
#include "tc.h"
#include "ed.defns.h"

# define PUTPURE putpure
# define PUTRAW putraw


/* #define DEBUG_LITERAL */

/*
 * IMPORTANT NOTE: these routines are allowed to look at the current screen
 * and the current possition assuming that it is correct.  If this is not
 * true, then the update will be WRONG!  This is (should be) a valid
 * assumption...
 */



extern int nt_getsize(int*,int*,int*);
extern int nt_ClearEOL( void) ;
extern void NT_ClearEOD( void) ;
extern void NT_ClearScreen(void) ;
extern void NT_VisibleBell(void);
extern void NT_WrapHorizontal(void);

int DisplayWindowHSize;
	void
terminit()
{
	return;
}



int T_ActualWindowSize;

static	void	ReBufferDisplay	__P((void));


/*ARGSUSED*/
	void
TellTC(what)
	char   *what;
{

	USE(what);
	xprintf(CGETS(7, 1, "\n\tYou're using a Windows console.\n"));
}


	static void
ReBufferDisplay()
{
	register int i;
	eChar **b;
	eChar **bufp;
	int lins,cols;

	nt_getsize(&lins,&cols,&DisplayWindowHSize);

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
	TermH = cols;

	TermV = (INBUFSIZE * 4) / TermH + 1;
	b = (eChar **) xmalloc((size_t) (sizeof(*b) * (TermV + 1)));
	for (i = 0; i < TermV; i++)
		b[i] = (eChar *) xmalloc((size_t) (sizeof(*b[i]) * (TermH + 1)));
	b[TermV] = NULL;
	Display = b;
	b = (eChar **) xmalloc((size_t) (sizeof(*b) * (TermV + 1)));
	for (i = 0; i < TermV; i++)
		b[i] = (eChar *) xmalloc((size_t) (sizeof(*b[i]) * (TermH + 1)));
	b[TermV] = NULL;
	Vdisplay = b;
}

	void
SetTC(what, how)
	char   *what, *how;
{
	int li,win,co;

	nt_getsize(&li,&co,&win);
	if (!lstrcmp(what,"li")) {
		li = atoi(how);
		
	}else if(!lstrcmp(what,"co")) { //set window, not buffer size
		win = atoi(how);
	}
	else
		stderror(ERR_SYSTEM, "SetTC","Sorry, this function is not supported");

	ChangeSize(li,win);
	return;
}


/*
 * Print the termcap string out with variable substitution
 */
	void
EchoTC(v)
	Char  **v;
{

	char    cv[BUFSIZE];
	int     verbose = 0, silent = 0;
	static char *fmts = "%s\n", *fmtd = "%d\n";


	setname("echotc");

	tglob(v);
	if (gflag) {
		v = globall(v);
		if (v == 0)
			stderror(ERR_NAME | ERR_NOMATCH);
	}
	else
		v = gargv = saveblk(v);
	trim(v);

	if (!*v || *v[0] == '\0')
		return;
	if (v[0][0] == '-') {
		switch (v[0][1]) {
			case 'v':
				verbose = 1;
				break;
			case 's':
				silent = 1;
				break;
			default:
				stderror(ERR_NAME | ERR_TCUSAGE);
				break;
		}
		v++;
	}
	if (!*v || *v[0] == '\0')
		return;
	(void) strcpy(cv, short2str(*v));

	if(!lstrcmp(cv,"rows") || !lstrcmp(cv,"lines") ) {
		xprintf(fmtd,T_Lines);
		return;
	}
	else if(!lstrcmp(cv,"cols") ) {
		xprintf(fmtd,T_ActualWindowSize);
		return;
	}
	else if(!lstrcmp(cv,"buffer") ) {
		xprintf(fmtd,T_Cols);
		return;
	}
	else
		stderror(ERR_SYSTEM, "EchoTC","Sorry, this function is not supported");

}

bool    GotTermCaps = 0;


	void
ResetArrowKeys()
{
}

	void
DefaultArrowKeys() 
{
}


	int
SetArrowKeys(name, fun, type)
	CStr *name;
	XmapVal *fun;
	int type;
{
	return -1;
}

	int
IsArrowKey(name)
	Char *name;
{
	return 0;
}

	int
ClearArrowKeys(name)
	CStr *name;
{
	return -1;
}

	void
PrintArrowKeys(name)
	CStr *name;
{
	return;
}


	void
BindArrowKeys()
{
	return;
}
static Char cur_atr = 0;	/* current attributes */

#define GoodStr(ignore)  1
	void
SetAttributes(atr)
	int     atr;
{
	atr &= ATTRIBUTES;
}

/* PWP 6-27-88 -- if the tty driver thinks that we can tab, we ask termcap */
	int
CanWeTab()
{
	return 1;
}

	void
MoveToLine(where)		/* move to line <where> (first line == 0) */
	int     where;		/* as efficiently as possible; */
{
	int     del;

	if (where == CursorV)
		return;

	if (where > TermV) {
#ifdef DEBUG_SCREEN
		xprintf("MoveToLine: where is ridiculous: %d\r\n", where);
		flush();
#endif /* DEBUG_SCREEN */
		return;
	}

	del = where - CursorV;

	NT_MoveToLineOrChar(del, 1);

	CursorV = where;		/* now where is here */
}

	void
MoveToChar(where)		/* move to character position (where) */
	int     where;
{				/* as efficiently as possible */
	if (where == CursorH)
		return;

	if (where >= TermH) {
#ifdef DEBUG_SCREEN
		xprintf("MoveToChar: where is riduculous: %d\r\n", where);
		flush();
#endif /* DEBUG_SCREEN */
		return;
	}

	if (!where) {		/* if where is first column */
		//(void) putraw('\r');	/* do a CR */
		NT_MoveToLineOrChar(where, 0);
		flush();
		CursorH = 0;
		return;
	}

	NT_MoveToLineOrChar(where, 0);
	CursorH = where;		/* now where is here */
}

	void
so_write(cp, n)
	register eChar *cp;
	register int n;
{
	if (n <= 0)
		return;			/* catch bugs */

	if (n > TermH) {
		return;
	}

	do {
		if (*cp & LITERAL) {
			extern Char *litptr[];
			Char   *d;

			for (d = litptr[*cp++ & CHAR]; *d & LITERAL; d++)
				(void) putraw(*d & CHAR);
			(void) putraw(*d);

		}
		else
			(void) putraw(*cp++);
		CursorH++;
	} while (--n);

	if (CursorH >= TermH) { /* wrap? */
		CursorH = 0;
		CursorV++;
		NT_WrapHorizontal();

	}
	else if(CursorH >= DisplayWindowHSize) {
		flush();
		NT_MoveToLineOrChar(CursorH,0);
	}
}


	void
DeleteChars(num)		/* deletes <num> characters */
	int     num;
{
	if (num <= 0)
		return;

	if (!T_CanDel) {
#ifdef DEBUG_EDIT
		xprintf(CGETS(7, 16, "ERROR: cannot delete\r\n"));
#endif /* DEBUG_EDIT */
		flush();
		return;
	}

	if (num > TermH) {
#ifdef DEBUG_SCREEN
		xprintf(CGETS(7, 17, "DeleteChars: num is riduculous: %d\r\n"), num);
		flush();
#endif /* DEBUG_SCREEN */
		return;
	}

}

	void
Insert_write(cp, num)		/* Puts terminal in insert character mode, */
	register eChar *cp;
	register int num;		/* or inserts num characters in the line */
{
	if (num <= 0)
		return;
	if (!T_CanIns) {
#ifdef DEBUG_EDIT
		xprintf(CGETS(7, 18, "ERROR: cannot insert\r\n"));
#endif /* DEBUG_EDIT */
		flush();
		return;
	}

	if (num > TermH) {
#ifdef DEBUG_SCREEN
		xprintf(CGETS(7, 19, "StartInsert: num is riduculous: %d\r\n"), num);
		flush();
#endif /* DEBUG_SCREEN */
		return;
	}


}

	void
ClearEOL(num)			/* clear to end of line.  There are num */
	int     num;		/* characters to clear */
{

	if (num <= 0)
		return;

	nt_ClearEOL();

}

	void
ClearScreen()
{				/* clear the whole screen and home */

	NT_ClearScreen();

}

	void
SoundBeep()
{				/* produce a sound */
	beep_cmd ();
	if (adrof(STRnobeep))
		return;

	if (adrof(STRvisiblebell))
		NT_VisibleBell();	/* visible bell */
	else
		MessageBeep(MB_ICONQUESTION);
}

	void
ClearToBottom()
{				/* clear to the bottom of the screen */
	NT_ClearEOD();

}

	void
GetTermCaps()
{
	int lins,cols;

	nt_getsize(&lins,&cols,&DisplayWindowHSize);

	GotTermCaps = 1;

	T_Cols = cols;
	T_Lines = lins;
	T_ActualWindowSize = DisplayWindowHSize;
	T_Margin = MARGIN_AUTO;
	T_CanCEOL  = 1;
	T_CanDel = 0;
	T_CanIns = 0;
	T_CanUP = 1;

	ReBufferDisplay();
	ClearDisp();

	return;
}
#ifdef SIG_WINDOW
/* GetSize():
 *	Return the new window size in lines and cols, and
 *	true if the size was changed.
 */
	int
GetSize(lins, cols)
	int    *lins, *cols;
{

	*lins = T_Lines;

	*cols = T_Cols;

	nt_getsize(lins,cols,&DisplayWindowHSize);

	// compare the actual visible window size,but return the console buffer size
	// this is seriously demented.
	return  (T_Lines != *lins || T_ActualWindowSize != DisplayWindowHSize);


}

#endif /* SIGWINDOW */

	void
ChangeSize(lins, cols)
	int     lins, cols;
{

	// here we're setting the window size, not the buffer size.
	// 
	nt_set_size(lins,cols);

	T_Lines = lins;
	T_ActualWindowSize = cols;

	ReBufferDisplay();		/* re-make display buffers */
	ClearDisp();
}
	void
PutPlusOne(c)
	Char  c;
{
	extern int OldvcV;

	(void) putwraw(c);

	Display[CursorV][CursorH++] = (Char) c;

	if (CursorH >= TermH) {	/* if we must overflow */
		CursorH = 0;
		CursorV++;
		OldvcV++;
		NT_WrapHorizontal();
	}
	else if(CursorH >= DisplayWindowHSize) {
		NT_MoveToLineOrChar(CursorH,0);
	}
}

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/ed.inputl.c,v 1.16 1991/03/20 19:04:50 christos Exp $ */
/*
 * ed.inputl.c: Input line handling.
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id: ed.inputl.c,v 1.16 1991/03/20 19:04:50 christos Exp $";
#endif

#include "sh.h"
#include "ed.h"
#include "ed.defns.h"            /* for the function names */
#include "tw.h"			/* for twenex stuff */

#define OKCMD (INBUFSIZ+INBUFSIZ)
extern CCRETVAL e_up_hist();
extern CCRETVAL e_expand_history();

/* ed.inputl -- routines to get a single line from the input. */

extern bool tellwhat;
extern bool MapsAreInited;
extern bool Tty_raw_mode;

/* mismatched first character */
static Char mismatch[] = { '!', '\\', '^', '-', '%', '\0' };    

static int GetNextCommand();
static int SpellLine();

/* CCRETVAL */
int
Inputl()
{
    CCRETVAL retval;
    KEYCMD cmdnum = 0;
    extern KEYCMD NumFuns;
    unsigned char tch;		/* the place where read() goes */
    Char ch;
    int num;			/* how many chars we have read at NL */
    struct varent *crct = adrof(STRcorrect);
    struct varent *autol = adrof(STRautolist);
    Char *SaveChar, *CorrChar;
    Char Origin[INBUFSIZ],Change[INBUFSIZ];
    int matchval;		/* from tenematch() */

    if (!MapsAreInited)		/* double extra just in case */
	ed_InitMaps();

    ClearDisp();		/* reset the display stuff */
    ResetInLine();		/* reset the input pointers */
    if (GettingInput)
        MacroLvl = -1;		/* editor was interrupted during input */

#ifdef FIONREAD
    if (!Tty_raw_mode && MacroLvl < 0) {
        long chrs = 0;

        (void) ioctl(SHIN,FIONREAD, (ioctl_t) &chrs);
        if (chrs == 0) {
            if (Rawmode() < 0)
                return 0;
        }
    }
#endif 

    GettingInput = 1;
    NeedsRedraw = 0;

    if (tellwhat) {
 	copyn(InputBuf, WhichBuf, INBUFSIZ);
	LastChar = InputBuf + (LastWhich - WhichBuf);
	Cursor = InputBuf + (CursWhich - WhichBuf);
	tellwhat = 0;
	Hist_num = HistWhich;
    }
    if (Expand) {
	(void) e_up_hist ();
	Expand = 0;
    }
    Refresh();			/* print the prompt */

    for(num = OKCMD; num == OKCMD;) {	/* while still editing this line */
#ifdef DEBUG_EDIT
	if (Cursor > LastChar)
	    CSHprintf("Cursor > LastChar\r\n");
	if (Cursor < InputBuf)
	    CSHprintf("Cursor < InputBuf\r\n");
	if (Cursor > InputLim)
	    CSHprintf ("Cursor > InputLim\r\n");
	if (LastChar > InputLim)
	    CSHprintf ("LastChar > InputLim\r\n");
	if (InputLim != &InputBuf[INBUFSIZ-2])
	    CSHprintf  ("InputLim != &InputBuf[INBUFSIZ-2]\r\n");
	if ((!DoingArg) && (Argument != 1))
	    CSHprintf ("(!DoingArg) && (Argument != 1)\r\n");
	if (CcKeyMap[0] == 0)
	    CSHprintf ("CcKeyMap[0] == 0 (maybe not inited)\r\n");
#endif

	/* if EOF or error */
	if ((num = GetNextCommand(&cmdnum,&ch)) != OKCMD) { 
	    break;
	}

	if (cmdnum >= NumFuns) { /* BUG CHECK command */
#ifdef DEBUG_EDIT
            CSHprintf ("ERROR: illegal command from key 0%o\r\n", ch);
#endif
            continue;		/* try again */
	}

	/* now do the real command */
	retval = (*CcFuncTbl[cmdnum])(ch);

	/* save the last command here */
	LastCmd = cmdnum;

	/* use any return value */
	switch (retval) {

          case CC_REFRESH:
            Refresh();
	    /* fall through */
	  case CC_NORM:		/* normal char */
	    Argument = 1;
	    DoingArg = 0;
	    /* fall through */
	  case CC_ARGHACK:	/* Suggested by Rich Salz */
				/* <rsalz@pineapple.bbn.com> */
	    break;		/* keep going... */
	    
	  case CC_EOF:		/* end of file typed */
#ifdef notdef
	    PromptBuf[0] = '\0';
#endif
	    num = 0;
            break;

	  case CC_WHICH:	/* tell what this command does */
	    tellwhat = 1;
	    copyn(WhichBuf, InputBuf, INBUFSIZ);
	    LastWhich = WhichBuf + (LastChar - InputBuf);
	    CursWhich = WhichBuf + (Cursor - InputBuf);
	    *LastChar++ = '\n';	/* for the benifit of CSH */
	    HistWhich = Hist_num;
	    Hist_num = 0;	/* for the history commands */
	    num = LastChar - InputBuf; /* number characters read */
#ifdef notdef
	    ResetInLine();	/* reset the input pointers */
#endif
	    break;

	  case CC_NEWLINE:	/* normal end of line */
	    if ( crct && (!Strcmp(*(crct->vec),STRcmd) ||
				!Strcmp(*(crct->vec),STRall))) {
		(void) Strcpy(Origin,InputBuf);
		SaveChar = LastChar;
		if (SpellLine(!Strcmp(*(crct->vec),STRcmd)) == 1){
		    (void) Strcpy(Change,InputBuf);
		    *Strchr(Change,'\n') = '\0';
		    CorrChar = LastChar;	/* Save the corrected end */
		    LastChar = InputBuf;	/* Null the current line */
		    Beep();
		    printprompt(2, Change);
		    Refresh();
		    (void) read(SHIN, (char *) &tch, 1);
		    ch = tch;
		    if (ch != 'y' && ch != ' ') {
			(void) Strcpy(InputBuf,Origin);
			LastChar = SaveChar;
			CSHprintf("no\n");
		    } else {
			LastChar = CorrChar;	/* Restore the corrected end */
			CSHprintf("yes\n");
		    }
		    flush();
		}
	    }			/* end CORRECT code */
	    tellwhat = 0;	/* just in case */
	    Hist_num = 0;	/* for the history commands */
	    num = LastChar - InputBuf; /* return the number of chars read */
	    /*
	     * For continuation lines, we set the prompt to prompt 2
	     */
	    printprompt(1, (Char *) 0);
#ifdef notdef
	    ResetInLine();	/* reset the input pointers */
	    PromptBuf[0] = '\0';
#endif
	    break;

	  case CC_CORRECT:
	    if (tenematch (InputBuf, INBUFSIZ, Cursor-InputBuf,
			   SPELL) < 0)
		Beep ();	/* Beep = No match/ambiguous */
	    if (NeedsRedraw) {
		ClearLines();
		ClearDisp();
		NeedsRedraw = 0;
	    }
	    Refresh();
	    Argument = 1;
	    DoingArg = 0;
	    break;

	  case CC_CORRECT_L:
	    if (SpellLine(FALSE) < 0)
		Beep ();	/* Beep = No match/ambiguous */
	    if (NeedsRedraw) {
		ClearLines();
		ClearDisp();
		NeedsRedraw = 0;
	    }
	    Refresh();
	    Argument = 1;
	    DoingArg = 0;
	    break;


	  case CC_COMPLETE:
	    if (adrof(STRautoexpand))
		(void) e_expand_history();
	    if ((matchval = tenematch (InputBuf, INBUFSIZ, Cursor-InputBuf,
			   RECOGNIZE)) != 1) {
/*
 * Addition by David C Lawrence <tale@pawl.rpi.edu>: If an attempted
 * completion is ambiguous, list the choices.  (PWP: this is the best
 * feature addition to tcsh I have seen in many months.)
 */
		if (matchval >= 0 && autol) {
		    if (matchval > 1) { /* ambigous */
			if (Strcmp(*(autol->vec), STRbeepnone) &&
			    Strcmp(*(autol->vec), STRbeepnever))
			    Beep();
			PastBottom();
			(void) tenematch (InputBuf, INBUFSIZ, Cursor-InputBuf, 
					  LIST);
		    } else {	/* no match */
			if (Strcmp(*(autol->vec), STRbeepnever))
			    Beep();
		    }
		} else
		    Beep ();	/* error */
	    }
	    if (NeedsRedraw) {
		PastBottom();
		ClearLines();
		ClearDisp();
		NeedsRedraw = 0;
	    }
	    Refresh();
	    Argument = 1;
	    DoingArg = 0;
	    break;

	  case CC_LIST_CHOICES:
	    /* should catch ^C here... */
	    (void) tenematch (InputBuf, INBUFSIZ, Cursor-InputBuf, LIST);
	    Refresh();
	    Argument = 1;
	    DoingArg = 0;
	    break;

	  case CC_LIST_GLOB:
	    (void) tenematch (InputBuf, INBUFSIZ, Cursor-InputBuf, GLOB);
	    Refresh();
	    Argument = 1;
	    DoingArg = 0;
	    break;

	  case CC_EXPAND_GLOB:
	    if (tenematch (InputBuf, INBUFSIZ, Cursor-InputBuf,
			   GLOB_EXPAND) <= 0)
		Beep ();	/* Beep = No match */
	    if (NeedsRedraw) {
		ClearLines();
		ClearDisp();
		NeedsRedraw = 0;
	    }
	    Refresh();
	    Argument = 1;
	    DoingArg = 0;
	    break;

	  case CC_EXPAND_VARS:
	    if (tenematch (InputBuf, INBUFSIZ, Cursor-InputBuf,
			   VARS_EXPAND) <= 0)
		Beep ();	/* Beep = No match */
	    if (NeedsRedraw) {
		ClearLines();
		ClearDisp();
		NeedsRedraw = 0;
	    }
	    Refresh();
	    Argument = 1;
	    DoingArg = 0;
	    break;

	  case CC_HELPME:
	    CSHputchar ('\n');
	    /* should catch ^C here... */
	    (void) tenematch (InputBuf, INBUFSIZ, LastChar-InputBuf, 
			      PRINT_HELP);
	    Refresh();
	    Argument = 1;
	    DoingArg = 0;
	    break;

	  case CC_FATAL:	/* fatal error, reset to known state */
#ifdef DEBUG_EDIT
	    CSHprintf ("*** editor fatal ERROR ***\r\n\n");
#endif /* DEBUG_EDIT */
	    			/* put (real) cursor in a known place */
	    ClearDisp();		/* reset the display stuff */
	    ResetInLine();		/* reset the input pointers */
	    Refresh();			/* print the prompt again */
	    Argument = 1;
	    DoingArg = 0;
	    break;

	  case CC_ERROR:
	  default:		/* functions we don't know about */
	    DoingArg = 0;
	    Argument = 1;
	    Beep();
	    flush();
	    break;
	}
    }
    (void) Cookedmode();	/* make sure the tty is set up correctly */
    GettingInput = 0;
    flush();		/* flush any buffered output */
    return num;
}

void
PushMacro(str)
    Char *str;
{
    if (str != NULL && MacroLvl+1 < MAXMACROLEVELS) {
        MacroLvl++;
        KeyMacro[MacroLvl] = str;
    } else {
        Beep();
        flush();
    }
}

static int
GetNextCommand(cmdnum,ch)
KEYCMD * cmdnum;
register Char *ch;
{
    KEYCMD cmd = 0;
    int num;
    Char *str;

    for (; cmd == 0 || cmd == F_XKEY; ) {
        if (( num = GetNextChar(ch)) != 1) { /* if EOF or error */
            return num;
        }
#ifdef	KANJI
	if ( *ch & META ) {
		MetaNext = 0;
		cmd = CcViMap[' '];
		break;
	} else
#endif /* KANJI */
	if (MetaNext) {
	    MetaNext = 0;
            *ch |= META;
        }
        cmd = CurrentKeyMap[(unsigned char) *ch];
        if (cmd == F_XKEY) {
            if (GetXkey(ch,&str)) 
                cmd = (KEYCMD) *str;
            else 
                PushMacro(str);
        }
        if (!AltKeyMap) {
            CurrentKeyMap = CcKeyMap;
        }
    }
    *cmdnum = cmd;
    return OKCMD;
}
      
int
GetNextChar(cp)
register Char *cp;
{
    register int num_read;
#if defined(EWOULDBLOCK) || (defined(POSIX) && defined(EAGAIN))
# if defined(FIONBIO) || (defined(F_SETFL) && defined(O_NDELAY))
    int tried = 0;
# endif /* FIONBIO || (F_SETFL && O_NDELAY) */
#endif /* EWOULDBLOCK || (POSIX && EAGAIN) */
    unsigned char tcp;

    for (;;) {
	if (MacroLvl < 0) {
            if (!Load_input_line())
                break;
        }
	if (*KeyMacro[MacroLvl] == 0) {
	    MacroLvl--;
	    continue;
	}
	*cp = *KeyMacro[MacroLvl]++ & CHAR;
	if (*KeyMacro[MacroLvl] == 0) {  /* Needed for QuoteMode On */
	    MacroLvl--;
        }
	return (1);
    }

    if (Rawmode() < 0)		/* make sure the tty is set up correctly */
	return 0;		/* oops: SHIN was closed */
again:
    if (( num_read = read(SHIN, (char *) &tcp, 1)) != 1) { /* if EOF or error */
#if defined(EWOULDBLOCK) || (defined(POSIX) && defined(EAGAIN))
# ifdef FIONBIO
	if ((num_read < 0) && (errno == EWOULDBLOCK
#if defined(POSIX) && defined(EAGAIN)
	    || EAGAIN
#endif /* POSIX && EAGAIN */
	    ) && !tried) {
	    (void) ioctl(SHIN, FIONBIO, (ioctl_t) &tried);
	    tried = 1;		/* prevent looping */
	    goto again;
	}
# endif /* FIONBIO */
# if defined(F_SETFL) && defined(O_NDELAY)
	/*
	 * Someone might have set our file descriptor to non blocking 
	 * From Gray Watson (gray%antr.uucp@med.pitt.edu), Thanks!!!
	 */
	if ((num_read < 0) && (errno == EWOULDBLOCK
#if defined(POSIX) && defined(EAGAIN)
	    || EAGAIN
#endif /* POSIX && EAGAIN */
	    ) && !tried) {
	    (void) fcntl(SHIN, F_SETFL, fcntl(SHIN, F_GETFL, 0) & ~O_NDELAY);
	    tried = 1;
	    goto again;
	}
# endif /* F_SETFL && O_NDELAY */
#endif /* EWOULDBLOCK || (POSIX && EAGAIN) */
#ifdef _SEQUENT_
	if (errno == EINTR || errno == EBADF)
#else /* _SEQUENT_ */
	if (errno == EINTR)
#endif /* _SEQUENT_ */
	    goto again;
	CSHprintf ("GetNextChar(): errno == %d\n", errno);
	*cp = tcp;
	return num_read;
    }
    *cp = tcp;

    return num_read;
}

/*
 * SpellLine - do spelling correction on the entire command line
 * (which may have trailing newline).
 * If cmdonly is set, only check spelling of command words.
 * Return value:
 * -1: Something was incorrectible, and nothing was corrected
 *  0: Everything was correct
 *  1: Something was corrected
 */
static int
SpellLine(cmdonly)
int cmdonly;
{
    int endflag, matchval;
    Char *argptr, *OldCursor, *OldLastChar;

    OldLastChar = LastChar;
    OldCursor = Cursor;
    argptr = InputBuf;
    endflag = 1;
    matchval = 0;
    do {
	while (ismeta(*argptr) || iscmdmeta(*argptr))
	    argptr++;
	for (Cursor = argptr;
	     *Cursor != '\0' && !ismeta(*Cursor) && !iscmdmeta(*Cursor);
	     Cursor++)
	    ;
	if (*Cursor == '\0') {
	    Cursor = LastChar;
	    if (LastChar[-1] == '\n')
		Cursor--;
	    endflag = 0;
	}
	if (!Strchr(mismatch,*argptr) &&
	    (!cmdonly || starting_a_command(argptr, InputBuf))) {
	    switch (tenematch (InputBuf, INBUFSIZ, Cursor-InputBuf, SPELL)) {
	    case 1:		/* corrected */
		matchval = 1;
		break;
	    case -1:		/* couldn't be corrected */
		if (!matchval)
		    matchval = -1;
		break;
	    default:		/* was correct */
		break;
	    }
	    if (LastChar != OldLastChar) {
		if (argptr < OldCursor)
		    OldCursor += (LastChar - OldLastChar);
		OldLastChar = LastChar;
	    }
	}
	argptr = Cursor;
    } while(endflag);
    Cursor = OldCursor;
    return matchval;
}

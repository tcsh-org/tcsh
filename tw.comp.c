/* $Header: /u/christos/src/beta-6.01/RCS/tw.comp.c,v 1.7 1992/03/21 02:46:07 christos Exp $ */
/*
 * tw.comp.c: File completion builtin
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

RCSID("$Id: tw.comp.c,v 1.7 1992/03/21 02:46:07 christos Exp $")

#include "tw.h"
#include "ed.h"
#include "tc.h"

/* #define TDEBUG */
struct varent completions;

static int 	 	  tw_result	__P((Char *, Char *));
static Char		**tw_find	__P((Char *, struct varent *, int));
static Char 		 *tw_tok	__P((Char *));
static bool	 	  tw_pos	__P((Char *, int));
static void	  	  tw_pr		__P((Char **));
static void	  	  tw_match	__P((Char *, Char *));
static void	 	  tw_prlist	__P((struct varent *));

/* docomplete():
 *	Add or list completions in the completion list
 */
/*ARGSUSED*/
void
docomplete(v, t)
    Char **v;
    struct command *t;
{
    register struct varent *vp;
    register Char *p;

    v++;
    p = *v++;
    if (p == 0)
	tw_prlist(&completions);
    else if (*v == 0) {
	vp = adrof1(strip(p), &completions);
	if (vp)
	    tw_pr(vp->vec), xputchar('\n');
    }
    else
	set1(strip(p), saveblk(v), &completions);
} /* end docomplete */


/* douncomplete():
 *	Remove completions from the completion list
 */
/*ARGSUSED*/
void
douncomplete(v, t)
    Char **v;
    struct command *t;
{
    unset1(v, &completions);
} /* end douncomplete */


/* tw_prlist():
 *	Pretty print a list of variables
 */
static void
tw_prlist(p)
    struct varent *p;
{
    register struct varent *c;

    if (setintr)
#ifdef BSDSIGS
	(void) sigsetmask(sigblock((sigmask_t) 0) & ~sigmask(SIGINT));
#else				/* BSDSIGS */
	(void) sigrelse(SIGINT);
#endif				/* BSDSIGS */

    for (;;) {
	while (p->v_left)
	    p = p->v_left;
x:
	if (p->v_parent == 0)	/* is it the header? */
	    return;
	xprintf("%s\t", short2str(p->v_name));
	tw_pr(p->vec);
	xputchar('\n');
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
} /* end tw_prlist */


/* tw_pr():
 *	Pretty print a completion, adding single quotes around 
 *	a completion argument and collapsing multiple spaces to one.
 */
static void
tw_pr(cmp)
    Char **cmp;
{
    bool sp, osp;
    Char *ptr;

    for (; *cmp; cmp++) {
	xputchar('\'');
	for (osp = 0, ptr = *cmp; *ptr; ptr++) {
	    sp = Isspace(*ptr);
	    if (sp && osp)
		continue;
	    xputchar(*ptr);
	    osp = sp;
	}
	xputchar('\'');
	if (cmp[1])
	    xputchar(' ');
    }
} /* end tw_pr */


/* tw_find():
 *	Find the first matching completion. 
 *	For commands we only look at names that start with -
 */
static Char **
tw_find(nam, vp, cmd)
    Char   *nam;
    register struct varent *vp;
    int cmd;
{
    register struct varent *vp1;

    for (vp = vp->v_left; vp; vp = vp->v_right) {
	if (vp->v_left && (vp1 = tw_find(nam, vp, cmd)) != NULL)
	    return vp1;
	if (cmd) {
	    if (vp->name[0] != '-')
		continue;
	    if (Gmatch(nam, &vp->name[1]) && vp->vec != NULL)
		return vp->vec;
	}
	else
	    if (Gmatch(nam, vp->v_name) && vp->vec != NULL)
		return vp->vec;
    }
    return vp;
} /* end tw_find */


/* tw_pos():
 *	Return true if the position is within the specified range
 */
static bool
tw_pos(ran, wno)
    Char *ran;
    int	  wno;
{
    Char *p;

    for (p = ran; *p && *p != '-'; p++)
	continue;

    if (*p == '\0')			/* range == <number> */
	return wno == getn(ran);
    
    if (ran == p)			/* range = - <number> */
	return wno <= getn(&ran[1]);
    *p++ = '\0';

    if (*p == '\0')			/* range = <number> - */
	return getn(ran) <= wno;
    else				/* range = <number> - <number> */
	return (getn(ran) <= wno) && (wno <= getn(p));
	   
} /* end tw_pos */


/* tw_tok():
 *	Return the next word from string, unquoteing it.
 */
static Char *
tw_tok(str)
    Char *str;
{
    static Char *bf = NULL;

    if (str != NULL)
	bf = str;
    
    /* skip leading spaces */
    for (; *bf && Isspace(*bf); bf++)
	continue;

    for (str = bf; *bf && !Isspace(*bf); bf++) {
	if (ismeta(*bf))
	    return NULL;
	*bf = *bf & ~QUOTE;
    }
    if (*bf != '\0')
	*bf++ = '\0';

    return *str ? str : NULL;
} /* end tw_tok */


/* tw_match():
 *	Match a string against the pattern given.
 *	and return the number of matched characters
 *	in a prefix of the string.
 */
static int
tw_match(str, pat)
    Char *str, *pat;
{
    Char *estr;
    (void) Gnmatch(str, pat, &estr);
#ifdef TDEBUG
    xprintf("Gnmatch(%s, ", short2str(str));
    xprintf("%s, ", short2str(pat));
    xprintf("%s) = %d\n", short2str(estr), estr - str);
#endif /* TDEBUG */
    return (estr - str);
}


/* tw_result():
 *	Return what the completion action should be depending on the
 *	string
 */
static int
tw_result(act, pat)
    Char *act, *pat;
{
    int looking;

    switch (act[0] & ~QUOTE) {
    case 'C':
	looking = TW_COMPLETION;
	break;
    case 'S':
	looking = TW_SIGNAL;
	break;
    case 'a':
	looking = TW_ALIAS;
	break;
    case 'b':
	looking = TW_BINDING;
	break;
    case 'c':
	looking = TW_COMMAND;
	break;
    case 'd':
	looking = TW_DIRECTORY;
	break;
    case 'e':
	looking = TW_ENVVAR;
	break;
    case 'f':
	looking = TW_FILE;
	break;
    case 'j':
	looking = TW_JOB;
	break;
    case 'l':
	looking = TW_LIMIT;
	break;
    case 'n':
	looking = TW_NONE;
	break;
    case 's':
	looking = TW_SHELLVAR;
	break;
    case 'v':
	looking = TW_VARIABLE;
	break;
    case 'u':
	looking = TW_USER;
	break;

    case '$':
	copyn(pat, &act[1], MAXPATHLEN);
	(void) strip(pat);
	return(TW_VARLIST);

    case '(':
	copyn(pat, &act[1], MAXPATHLEN);
	if ((act = Strchr(pat, ')')) != NULL)
	    *act = '\0';
	(void) strip(pat);
	return TW_WORDLIST;

    default:
	stderror(ERR_COMPCOM, short2str(act));
	return TW_ZERO;
    }

    switch (act[1] & ~QUOTE) {
    case '\0':
	pat[0] = '\0';
	return looking;

    case ':':
	copyn(pat, &act[2], MAXPATHLEN);
	(void) strip(pat);
	return looking;

    default:
	stderror(ERR_COMPCOM, short2str(act));
	return TW_ZERO;
    }
} /* end tw_result */
		

/* tw_complete():
 *	Return the appropriate completion for the command
 *
 *	valid completion strings are:
 *	p/<range>/<completion>/[<suffix>/]	positional
 *	c/<pattern>/<completion>/[<suffix>/]	current word ignore pattern
 *	C/<pattern>/<completion>/[<suffix>/]	current word with pattern
 *	n/<pattern>/<completion>/[<suffix>/]	next word
 */
int
tw_complete(line, word, pat, looking, suf)
    Char *line, **word, *pat;
    int looking, *suf;
{
    Char buf[MAXPATHLEN + 1], **vec, *cword, *wp, *owp, *ptr;
    int wordno, n;

    copyn(buf, line, MAXPATHLEN);

    /* find the command */
    if ((cword = tw_tok(buf)) == NULL)
	return TW_ZERO;

    wp = strip(buf);

    /*
     * look for hardwired command completions using a globbing
     * search and for arguments using a normal search.
     */
    if ((vec = tw_find(wp, &completions, (looking == TW_COMMAND))) == NULL)
	return looking;

    /* find the last word before the current one */
    wp = owp = cword;
    ptr = tw_tok(NULL);
    for (wordno = 1; ptr != NULL; wordno++) {
	owp = wp;
	wp = ptr;
	ptr = tw_tok(NULL);
    }

    /* if the current word is empty move the last word to the next */
    if (**word == '\0') {
	owp = wp;
	wp = *word;
    }
    else
	wordno--;

#ifdef TDEBUG
    xprintf("\r\n");
    xprintf("wordno: %d\n", wordno);
    xprintf("line: %x %s\n", line,   short2str(line));
    xprintf("cword: %x %s\n", cword,    short2str(cword));
    xprintf("word: %x %s\n", *word,  short2str(*word));
    xprintf("last: %x %s\n", owp,    short2str(owp));
    xprintf("this: %x %s\n", wp,     short2str(wp));
#endif /* TDEBUG */
    
    for (;vec != NULL && (ptr = vec[0]) != NULL; vec++) {
	Char  spc[MAXPATHLEN+1],/* Scratch space 			*/
	     *ran, *eran,	/* The pattern or range X/<range>/XXXX/ */
	     *com, 		/* The completion X/XXXXX/<completion>/ */
	     *pos,		/* scratch pointer 			*/
	      cmd, sep;		/* the command and separator characters */

	if (ptr[0] == '\0')
	    continue;

	copyn(spc, ptr, MAXPATHLEN);	/* make a scratch copy */

#ifdef TDEBUG
	xprintf("match %s\n", short2str(spc));
#endif /* TDEBUG */

	switch (cmd = spc[0]) {
	case 'p':
	case 'n':
	case 'c':
	case 'C':
	    break;
	default:
	    stderror(ERR_COMPILL, "command", cmd);
	    return TW_ZERO;
	}

	sep = spc[1];
	if (!Ispunct(sep)) {
	    stderror(ERR_COMPILL, "separator", sep);
	    return TW_ZERO;
	}

	if ((pos = Strchr(&spc[2], sep)) == NULL) {
	    stderror(ERR_COMPINC, "pattern", short2str(ptr));
	    return TW_ZERO;
	}
	com = pos + 1;	/* pos + 1 points to the completion */
	*pos = '\0';
	/*
	 * move pattern to the beginning of the scratch space
	 * so that we make 2 character more space
	 */
	for (eran = spc; eran <= pos - 2; eran++)
	     eran[0] = eran[2];
	eran--;
	ran = spc;

	if ((pos = Strchr(com, sep)) == NULL) {
	    stderror(ERR_COMPINC, "completion", short2str(ptr));
	    return TW_ZERO;
	}

	*pos++ = '\0';
	if (*pos != '\0') {
	    if (*pos == sep)
		*suf = -1;
	    else
		*suf = *pos;
	}

#ifdef TDEBUG
	xprintf("command:    %c\nseparator:  %c\n", cmd, sep);
	xprintf("pattern:    %s\n", short2str(ran));
	xprintf("completion: %s\n", short2str(com));
	xprintf("suffix:     ");
        switch (*suf) {
	case 0:
	    xprintf("*auto suffix*\n");
	    break;
	case -1:
	    xprintf("*no suffix*\n");
	    break;
	default:
	    xprintf("%c\n", *suf);
	    break;
	}
#endif /* TDEBUG */

	switch (cmd) {
	case 'p':			/* positional completion */
#ifdef TDEBUG
	    xprintf("p: tw_pos(%s, %d) = ", short2str(ran), wordno);
	    xprintf("%d\n", tw_pos(ran, wordno));
#endif /* TDEBUG */
	    if (!tw_pos(ran, wordno))
		continue;
	    return tw_result(com, pat);

	case 'c':			/* match with the current word */
	case 'C':
#ifdef TDEBUG
	    xprintf("%c: ", cmd);
#endif /* TDEBUG */
	    if ((n = tw_match(wp, ran)) == 0)
		continue;
	    if (cmd != 'C')
		*word += n;
	    return tw_result(com, pat);

	case 'n':			/* match with the next word */
#ifdef TDEBUG
	    xprintf("n: ");
#endif /* TDEBUG */
	    if (tw_match(owp, ran) == 0)
		continue;
	    return tw_result(com, pat);

	default:
	    return TW_ZERO;	/* Cannot happen */
	}
    }
    return TW_ZERO;
} /* end tw_complete */

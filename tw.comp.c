/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.01/RCS/tw.comp.c,v 1.3 1992/02/13 05:28:51 christos Exp $ */
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

RCSID("$Id: tw.comp.c,v 1.3 1992/02/13 05:28:51 christos Exp $")

#include "tw.h"
#include "ed.h"
#include "tc.h"

/* #define TDEBUG */
static struct varent completions;

static int 	 tw_result	__P((Char *, Char *));
static Char 	*tw_tok		__P((Char *));

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
	plist(&completions);
    else if (*v == 0) {
	vp = adrof1(strip(p), &completions);
	if (vp)
	    blkpr(vp->vec), xprintf("\n");
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


/* tw_result():
 *	Return what the completion action should be depending on the
 *	string
 */
static int
tw_result(act, pat)
    Char *act, *pat;
{
    int looking;

    if ((act[0] & ~QUOTE) == '=') {
	switch (act[1] & ~QUOTE) {
	case 'a':
	    looking = TW_ALIAS;
	    break;
	case 'e':
	    looking = TW_ENVVAR;
	    break;
	case 's':
	    looking = TW_SHELLVAR;
	    break;
	case 'v':
	    looking = TW_VARIABLE;
	    break;
	case 'f':
	    looking = TW_FILE;
	    break;
	case 'd':
	    looking = TW_DIRECTORY;
	    break;
	case 'u':
	    looking = TW_USER;
	    break;
	case 'c':
	    looking = TW_COMMAND;
	    break;
	case 'b':
	    looking = TW_BINDING;
	    break;

	case '$':
	    copyn(pat, &act[2], MAXPATHLEN);
	    (void) strip(pat);
	    return(TW_VARLIST);

	case '(':
	    copyn(pat, &act[2], MAXPATHLEN);
	    if ((act = Strchr(pat, ')')) != NULL)
		*act = '\0';
	    (void) strip(pat);
	    return TW_WORDLIST;

	default:
	    copyn(pat, act, MAXPATHLEN);
	    (void) strip(pat);
	    return(TW_LITERAL);
	}

	if ((act[2] & ~QUOTE) == ':') {
	    copyn(pat, &act[3], MAXPATHLEN);
	    (void) strip(pat);
	}

	return looking;
    }
    copyn(pat, act, MAXPATHLEN);
    (void) strip(pat);
    return(TW_LITERAL);
} /* end tw_result */
		

/* tw_complete():
 *	Return the appropriate completion for the command
 */
int
tw_complete(line, wstart, word, pat)
    Char *line, *wstart, **word, *pat;
{
    Char buf[MAXPATHLEN + 1], **vec, *cmd, *wp, *owp, *ptr;
    struct varent *vp;
    int res, plen = 0;

    copyn(buf, line, MAXPATHLEN);

    /* find the command */
    if ((cmd = tw_tok(buf)) == NULL)
	return TW_ZERO;

    if ((vp = adrof1(strip(buf), &completions)) == NULL)
	return TW_ZERO;

    if ((vec = vp->vec) == NULL || *vec == NULL)
	return TW_ZERO;
    
    /* find the last word before the current one */
    wp = owp = cmd;
    ptr = tw_tok(NULL);
    while (ptr != NULL) {
	owp = wp;
	wp = ptr;
	ptr = tw_tok(NULL);
    }

    /* if the current word is empty move the last word to the next */
    if (**word == '\0') {
	owp = wp;
	wp = *word;
    }

#ifdef TDEBUG
    xprintf("\r\n");
    xprintf("line: %x %s\n", line,   short2str(line));
    xprintf(" cmd: %x %s\n", cmd,    short2str(cmd));
    xprintf("wstr: %x %s\n", wstart, short2str(wstart));
    xprintf("word: %x %s\n", *word,   short2str(*word));
    xprintf("last: %x %s\n", owp,    short2str(owp));
    xprintf("this: %x %s\n", wp,     short2str(wp));
#endif /* TDEBUG */
    
    for (res = TW_ZERO; vec != NULL && (ptr = vec[0]) != NULL; vec++) {
	Char buf[MAXPATHLEN + 1], *pos, *mat;

	if (ptr[0] == '\0')
	    continue;
#ifdef TDEBUG
	xprintf("match %s\n", short2str(ptr));
#endif /* TDEBUG */
	copyn(buf, ptr, MAXPATHLEN);
	plen = 0;
	cmd = ptr;
	if ((pos = Strchr(buf, ',')) != NULL && pos[1] == '=') {
	    cmd = &ptr[pos - buf + 1];
	    mat = buf;
	    *pos = '\0';
	    /*
	     * If the pattern did not end with a space, the match
	     * against the current word otherwise the match is 
	     * against the previous word.
	     */
	    if (pos != buf) {
		if (Isspace(pos[-1])) {
		    /* match is with the previous word */
		    pos[-1] = '\0';
#ifdef TDEBUG
		    xprintf("prev Gmatch(%s, ", short2str(owp));
		    xprintf("%s) = ", short2str(mat));
		    xprintf("%d\n", Gmatch(owp, mat));
#endif /* TDEBUG */
		    if (!Gmatch(owp, mat))
			continue;
		}
		else {
#ifdef TDEBUG
		    xprintf("curr Gmatch(%s, ", short2str(wp));
		    xprintf("%s) = ", short2str(mat));
		    xprintf("%d\n", Gmatch(owp, mat));
#endif /* TDEBUG */
		    plen = pos - buf;
		    *pos++ = '*';
		    *pos = '\0';
		    /* match is with this word */
		    if (!Gmatch(wp, mat))
			continue;
		}
	    }
	}
#ifdef TDEBUG
	xprintf("okvec = %s\n", short2str(cmd));
#endif /* TDEBUG */
	res = tw_result(cmd, pat);
	break;
    }
    if (res != TW_ZERO)
	*word += plen;
    return res;
} /* end tw_complete */

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.01/RCS/tw.comp.c,v 1.1 1992/01/16 13:07:59 christos Exp $ */
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

RCSID("$Id: tw.comp.c,v 1.1 1992/01/16 13:07:59 christos Exp $")

#include "tw.h"
#include "ed.h"
#include "tc.h"

/* #define TDEBUG */
static struct varent completions;

static int 	tw_result	__P((Char *, Char *));
static void	tw_tok		__P((Char **, Char *));

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
static void
tw_tok(str, buf)
    Char **str;
    Char *buf;
{
    Char *ptr = *str;

    while (*ptr && !Isspace(*ptr))
	*buf++ = *ptr++ & ~QUOTE;
    *buf = '\0';
    while (*ptr && Isspace(*ptr))
	ptr++;
    *str = ptr;
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
	case '$':
	    copyn(pat, &act[2], MAXPATHLEN);
	    (void) strip(pat);
	    return(TW_VARLIST);
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
tw_complete(cmd, word, pat)
    Char *cmd, *word, *pat;
{
    Char buf[MAXPATHLEN + 1], **vec;
    struct varent *vp;

    tw_tok(&cmd, buf); 

#ifdef TDEBUG
    xprintf(" cmd: %x %s\n", cmd, short2str(buf));
    xprintf("word: %x %s\n", word, short2str(word));
#endif

    if ((vp = adrof1(strip(buf), &completions)) == NULL)
	return TW_ZERO;

    if ((vec = vp->vec) == NULL || *vec == NULL)
	return TW_ZERO;

    /*
     * Now match the arguments
     */
    for (;;) {
	if (cmd >= word) 
	    /* we are done */
	    return tw_result(*vec, pat);
	tw_tok(&cmd, buf);
#ifdef TDEBUG
	xprintf("cmd %x word %x token = %s\n", cmd, word, short2str(buf));
	xprintf("Gmatch(%s, ", short2str(buf));
	xprintf("%s) = %d\n", short2str(*vec), Gmatch(buf, *vec));
#endif
	if (vec[0][0] == '=' || Gmatch(buf, *vec)) {
	    if (vec[1] != NULL)
		vec++;
	}
    }
} /* end tw_complete */

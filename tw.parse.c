/* $Header: /u/christos/src/tcsh-6.02/RCS/tw.parse.c,v 3.33 1992/05/09 04:03:53 christos Exp $ */
/*
 * tw.parse.c: Everyone has taken a shot in this futile effort to
 *	       lexically analyze a csh line... Well we cannot good
 *	       a job as good as sh.lex.c; but we try. Amazing that
 *	       it works considering how many hands have touched this code
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

RCSID("$Id: tw.parse.c,v 3.33 1992/05/09 04:03:53 christos Exp $")

#include "tw.h"
#include "ed.h"
#include "tc.h"

#define EVEN(x) (((x) & 1) != 1)

#define DOT_NONE	0	/* Don't display dot files		*/
#define DOT_NOT		1	/* Don't display dot or dot-dot		*/
#define DOT_ALL		2	/* Display all dot files		*/

/*  TW_NONE,	       TW_COMMAND,     TW_VARIABLE,    TW_LOGNAME,	*/
/*  TW_FILE,	       TW_DIRECTORY,   TW_VARLIST,     TW_USER,		*/
/*  TW_COMPLETION,     TW_ALIAS,       TW_SHELLVAR,    TW_ENVVAR,	*/
/*  TW_BINDING,        TW_WORDLIST,    TW_LIMIT,       TW_SIGNAL	*/
/*  TW_JOB,	       TW_EXPLAIN					*/
static void (*tw_start_entry[]) __P((DIR *, Char *)) = {
    tw_file_start,     tw_cmd_start,   tw_var_start,   tw_logname_start, 
    tw_file_start,     tw_file_start,  tw_vl_start,    tw_logname_start, 
    tw_complete_start, tw_alias_start, tw_var_start,   tw_var_start,     
    tw_bind_start,     tw_wl_start,    tw_limit_start, tw_sig_start,
    tw_job_start,      tw_file_start
};

static Char * (*tw_next_entry[]) __P((Char *, int *)) = {
    tw_file_next,      tw_cmd_next,    tw_var_next,    tw_logname_next,  
    tw_file_next,      tw_file_next,   tw_var_next,    tw_logname_next,  
    tw_var_next,       tw_var_next,    tw_shvar_next,  tw_envvar_next,   
    tw_bind_next,      tw_wl_next,     tw_limit_next,  tw_sig_next,
    tw_job_next,       tw_file_next
};

static void (*tw_end_entry[]) __P((void)) = {
    tw_dir_end,        tw_dir_end,     tw_dir_end,    tw_logname_end,
    tw_dir_end,        tw_dir_end,     tw_dir_end,    tw_logname_end, 
    tw_dir_end,        tw_dir_end,     tw_dir_end,    tw_dir_end,
    tw_dir_end,        tw_dir_end,     tw_dir_end,    tw_dir_end,
    tw_dir_end,	       tw_dir_end
};

/* #define TDEBUG */

/* Set to TRUE if recexact is set and an exact match is found
 * along with other, longer, matches.
 */
int non_unique_match = FALSE;
static bool SearchNoDirErr = 0;	/* t_search returns -2 if dir is unreadable */

/* state so if a completion is interrupted, the input line doesn't get
   nuked */
int InsideCompletion = 0;

/* do the expand or list on the command line -- SHOULD BE REPLACED */

extern Char NeedsRedraw;	/* from ed.h */
extern int Tty_raw_mode;
extern int TermH;		/* from the editor routines */
extern int lbuffed;		/* from sh.print.c */

static	void	 extract_dir_and_name	__P((Char *, Char *, Char *));
static	Char	*quote_meta		__P((Char *, int, bool));
static	Char	*dollar			__P((Char *, Char *));
static	Char	*tilde			__P((Char *, Char *));
static  int      expand_dir		__P((Char *, Char *, DIR  **, COMMAND));
static	bool	 nostat			__P((Char *));
static	Char	 filetype		__P((Char *, Char *));
static	int	 t_glob			__P((Char ***, int));
static	int	 c_glob			__P((Char ***));
static	int	 is_prefix		__P((Char *, Char *));
static	int	 is_suffix		__P((Char *, Char *));
static	int	 recognize		__P((Char *, Char *, int, int));
static	int	 ignored		__P((Char *));
static	int	 isadirectory		__P((Char *, Char *));
static  int      tw_collect_items	__P((COMMAND, int, Char *, Char *, 
					     Char *, Char *, int));
static  int      tw_collect		__P((COMMAND, int, Char *, Char *, 
					     Char *, Char *, int, DIR *));
static	Char 	 tw_suffix		__P((int, Char *, Char *, Char *, 
					     Char *));
static	void 	 tw_fixword		__P((int, Char *, Char *, Char *, int));
static	void	 tw_list_items		__P((int, int, int));

#ifdef notdef
/*
 * If we find a set command, then we break a=b to a= and word becomes
 * b else, we don't break a=b. [don't use that; splits words badly and
 * messes up tw_complete()]
 */
#define isaset(c, w) ((w)[-1] == '=' && \
		      ((c)[0] == 's' && (c)[1] == 'e' && (c)[2] == 't' && \
		       ((c[3] == ' ' || (c)[3] == '\t'))))
#endif

#define QLINESIZE (INBUFSIZE + 1)

/* tenematch():
 *	Return:
 *		> 1:    No. of items found
 *		= 1:    Exactly one match / spelling corrected
 *		= 0:    No match / spelling was correct
 *		< 0:    Error (incl spelling correction impossible)
 */
int
tenematch(inputline, num_read, command)
    Char   *inputline;		/* match string prefix */
    int     num_read;		/* # actually in inputline */
    COMMAND command;		/* LIST or RECOGNIZE or PRINT_HELP */

{
    Char    qline[QLINESIZE];
    Char    qu = 0, *pat = STRNULL;
    Char   *str_end, *cp, *wp, *wordp;
    Char   *cmd_start, *word_start, *word;
    Char   *ocmd_start = NULL, *oword_start = NULL, *oword = NULL;
    int	    suf = 0;
    int     space_left;
    int     looking;		/* what we are looking for		*/
    int     search_ret;		/* what search returned for debugging 	*/
    int     backq = 0;

    if (num_read > QLINESIZE - 1)
	return -1;
    str_end = &inputline[num_read];

    word_start = inputline;
    word = cmd_start = wp = qline;
    for (cp = inputline; cp < str_end; cp++) {
        if (!cmap(qu, _ESC)) {
	    if (cmap(*cp, _Q|_ESC)) {
		if (qu == 0 || qu == *cp) {
		    qu ^= *cp;
		    continue;
		}
	    }
	    if (qu != '\'' && cmap(*cp, _Q1)) {
		if (backq ^= 1) {
		    ocmd_start = cmd_start;
		    oword_start = word_start;
		    oword = word;
		    word_start = cp + 1;
		    word = cmd_start = wp + 1;
		}
		else {
		    cmd_start = ocmd_start;
		    word_start = oword_start;
		    word = oword;
		}
		*wp++ = *cp;
		continue;
	    }
	}
	if (iscmdmeta(*cp))
	    cmd_start = wp + 1;
	/* Don't quote '/' to make the recognize stuff work easily */
	/* Don't quote '$' in double quotes */
	*wp = *cp | (qu && *cp != '/'  && (*cp != '$' || qu != '"') ? 
		     QUOTE : 0);
	if (ismetahash(*wp) /* || isaset(cmd_start, wp + 1) */)
	    word = wp + 1, word_start = cp + 1;
	wp++;
	if (cmap(qu, _ESC))
	    qu = 0;
      }
    *wp = 0;

    /* move the word_start after the quote */
    if (*word_start == qu)
	word_start++;

#ifdef masscomp
    /*
     * Avoid a nasty message from the RTU 4.1A & RTU 5.0 compiler concerning
     * the "overuse of registers". According to the compiler release notes,
     * incorrect code may be produced unless the offending expression is
     * rewritten. Therefore, we can't just ignore it, DAS DEC-90.
     */
    space_left = QLINESIZE - 1;
    space_left -= word - qline;
#else
    space_left = QLINESIZE - 1 - (word - qline);
#endif

    looking = starting_a_command(word - 1, qline) ? 
	TW_COMMAND : TW_ZERO;
    wordp = word;

#ifdef TDEBUG
    xprintf("starting_a_command %d\n", looking);
    xprintf("\ncmd_start:%S:\n", cmd_start);
    xprintf("qline:%S:\n", qline);
    xprintf("qline:");
    for (wp = qline; *wp; wp++)
	xprintf("%c", *wp & QUOTE ? '-' : ' ');
    xprintf(":\n");
    xprintf("word:%S:\n", word);
    xprintf("word:");
    /* Must be last, so wp is still pointing to the end of word */
    for (wp = word; *wp; wp++)
	xprintf("%c", *wp & QUOTE ? '-' : ' ');
    xprintf(":\n");
#endif

#ifdef TDEBUG
    xprintf("complete %d ", looking);
#endif
    looking = tw_complete(cmd_start, &wordp, &pat, looking, &suf);
#ifdef TDEBUG
    xprintf("complete %d %S\n", looking, pat);
#endif

    switch ((int) command) {
	Char    buffer[FILSIZ + 1], *bptr;
	Char   *slshp;
	Char   *items[2], **ptr;
	int     i, count;

    case RECOGNIZE:
	if (adrof(STRautocorrect)) {
	    if ((slshp = Strrchr(wordp, '/')) != NULL && slshp[1] != '\0') {
		SearchNoDirErr = 1;
		for (bptr = wordp; bptr < slshp; bptr++) {
		    /*
		     * do not try to correct spelling of words containing
		     * globbing characters
		     */
		    if (isglob(*bptr)) {
			SearchNoDirErr = 0;
			break;
		    }
		}
	    }
	}
	else
	    slshp = STRNULL;
	search_ret = t_search(wordp, wp, command, space_left, looking, 1, 
			      pat, suf);
	SearchNoDirErr = 0;

	if (search_ret == -2) {
	    Char    rword[FILSIZ + 1];

	    (void) Strcpy(rword, slshp);
	    if (slshp != STRNULL)
		*slshp = '\0';
	    search_ret = spell_me(wordp, QLINESIZE - (wordp - qline), looking);
	    if (search_ret == 1) {
		/* get rid of old word */
		DeleteBack(str_end - word_start);
		(void) Strcat(wordp, rword);
		/* insert newly spelled word */
		if (InsertStr(quote_meta(wordp, qu, 0)) < 0)	
		    return -1;	/* error inserting */
		wp = wordp + Strlen(wordp);
		search_ret = t_search(wordp, wp, command, space_left,
				      looking, 1, pat, suf);
	    }
	}

	/*
	 * Change by Christos Zoulas: if the name has metachars in it, quote
	 * the metachars, but only if we are outside quotes.
	 * We don't quote the last space if we had a unique match and 
	 * addsuffix was set. Otherwise the last space was part of a word.
	 */
	if (*wp && InsertStr(quote_meta(wp, qu, search_ret == 1 &&
					     (bool) is_set(STRaddsuffix))) < 0)
	    /* put it in the input buffer */
	    return -1;		/* error inserting */
	return search_ret;

    case SPELL:
	for (bptr = word_start; bptr < str_end; bptr++) {
	    /*
	     * do not try to correct spelling of words containing globbing
	     * characters
	     */
	    if (isglob(*bptr))
		return 0;
	}
	search_ret = spell_me(wordp, QLINESIZE - (wordp - qline), looking);
	if (search_ret == 1) {
	    /* get rid of old word */
	    DeleteBack(str_end - word_start);	
	    /* insert newly spelled word */
	    if (InsertStr(quote_meta(wordp, qu, 0)) < 0)
		return -1;	/* error inserting */
	}
	return search_ret;

    case PRINT_HELP:
	do_help(cmd_start);
	return 1;

    case GLOB:
    case GLOB_EXPAND:
	(void) Strncpy(buffer, wordp, FILSIZ + 1);
	items[0] = buffer;
	items[1] = NULL;
	ptr = items;
	count = (looking == TW_COMMAND && Strchr(wordp, '/') == 0) ? 
		c_glob(&ptr) : 
		t_glob(&ptr, looking == TW_COMMAND);
	if (count > 0) {
	    if (command == GLOB)
		print_by_column(STRNULL, ptr, count, 0);
	    else {
		DeleteBack(str_end - word_start);/* get rid of old word */
		for (i = 0; i < count; i++)
		    if (ptr[i] && *ptr[i]) {
			if (InsertStr(quote_meta(ptr[i], qu, 0)) < 0 ||
			    InsertStr(STRspace) < 0) {
			    blkfree(ptr);
			    return (-1);
			}
		    }
	    }
	    blkfree(ptr);
	}
	return count;

    case VARS_EXPAND:
	if (dollar(buffer, wordp)) {
	    DeleteBack(str_end - word_start);
	    if (InsertStr(quote_meta(buffer, qu, 0)) < 0)
		return (-1);
	    return (1);
	}
	return (0);

    case PATH_NORMALIZE:
	if ((bptr = dnormalize(wordp, symlinks == SYM_IGNORE ||
				      symlinks == SYM_EXPAND)) != NULL) {
	    (void) Strcpy(buffer, bptr);
	    xfree((ptr_t) bptr);
	    DeleteBack(str_end - word_start);
	    if (InsertStr(quote_meta(buffer, qu, 0)) < 0)
		return (-1);
	    return (1);
	}
	return (0);

    case LIST:
	search_ret = t_search(wordp, wp, command, space_left, looking, 1, 
			      pat, suf);
	return search_ret;

    default:
	xprintf("tcsh: Internal match error.\n");
	return 1;

    }
} /* end tenematch */


/* t_glob():
 * 	Return a list of files that match the pattern
 */
static int
t_glob(v, cmd)
    register Char ***v;
    int cmd;
{
    jmp_buf_t osetexit;

    if (**v == 0)
	return (0);
    gflag = 0, tglob(*v);
    if (gflag) {
	getexit(osetexit);	/* make sure to come back here */
	if (setexit() == 0)
	    *v = globall(*v);
	resexit(osetexit);
	gargv = 0;
	if (haderr) {
	    haderr = 0;
	    NeedsRedraw = 1;
	    return (-1);
	}
	if (*v == 0)
	    return (0);
    }
    else
	return (0);

    if (cmd) {
	Char **av = *v, *p;
	int fwd, i, ac = gargc;

	for (i = 0, fwd = 0; i < ac; i++) 
	    if (!executable(NULL, av[i], 0)) {
		fwd++;		
		p = av[i];
		av[i] = NULL;
		xfree((ptr_t) p);
	    }
	    else if (fwd) 
		av[i - fwd] = av[i];

	if (fwd)
	    av[i - fwd] = av[i];
	gargc -= fwd;
	av[gargc] = NULL;
    }

    return (gargc);
} /* end t_glob */


/* c_glob():
 * 	Return a list of commands that match the pattern
 */
static int
c_glob(v)
    register Char ***v;
{
    Char *pat = **v, *cmd, **av;
    Char dir[MAXPATHLEN+1];
    int flag, at, ac;

    if (pat == NULL)
	return (0);

    ac = 0;
    at = 10;
    av = (Char **) xmalloc((size_t) (at * sizeof(Char *)));
    av[ac] = NULL;

    tw_cmd_start(NULL, NULL);
    while ((cmd = tw_cmd_next(dir, &flag)) != NULL) 
	if (Gmatch(cmd, pat)) {
	    if (ac + 1 >= at) {
		at += 10;
		av = (Char **) xrealloc((ptr_t) av, 
					(size_t) (at * sizeof(Char *)));
	    }
	    av[ac++] = Strsave(cmd);
	    av[ac] = NULL;
	}
    tw_dir_end();
    *v = av;

    return (ac);
} /* end c_glob */


/* quote_meta():
 *	quote (\) the meta-characters in a word
 *	except trailing space if trail_space is set
 *	return pointer to quoted word in static storage
 */
static Char *
quote_meta(word, qu, trail_space)
    Char   *word;
    int     qu;
    bool    trail_space;
{
    static Char buffer[2 * FILSIZ + 1], *bptr, *wptr;

    for (bptr = buffer, wptr = word; *wptr != '\0';) {
	if (bptr > buffer + 2 * FILSIZ - 4)
	    break;
	  
	if (qu && *wptr == qu) {
	    *bptr++ = qu;
	    *bptr++ = '\\';
	    *bptr++ = qu;
	}
	if (!qu &&
	    (cmap(*wptr, _META | _DOL | _Q | _Q1 | _ESC | _GLOB) ||
	     *wptr == HIST || *wptr == HISTSUB) &&
	    (*wptr != ' ' || !trail_space || *(wptr + 1) != '\0') &&
             *wptr != '#')
	    *bptr++ = '\\';
	*bptr++ = *wptr++;
	if (cmap(qu, _ESC))
	  qu = 0;
    }
    *bptr = '\0';
    return (buffer);
} /* end quote_meta */



/* is_prefix():
 *	return true if check matches initial chars in template
 *	This differs from PWB imatch in that if check is null
 *	it matches anything
 */
static int
is_prefix(check, template)
    register Char *check, *template;
{
    for (; *check; check++, template++)
	if ((*check & TRIM) != (*template & TRIM))
	    return (FALSE);
    return (TRUE);
} /* end is_prefix */


/* is_suffix():
 *	Return true if the chars in template appear at the
 *	end of check, I.e., are it's suffix.
 */
static int
is_suffix(check, template)
    register Char *check, *template;
{
    register Char *t, *c;

    for (t = template; *t++;)
	continue;
    for (c = check; *c++;)
	continue;
    for (;;) {
	if (t == template)
	    return 1;
	--t;
	--c;
	if (c == check || (*t & TRIM) != (*c & TRIM))
	    return 0;
    }
} /* end is_suffix */


/* ignored():
 *	Return true if this is an ignored entry
 */
static int
ignored(entry)
    register Char *entry;
{
    struct varent *vp;
    register Char **cp;

    if ((vp = adrof(STRfignore)) == NULL || (cp = vp->vec) == NULL)
	return (FALSE);
    for (; *cp != NULL; cp++)
	if (is_suffix(entry, *cp))
	    return (TRUE);
    return (FALSE);
} /* end ignored */



/* starting_a_command():
 *	return true if the command starting at wordstart is a command
 */
int
starting_a_command(wordstart, inputline)
    register Char *wordstart, *inputline;
{
    register Char *ptr, *ncmdstart;
    int     count;
    static  Char
            cmdstart[] = {'`', ';', '&', '(', '|', '\0'},
            cmdalive[] = {' ', '\t', '\'', '"', '<', '>', '\0'};

    /*
     * Find if the number of backquotes is odd or even.
     */
    for (ptr = wordstart, count = 0;
	 ptr >= inputline;
	 count += (*ptr-- == '`'))
	continue;
    /*
     * if the number of backquotes is even don't include the backquote char in
     * the list of command starting delimiters [if it is zero, then it does not
     * matter]
     */
    ncmdstart = cmdstart + EVEN(count);

    /*
     * look for the characters previous to this word if we find a command
     * starting delimiter we break. if we find whitespace and another previous
     * word then we are not a command
     * 
     * count is our state machine: 0 looking for anything 1 found white-space
     * looking for non-ws
     */
    for (count = 0; wordstart >= inputline; wordstart--) {
	if (*wordstart == '\0')
	    continue;
	if (Strchr(ncmdstart, *wordstart))
	    break;
	/*
	 * found white space
	 */
	if ((ptr = Strchr(cmdalive, *wordstart)) != NULL)
	    count = 1;
	if (count == 1 && !ptr)
	    return (FALSE);
    }

    if (wordstart > inputline)
	switch (*wordstart) {
	case '&':		/* Look for >& */
	    while (wordstart > inputline &&
		   (*--wordstart == ' ' || *wordstart == '\t'))
		continue;
	    if (*wordstart == '>')
		return (FALSE);
	    break;
	case '(':		/* check for foreach, if etc. */
	    while (wordstart > inputline &&
		   (*--wordstart == ' ' || *wordstart == '\t'))
		continue;
	    if (!iscmdmeta(*wordstart) &&
		(*wordstart != ' ' && *wordstart != '\t'))
		return (FALSE);
	    break;
	default:
	    break;
	}
    return (TRUE);
} /* end starting_a_command */


/* recognize():
 *	Object: extend what user typed up to an ambiguity.
 *	Algorithm:
 *	On first match, copy full entry (assume it'll be the only match)
 *	On subsequent matches, shorten exp_name to the first
 *	character mismatch between exp_name and entry.
 *	If we shorten it back to the prefix length, stop searching.
 */
static int
recognize(exp_name, entry, name_length, numitems)
    Char   *exp_name, *entry;
    int     name_length, numitems;
{
    if (numitems == 1)		/* 1st match */
	copyn(exp_name, entry, MAXNAMLEN);
    else {			/* 2nd and subsequent matches */
	register Char *x, *ent;
	register int len = 0;

	for (x = exp_name, ent = entry;
	     *x && (*x & TRIM) == (*ent & TRIM); x++, len++, ent++)
	    continue;
	*x = '\0';		/* Shorten at 1st char diff */
	if (len == name_length)	/* Ambiguous to prefix? */
	    return (-1);	/* So stop now and save time */
    }
    return (0);
} /* end recognize */


/* tw_collect_items():
 *	Collect items that match target.
 *	SPELL command:
 *		Returns the spelling distance of the closest match.
 *	else
 *		Returns the number of items found.
 *		If none found, but some ignored items were found,
 *		It returns the -number of ignored items.
 */
static int
tw_collect_items(command, looking, exp_dir, exp_name, target, pat, flags)
    COMMAND command;
    int looking;
    Char *exp_dir, *exp_name, *target, *pat;
    int flags;

{
    int done = FALSE;			 /* Search is done */
    int showdots;			 /* Style to show dot files */
    int nignored = 0;			 /* Number of fignored items */
    int numitems = 0;			 /* Number of matched items */
    int	name_length = Strlen(target);	 /* Length of prefix (file name) */
    int exec_check = flags & TW_EXEC_CHK;/* need to check executability	*/
    int dir_check  = flags & TW_DIR_CHK; /* Need to check for directories */
    int dir_ok     = flags & TW_DIR_OK;  /* Ignore directories? */
    int gpat       = flags & TW_PAT_OK;	 /* Match against a pattern */
    int ignoring   = flags & TW_IGN_OK;	 /* Use fignore? */
    int d = 4, nd = 0;			 /* Spelling distance */
    Char *entry, *ptr;
    Char buf[MAXPATHLEN+1];
    struct varent *vp;
    int len;
    flags = 0;

    if ((vp = adrof(STRshowdots)) != NULL) {
	if (vp->vec[0][0] == '-' && vp->vec[0][1] == 'A' &&
	    vp->vec[0][2] == '\0')
	    showdots = DOT_NOT;
	else
	    showdots = DOT_ALL;
    }
    else
	showdots = DOT_NONE;

    while (!done && (entry = (*tw_next_entry[looking])(exp_dir, &flags))) {
#ifdef TDEBUG
	xprintf("entry = %S\n", entry);
#endif
	switch (looking) {
	case TW_FILE:
	case TW_DIRECTORY:
	    /*
	     * Don't match . files on null prefix match
	     */
	    if (showdots == DOT_NOT && (ISDOT(entry) || ISDOTDOT(entry)))
		done = TRUE;
	    if (name_length == 0 && entry[0] == '.' && showdots == DOT_NONE)
		done = TRUE;
	    break;

	case TW_COMMAND:
	    exec_check = flags & TW_EXEC_CHK;
	    dir_ok = flags & TW_DIR_OK;
	    break;

	default:
	    break;
	}

	if (done) {
	    done = FALSE;
	    continue;
	}

	switch (command) {

	case SPELL:		/* correct the spelling of the last bit */
	    if (name_length == 0) {/* zero-length word can't be misspelled */
		exp_name[0] = '\0';/* (not trying is important for ~) */
		d = 0;
		done = TRUE;
		break;
	    }
	    if (gpat && !Gmatch(entry, pat))
		break;
	    nd = spdist(entry, target);	/* test the entry against original */
	    if (nd <= d && nd != 4) {
		if (!(exec_check && !executable(exp_dir, entry, dir_ok))) {
		    (void) Strcpy(exp_name, entry);
		    d = nd;
		    if (d == 0)	/* if found it exactly */
			done = TRUE;
		}
	    }
	    else if (nd == 4) {
		if (spdir(exp_name, exp_dir, entry, target)) {
		    if (exec_check && !executable(exp_dir, exp_name, dir_ok)) 
			break;
#ifdef notdef
		    /*
		     * We don't want to stop immediately, because
		     * we might find an exact/better match later.
		     */
		    d = 0;
		    done = TRUE;
#endif
		    d = 3;
		}
	    }
	    break;

	case LIST:
	case RECOGNIZE:

	    if (!is_prefix(target, entry)) 
		break;

	    if (exec_check && !executable(exp_dir, entry, dir_ok))
		break;

	    if (dir_check && !isadirectory(exp_dir, entry))
		break;

	    if (gpat && !Gmatch(entry, pat) && !isadirectory(exp_dir, entry))
		break;

	    /*
	     * Remove duplicates in command listing and completion
	     */
	    if (looking == TW_COMMAND || command == LIST) {
		copyn(buf, entry, MAXPATHLEN);
		len = Strlen(buf);
		switch (looking) {
		case TW_COMMAND:
		    if (!(dir_ok && exec_check))
			break;
		    if (filetype(exp_dir, entry) == '/') {
			buf[len++] = '/';
			buf[len] = '\0';
		    }
		    break;

		case TW_FILE:
		case TW_DIRECTORY:
		    buf[len++] = filetype(exp_dir, entry);
		    buf[len] = '\0';
		    break;

		default:
		    break;
		}
		if (looking == TW_COMMAND && tw_item_find(buf))
		    break;
		else {
		    /* maximum length 1 (NULL) + 1 (~ or $) + 1 (filetype) */
		    ptr = tw_item_add(len + 3);
		    copyn(ptr, buf, MAXPATHLEN);
		    if (command == LIST)
			numitems++;
		}
	    }
		    
	    if (command == RECOGNIZE) {
		if (ignoring && ignored(entry)) {
		    nignored++;
		    break;
		}
		if (is_set(STRrecexact)) {
		    if (StrQcmp(target, entry) == 0) {	/* EXACT match */
			copyn(exp_name, entry, MAXNAMLEN);
			numitems = 1;	/* fake into expanding */
			non_unique_match = TRUE;
			done = TRUE;
			break;
		    }
		}
		if (recognize(exp_name, entry, name_length, ++numitems)) 
		    done = TRUE;
	    }
	    break;

	default:
	    break;
	}
#ifdef TDEBUG
	xprintf("done entry = %S\n", entry);
#endif
    }
    if (command == SPELL)
	return d;
    else {
	if (ignoring && numitems == 0 && nignored > 0) 
	    return -nignored;
	else
	    return numitems;
    }
}


/* tw_suffix():
 *	Find and return the appropriate suffix character
 */
static Char 
tw_suffix(looking, exp_dir, exp_name, target, name)
    int looking;
    Char *exp_dir, *exp_name, *target, *name;
{    
    Char *ptr;
    struct varent *vp;

    switch (looking) {

    case TW_LOGNAME:
	return '/';

    case TW_VARIABLE:
	/*
	 * Don't consider array variables or empty variables
	 */
	if ((vp = adrof(exp_name)) != NULL) {
	    if ((ptr = vp->vec[0]) != NULL || vp->vec[0][0] == '\0' ||
		vp->vec[1]) 
		return ' ';
	    else
		ptr = vp->vec[0];
	}
	else if ((ptr = Getenv(exp_name)) == NULL)
	    return ' ';
	*--target = '\0';
	(void) Strcat(exp_dir, name);
	return isadirectory(exp_dir, ptr) ? '/' : ' ';


    case TW_DIRECTORY:
	return '/';

    case TW_COMMAND:
    case TW_FILE:
	return isadirectory(exp_dir, exp_name) ? '/' : ' ';

    case TW_ALIAS:
    case TW_VARLIST:
    case TW_WORDLIST:
    case TW_SHELLVAR:
    case TW_ENVVAR:
    case TW_USER:
    case TW_BINDING:
    case TW_LIMIT:
    case TW_SIGNAL:
    case TW_JOB:
    case TW_COMPLETION:
	return ' ';

    default:
	return '\0';
    }
} /* end tw_suffix */


/* tw_fixword():
 *	Repair a word after a spalling or a recognizwe
 */
static void
tw_fixword(looking, word, dir, exp_name, max_word_length)
    int looking;
    Char *word, *dir, *exp_name;
    int max_word_length;
{
    Char *ptr;

    switch (looking) {
    case TW_LOGNAME:
	copyn(word, STRtilde, 1);
	break;
    
    case TW_VARIABLE:
	if ((ptr = Strrchr(word, '$')) != NULL)
	    *++ptr = '\0';	/* Delete after the dollar */
	else
	    word[0] = '\0';
	break;

    case TW_DIRECTORY:
    case TW_FILE:
	copyn(word, dir, max_word_length);	/* put back dir part */
	break;

    default:
	word[0] = '\0';
	break;
    }

    catn(word, exp_name, max_word_length);	/* add extended name */
} /* end tw_fixword */


/* tw_collect():
 *	Collect items. Return -1 in case we were interrupted or
 *	the return value of tw_collect
 *	This is really a wrapper for tw_collect_items, serving two
 *	purposes:
 *		1. Handles interrupt cleanups.
 *		2. Retries if we had no matches, but there were ignored matches
 */
static int
tw_collect(command, looking, exp_dir, exp_name, target, pat, flags, dir_fd)
    COMMAND command;
    int looking;
    Char *exp_dir, *exp_name, *target, *pat;
    int flags;
    DIR *dir_fd;
{
    static int ni;	/* static so we don't get clobbered */
    jmp_buf_t osetexit;

#ifdef TDEBUG
    xprintf("target = %S\n", target);
#endif
    ni = 0;
    getexit(osetexit);
    for (;;) {
	(*tw_start_entry[looking])(dir_fd, pat);
	InsideCompletion = 1;
	if (setexit()) {
	    /* interrupted, clean up */
	    resexit(osetexit);
	    InsideCompletion = 0;
	    haderr = 0;
	    (*tw_end_entry[looking])();
	    /* flag error */
	    return(-1);
	}
        if ((ni = tw_collect_items(command, looking, exp_dir, exp_name,
			           target, pat, 
				   ni >= 0 ? flags : 
					flags & ~TW_IGN_OK)) >= 0) {
	    resexit(osetexit);
	    InsideCompletion = 0;
	    (*tw_end_entry[looking])();
	    return(ni);
	}
    }
} /* end tw_collect */


/* tw_list_items():
 *	List the items that were found
 */
static void
tw_list_items(looking, numitems, list_max)
    int looking, numitems, list_max;
{
    Char *ptr;
    int max_items = 0;

    if ((ptr = value(STRlistmax)) != STRNULL) {
	while (*ptr) {
	    if (!Isdigit(*ptr)) {
		max_items = 0;
		break;
	    }
	    max_items = max_items * 10 + *ptr++ - '0';
	}
    }

    if ((max_items > 0) && (numitems > max_items) && list_max) {
	char    tc;

	xprintf("There are %d items, list them anyway? [n/y] ", numitems);
	flush();
	/* We should be in Rawmode here, so no \n to catch */
	(void) read(SHIN, &tc, 1);
	xprintf("%c\r\n", tc);	/* echo the char, do a newline */
	if ((tc != 'y') && (tc != 'Y'))
	    return;
    }

    if (looking != TW_SIGNAL)
	qsort((ptr_t) tw_item_get(), (size_t) numitems, sizeof(Char *), 
	      (int (*) __P((const void *, const void *))) fcompare);
    if (looking != TW_JOB)
	print_by_column(STRNULL, tw_item_get(), numitems, TRUE);
    else {
	/*
	 * print one item on every line because jobs can have spaces
	 * and it is confusing.
	 */
	int i;
	Char **w = tw_item_get();

	for (i = 0; i < numitems; i++) {
	    xprintf("%S", w[i]);
	    if (Tty_raw_mode)
		xputchar('\r');
	    xputchar('\n');
	}
    }
} /* end tw_list_items */


/* t_search():
 *	Perform a RECOGNIZE, LIST or SPELL command on string "word".
 *
 *	Return value:
 *		>= 0:   SPELL command: "distance" (see spdist())
 *		                other: No. of items found
 *  		 < 0:   Error (message or beep is output)
 */
/*ARGSUSED*/
int
t_search(word, wp, command, max_word_length, looking, list_max, pat, suf)
    Char   *word, *wp;		/* original end-of-word */
    COMMAND command;
    int     max_word_length, looking, list_max;
    Char   *pat;
    int     suf;
{
    int     numitems = 0,		/* Number of items matched */
	    flags = 0,			/* search flags */
	    gpat = pat[0] != '\0',	/* Glob pattern search */
	    nd;				/* Normalized directory return */
    Char    exp_dir[FILSIZ + 1],	/* dir after ~ expansion */
            dir[FILSIZ + 1],		/* /x/y/z/ part in /x/y/z/f */
            exp_name[MAXNAMLEN + 1],	/* the recognized (extended) */
            name[MAXNAMLEN + 1],	/* f part in /d/d/d/f name */
           *target = NULL;		/* Target to expand/correct/list */
    DIR    *dir_fd = NULL;	

    /*
     * bugfix by Marty Grossman (grossman@CC5.BBN.COM): directory listing can
     * dump core when interrupted
     */
    tw_item_free();

    non_unique_match = FALSE;	/* See the recexact code below */

    extract_dir_and_name(word, dir, name);

    /*
     * Try to figure out what we should be looking for
     */

    switch (looking) {
    case TW_NONE:
	return -1;

    case TW_ZERO:
	looking = TW_FILE;
	break;

    case TW_COMMAND:
	if (Strchr(word, '/')) {
	    looking = TW_FILE;
	    flags |= TW_EXEC_CHK;
	    flags |= TW_DIR_OK;
	}
#ifdef notdef
	/* PWP: don't even bother when doing ALL of the commands */
	if (looking == TW_COMMAND && (*word == '\0')) 
	    return (-1);
#endif
	break;

    case TW_VARLIST:
    case TW_WORDLIST:
	gpat = 0;	/* pattern holds the name of the variable */
	break;

    case TW_EXPLAIN:
	if (command == LIST && pat != NULL) {
	    xprintf("%S", pat);
	    if (Tty_raw_mode)
		xputchar('\r');
	    xputchar('\n');
	}
	return 2;

    default:
	break;
    }

    /*
     * let fignore work only when we are not using a pattern
     */
    flags |= (gpat == 0) ? TW_IGN_OK : TW_PAT_OK;

    if ((*word == '~') && (Strchr(word, '/') == NULL)) {
	looking = TW_LOGNAME;
	target = name;
    }
    else if ((target = Strrchr(name, '$')) != 0 && 
	     (Strchr(name, '/') == NULL)) {
	target++;
	looking = TW_VARIABLE;
    }
    else
	target = name;

#ifdef TDEBUG
    xprintf("looking = %d\n", looking);
#endif
    exp_dir[0] = '\0';

    switch (looking) {
    case TW_ALIAS:
    case TW_SHELLVAR:
    case TW_ENVVAR:
    case TW_BINDING:
    case TW_LIMIT:
    case TW_SIGNAL:
    case TW_JOB:
    case TW_COMPLETION:
	break;


    case TW_VARIABLE:
	if ((nd = expand_dir(dir, exp_dir, &dir_fd, command)) != 0)
	    return nd;
	break;

    case TW_DIRECTORY:
	flags |= TW_DIR_CHK;

#ifdef notyet
	/*
	 * This is supposed to expand the directory stack.
	 * Problems:
	 * 1. Slow
	 * 2. directories with the same name
	 */
	flags |= TW_DIR_OK;
#endif
#ifdef notyet
	/*
	 * Supposed to do delayed expansion, but it is inconsistent
	 * from a user-interface point of view, since it does not
	 * immediately obey addsuffix
	 */
	if ((nd = expand_dir(dir, exp_dir, &dir_fd, command)) != 0)
	    return nd;
	if (isadirectory(exp_dir, name)) {
	    if (exp_dir[0] != '\0' || name[0] != '\0') {
		catn(dir, name, MAXNAMLEN);
		if (dir[Strlen(dir) - 1] != '/')
		    catn(dir, STRslash, MAXNAMLEN);
		if ((nd = expand_dir(dir, exp_dir, &dir_fd, command)) != 0)
		    return nd;
		if (word[Strlen(word) - 1] != '/')
		    catn(word, STRslash, MAXNAMLEN);
		name[0] = '\0';
	    }
	}
#endif
	if ((nd = expand_dir(dir, exp_dir, &dir_fd, command)) != 0)
	    return nd;
	break;

    case TW_FILE:
	if ((nd = expand_dir(dir, exp_dir, &dir_fd, command)) != 0)
	    return nd;
	break;

    case TW_LOGNAME:
	word++;
	/*FALLTHROUGH*/
    case TW_USER:
	/*
	 * Check if the spelling was already correct
	 * From: Rob McMahon <cudcv@cu.warwick.ac.uk>
	 */
	if (command == SPELL && getpwnam(short2str(word)) != NULL) {
#ifdef YPBUGS
	    fix_yp_bugs();
#endif /* YPBUGS */
	    if (looking == TW_LOGNAME)
		word--;
	    return (0);
	}
	copyn(name, word, MAXNAMLEN);	/* name sans ~ */
	if (looking == TW_LOGNAME)
	    word--;
	break;

    case TW_COMMAND:
    case TW_VARLIST:
    case TW_WORDLIST:
	copyn(target, word, MAXNAMLEN);	/* so it can match things */
	break;

    default:
	xprintf("\ntcsh internal error: I don't know what I'm looking for!\n");
	NeedsRedraw = 1;
	return (-1);
    }

    numitems = tw_collect(command, looking, exp_dir, exp_name, 
			  target, pat, flags, dir_fd);
    if (numitems == -1)
	return -1;

    switch (command) {
    case RECOGNIZE:
	if (numitems <= 0) 
	    return (numitems);

	tw_fixword(looking, word, dir, exp_name, max_word_length);

	if (is_set(STRaddsuffix) && numitems == 1) {
	    Char suffix[2];

	    suffix[1] = '\0';
	    switch (suf) {
	    case 0: 	/* Automatic suffix */
		suffix[0] = tw_suffix(looking, exp_dir, exp_name, target, name);
		break;

	    case -1:	/* No suffix */
		return numitems;

	    default:	/* completion specified suffix */
		suffix[0] = suf;
		break;
	    }
	    catn(word, suffix, max_word_length);
	}
	return numitems;

    case LIST:
	tw_list_items(looking, numitems, list_max);
	tw_item_free();
	return (numitems);

    case SPELL:
	tw_fixword(looking, word, dir, exp_name, max_word_length);
	return (numitems);

    default:
	xprintf("Bad tw_command\n");
	return (0);
    }
} /* end t_search */


/* extract_dir_and_name():
 * 	parse full path in file into 2 parts: directory and file names
 * 	Should leave final slash (/) at end of dir.
 */
static void
extract_dir_and_name(path, dir, name)
    Char   *path, *dir, *name;
{
    register Char *p;

    p = Strrchr(path, '/');
    if (p == NULL) {
	copyn(name, path, MAXNAMLEN);
	dir[0] = '\0';
    }
    else {
	p++;
	copyn(name, p, MAXNAMLEN);
	copyn(dir, path, p - path);
    }
} /* end extract_dir_and_name */


/* dollar():
 * 	expand "/$old1/$old2/old3/"
 * 	to "/value_of_old1/value_of_old2/old3/"
 */
static Char *
dollar(new, old)
    Char   *new, *old;
{
    Char   *var, *val, *p, save;
    int     space;

    for (space = FILSIZ, p = new; *old && space > 0;)
	if (*old != '$') {
	    *p++ = *old++;
	    space--;
	}
	else {
	    struct varent *vp;

	    /* found a variable, expand it */
	    for (var = ++old; alnum(*old); old++)
		continue;
	    save = *old;
	    *old = '\0';
	    vp = adrof(var);
	    val = (!vp) ? Getenv(var) : NULL;
	    *old = save;
	    /*
	     * Don't expand array variables
	     */
	    if (vp) {
		if (!vp->vec[0] || vp->vec[1]) {
		    *new = '\0';
		    return (NULL);
		}
		else
		    val = vp->vec[0];
	    }
	    else if (!val) {
		*new = '\0';
		return (NULL);
	    }
	    for (; space > 0 && *val; space--)
		*p++ = *val++;
	}
    *p = '\0';
    return (new);
} /* end dollar */


/* tilde():
 * 	expand ~person/foo to home_directory_of_person/foo
 *	or =<stack-entry> to <dir in stack entry>
 */
static Char *
tilde(new, old)
    Char   *new, *old;
{
    register Char *o, *p;

    switch (old[0]) {
    case '~':
	for (p = new, o = &old[1]; *o && *o != '/'; *p++ = *o++) 
	    continue;
	*p = '\0';
	if (gethdir(new)) {
	    new[0] = '\0';
	    return NULL;
	}
	(void) Strcat(new, o);
	return new;

    case '=':
	if ((p = globequal(new, old)) == NULL) {
	    *new = '\0';
	    return NULL;
	}
	if (p == new)
	    return new;
	/*FALLTHROUGH*/

    default:
	(void) Strcpy(new, old);
	return new;
    }
} /* end tilde */


/* expand_dir():
 *	Open the directory given, expanding ~user and $var
 *	Optionally normalize the path given
 */
static int
expand_dir(dir, edir, dfd, cmd)
    Char   *dir, *edir;
    DIR   **dfd;
    COMMAND cmd;
{
    Char   *nd = NULL;
    Char    tdir[MAXPATHLEN + 1];

    if ((dollar(tdir, dir) == 0) ||
	(tilde(edir, tdir) == 0) ||
	!(nd = dnormalize(*edir ? edir : STRdot, symlinks == SYM_IGNORE ||
						 symlinks == SYM_EXPAND)) ||
	((*dfd = opendir(short2str(nd))) == NULL)) {
	xfree((ptr_t) nd);
	if (cmd == SPELL || SearchNoDirErr)
	    return (-2);
	/*
	 * From: Amos Shapira <amoss@cs.huji.ac.il>
	 * Print a better message when completion fails
	 */
	xprintf("\n%S %s\n",
		*edir ? edir :
		(*tdir ? tdir : dir),
		(errno == ENOTDIR ? "not a directory" :
		(errno == ENOENT ? "not found" : "unreadable")));
	NeedsRedraw = 1;
	return (-1);
    }
    if (nd) {
	if (*dir != '\0') {
	    Char   *s, *d, *p;

	    /*
	     * Copy and append a / if there was one
	     */
	    for (p = edir; *p; p++)
		continue;
	    if (*--p == '/') {
		for (p = nd; *p; p++)
		    continue;
		if (*--p != '/')
		    p = NULL;
	    }
	    for (d = edir, s = nd; (*d++ = *s++) != '\0';)
		continue;
	    if (!p) {
		*d-- = '\0';
		*d = '/';
	    }
	}
	xfree((ptr_t) nd);
    }
    return 0;
} /* end expand_dir */


/* nostat():
 *	Returns true if the directory should not be stat'd,
 *	false otherwise.
 *	This way, things won't grind to a halt when you complete in /afs
 *	or very large directories.
 */
static bool
nostat(dir)
     Char *dir;
{
    struct varent *vp;
    register Char **cp;

    if ((vp = adrof(STRnostat)) == NULL || (cp = vp->vec) == NULL)
	return (FALSE);
    for (; *cp != NULL; cp++)
	if (eq(*cp, dir))
	    return (TRUE);
    return (FALSE);
} /* end nostat */


/* filetype():
 *	Return a character that signifies a filetype
 *	symbology from 4.3 ls command.
 */
static  Char
filetype(dir, file)
    Char   *dir, *file;
{
    if (dir) {
	Char    path[512];
	char   *ptr;
	struct stat statb;

	if (nostat(dir)) return('/');

	(void) Strcpy(path, dir);
	catn(path, file, sizeof(path) / sizeof(Char));

	if (lstat(ptr = short2str(path), &statb) != -1)
	    /* see above #define of lstat */
	{
#ifdef S_ISLNK
	    if (S_ISLNK(statb.st_mode)) {	/* Symbolic link */
		if (adrof(STRlistlinks)) {
		    if (stat(ptr, &statb) == -1)
			return ('&');
		    else if (S_ISDIR(statb.st_mode))
			return ('>');
		    else
			return ('@');
		}
		else
		    return ('@');
	    }
#endif
#ifdef S_ISSOCK
	    if (S_ISSOCK(statb.st_mode))	/* Socket */
		return ('=');
#endif
#ifdef S_ISFIFO
	    if (S_ISFIFO(statb.st_mode)) /* Named Pipe */
		return ('|');
#endif
#ifdef S_ISHIDDEN
	    if (S_ISHIDDEN(statb.st_mode)) /* Hidden Directory [aix] */
		return ('+');
#endif
#ifdef S_ISCDF	
	    if (S_ISCDF(statb.st_mode))	/* Context Dependent Files [hpux] */
		return ('+');
#endif 
#ifdef S_ISNWK
	    if (S_ISNWK(statb.st_mode)) /* Network Special [hpux] */
		return (':');
#endif
	    if (S_ISCHR(statb.st_mode))	/* char device */
		return ('%');
	    if (S_ISBLK(statb.st_mode))	/* block device */
		return ('#');
	    if (S_ISDIR(statb.st_mode))	/* normal Directory */
		return ('/');
	    if (statb.st_mode & 0111)
		return ('*');
	}
    }
    return (' ');
} /* end filetype */


/* isadirectory():
 *	Return trus if the file is a directory
 */
static int
isadirectory(dir, file)		/* return 1 if dir/file is a directory */
    Char   *dir, *file;		/* uses stat rather than lstat to get dest. */
{
    if (dir) {
	Char    path[MAXPATHLEN];
	struct stat statb;

	if (nostat(dir)) return(1);

	(void) Strcpy(path, dir);
	catn(path, file, sizeof(path) / sizeof(Char));
	if (stat(short2str(path), &statb) >= 0) {	/* resolve through
							 * symlink */
#ifdef S_ISSOCK
	    if (S_ISSOCK(statb.st_mode))	/* Socket */
		return 0;
#endif
#ifdef S_ISFIFO
	    if (S_ISFIFO(statb.st_mode))	/* Named Pipe */
		return 0;
#endif
	    if (S_ISDIR(statb.st_mode))	/* normal Directory */
		return 1;
	}
    }
    return 0;
} /* end isadirectory */


/* print_by_column():
 * 	Print sorted down columns
 */
void
print_by_column(dir, items, count, no_file_suffix)
    register Char *dir, *items[];
    int     count, no_file_suffix;
{
    register int i, r, c, columns, rows;
    unsigned int w, maxwidth = 0;

    lbuffed = 0;		/* turn off line buffering */

    for (i = 0; i < count; i++)	/* find widest string */
	maxwidth = max(maxwidth, Strlen(items[i]));

    maxwidth += no_file_suffix ? 1 : 2;	/* for the file tag and space */
    columns = (TermH + 1) / maxwidth;	/* PWP: terminal size change */
    if (!columns)
	columns = 1;
    rows = (count + (columns - 1)) / columns;

    for (r = 0; r < rows; r++) {
	for (c = 0; c < columns; c++) {
	    i = c * rows + r;

	    if (i < count) {
		w = Strlen(items[i]);

		if (no_file_suffix) {
		    /* Print the command name */
		    xprintf("%S", items[i]);
		}
		else {
		    /* Print filename followed by '/' or '*' or ' ' */
		    xprintf("%S%c", items[i],
			    filetype(dir, items[i]));
		    w++;
		}

		if (c < (columns - 1))	/* Not last column? */
		    for (; w < maxwidth; w++)
			xputchar(' ');
	    }
	}
	if (Tty_raw_mode)
	    xputchar('\r');
	xputchar('\n');
    }

    lbuffed = 1;		/* turn back on line buffering */
    flush();
} /* end print_by_column */


/* StrQcmp():
 *	Compare strings ignoring the quoting chars
 */
int
StrQcmp(str1, str2)
    register Char *str1, *str2;
{
    for (; *str1 && (*str1 & TRIM) == (*str2 & TRIM); str1++, str2++)
	continue;
    /*
     * The following case analysis is necessary so that characters which look
     * negative collate low against normal characters but high against the
     * end-of-string NUL.
     */
    if (*str1 == '\0' && *str2 == '\0')
	return (0);
    else if (*str1 == '\0')
	return (-1);
    else if (*str2 == '\0')
	return (1);
    else
	return ((*str1 & TRIM) - (*str2 & TRIM));
} /* end StrQcmp */


/* fcompare():
 * 	Comparison routine for qsort
 */
int
fcompare(file1, file2)
    Char  **file1, **file2;
{
#if defined(NLS) && !defined(NOSTRCOLL)
    char    buf[2048];

    (void) strcpy(buf, short2str(*file1));
    return ((int) strcoll(buf, short2str(*file2)));
#else
    return (StrQcmp(*file1, *file2));
#endif
} /* end fcompare */


/* catn():
 *	Concatenate src onto tail of des.
 *	Des is a string whose maximum length is count.
 *	Always null terminate.
 */
void
catn(des, src, count)
    register Char *des, *src;
    register count;
{
    while (--count >= 0 && *des)
	des++;
    while (--count >= 0)
	if ((*des++ = *src++) == 0)
	    return;
    *des = '\0';
} /* end catn */


/* copyn():
 *	 like strncpy but always leave room for trailing \0
 *	 and always null terminate.
 */
void
copyn(des, src, count)
    register Char *des, *src;
    register count;
{
    while (--count >= 0)
	if ((*des++ = *src++) == 0)
	    return;
    *des = '\0';
} /* end copyn */


/* Getenv():
 *	like it's normal string counter-part
 *	[apollo uses that in tc.os.c, so it cannot be static]
 */
Char *
Getenv(str)
    Char   *str;
{
    Char  **var;
    int     len, res;

    len = Strlen(str);
    for (var = STR_environ; var != NULL && *var != NULL; var++)
	if ((*var)[len] == '=') {
	    (*var)[len] = '\0';
	    res = StrQcmp(*var, str);
	    (*var)[len] = '=';
	    if (res == 0)
		return (&((*var)[len + 1]));
	}
    return (NULL);
} /* end Getenv */

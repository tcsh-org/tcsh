/*
 * tw.color.c: builtin color ls-F
 */
/*-
 * Copyright (c) 1998 The Regents of the University of California.
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
#include "tw.h"
#include "ed.h"
#include "tc.h"

#include <assert.h>

#ifdef COLOR_LS_F

typedef struct {
    const char	 *s;
    size_t len;
} Str;


#define VAR(varindex,colorvar,variable,defaultcolor) \
{ \
    varindex, colorvar, variable, { defaultcolor, sizeof(defaultcolor) - 1 }, \
      { defaultcolor, sizeof(defaultcolor) - 1 } \
}

/* VARINDEX. LS_COLORS color variable.
 * Used as index to variables[].
 */
typedef enum {
    Vdi, Vln, Vor, Vpi, Vso, Vdo, Vbd, Vcd,
    Vex, Vfi, Vno, Vmi, Vlc, Vrc, Vec, Vsu,
    Vsg, Vtw, Vow, Vst, Vrs, Vhl, Vmh, Vca,
} VARINDEX;

typedef struct {
    const VARINDEX	 varindex;	/* Must be same as index in variables[] */
    const COLORVAR	 colorvar;	/* COLORVAR. Use CV_NONE if no mapping */
    const char		*variable;	/* LS_COLORS variable */
    Str			 color;		/* Color: default or user-provided color */
    const Str		 defaultcolor;	/* Default color */
} Variable;

/*
 * variables[]: array of Variable.
 * Must be indexed on enum VARINDEX, and .varindex must be the index.
 */
static Variable variables[] = {
    VAR(Vdi, CV_DIR,	"di", "01;34"),	/* Directory */
    VAR(Vln, CV_LNK,	"ln", "01;36"),	/* Symbolic link */
    VAR(Vor, CV_ORPHAN,	"or", ""),	/* Orphaned (broken) symbolic link. Defaults to ln */
    VAR(Vpi, CV_FIFO,	"pi", "33"),	/* Named pipe (FIFO) */
    VAR(Vso, CV_SOCK,	"so", "01;35"),	/* Socket */
    VAR(Vdo, CV_DOOR,	"do", "01;35"),	/* Door (solaris fast ipc mechanism)  */
    VAR(Vbd, CV_BLK,	"bd", "01;33"), /* Block device */
    VAR(Vcd, CV_CHR,	"cd", "01;33"),	/* Character device */
    VAR(Vex, CV_EXE,	"ex", "01;32"),	/* Executable file */
    VAR(Vfi, CV_NONE,	"fi", "0"),	/* Regular file, possibly with extension */
    VAR(Vno, CV_NONE,	"no", "0"),	/* Normal (non-filename) text */
    VAR(Vmi, CV_NONE,	"mi", ""),	/* Missing file (orphaned symbolic link target). Defaults to fi */
#ifdef IS_ASCII
    VAR(Vlc, CV_NONE,	"lc", "\033["),	/* Left code (ASCII) */
#else
    VAR(Vlc, CV_NONE,	"lc", "\x27["),	/* Left code (EBCDIC)*/
#endif
    VAR(Vrc, CV_NONE,	"rc", "m"),	/* Right code */
    VAR(Vec, CV_NONE,	"ec", ""),	/* End code (replaces lc+no+rc) */
    VAR(Vsu, CV_SUID,	"su", "37;41"),	/* Setuid file (u+s) */
    VAR(Vsg, CV_SGID,	"sg", "30;43"),	/* Setgid file (g+s) */
    VAR(Vtw, CV_DIR_TW,	"tw", "30;42"),	/* Sticky and other writable dir (+t,o+w) */
    VAR(Vow, CV_DIR_OW,	"ow", "34;42"),	/* Other writable dir (o+w) but not sticky */
    VAR(Vst, CV_DIR_ST,	"st", "37;44"),	/* Sticky dir (+t) but not other writable */
    VAR(Vrs, CV_NONE,	"rs", "0"),	/* Reset to normal color */
    VAR(Vhl, CV_NONE,	"hl", ""),	/* Obsolete, use mh */
    VAR(Vmh, CV_HARD,	"mh", ""),	/* Regular file with multiple hard links */
    VAR(Vca, CV_NONE,	"ca", ""),	/* File with capability. Not implemented. */
};

#define nvariables (sizeof(variables)/sizeof(variables[0]))

/*
 * Map from LSCOLORS entry index to VARINDEX in variables[].
 */
static const uint8_t lscolors_to_varindex[] = {
    Vdi,	/* Directory */
    Vln,	/* Symbolic Link */
    Vso,	/* Socket */
    Vpi,	/* Named Pipe */
    Vex,	/* Executable */
    Vbd,	/* Block Special */
    Vcd,	/* Character Special */
    Vsu,	/* Setuid Executable */
    Vsg,	/* Setgid Executable */
    Vtw,	/* Directory writable to others and sticky */
    Vow,	/* Directory writable to others but not sticky */
};


enum ansi {
    ANSI_RESET_ON = 0,		/* reset colors/styles (white on black) */
    ANSI_BOLD_ON = 1,		/* bold on */
    ANSI_ITALICS_ON = 3,	/* italics on */
    ANSI_UNDERLINE_ON = 4,	/* underline on */
    ANSI_INVERSE_ON = 7,	/* inverse on */
    ANSI_STRIKETHROUGH_ON = 9,	/* strikethrough on */
    ANSI_BOLD_OFF = 21,		/* bold off */
    ANSI_ITALICS_OFF = 23,	/* italics off */
    ANSI_UNDERLINE_OFF = 24,	/* underline off */
    ANSI_INVERSE_OFF = 27,	/* inverse off */
    ANSI_STRIKETHROUGH_OFF = 29,/* strikethrough off */
    ANSI_FG_BLACK = 30,		/* fg black */
    ANSI_FG_RED = 31,		/* fg red */
    ANSI_FG_GREEN = 32,		/* fg green */
    ANSI_FG_YELLOW = 33,	/* fg yellow */
    ANSI_FG_BLUE = 34,		/* fg blue */
    ANSI_FG_MAGENTA = 35,	/* fg magenta */
    ANSI_FG_CYAN = 36,		/* fg cyan */
    ANSI_FG_WHITE = 37,		/* fg white */
    ANSI_FG_DEFAULT = 39,	/* fg default (white) */
    ANSI_BG_BLACK = 40,		/* bg black */
    ANSI_BG_RED = 41,		/* bg red */
    ANSI_BG_GREEN = 42,		/* bg green */
    ANSI_BG_YELLOW = 43,	/* bg yellow */
    ANSI_BG_BLUE = 44,		/* bg blue */
    ANSI_BG_MAGENTA = 45,	/* bg magenta */
    ANSI_BG_CYAN = 46,		/* bg cyan */
    ANSI_BG_WHITE = 47,		/* bg white */
    ANSI_BG_DEFAULT = 49,	/* bg default (black) */
};
#define TCSH_BOLD	0x80

typedef struct {
    Str	    extension;	/* file extension */
    Str	    color;	/* color string */
} Extension;

static Extension *extensions = NULL;
static size_t nextensions = 0;

static char *colors = NULL;
int	     color_context_ls = FALSE;	/* do colored ls */
static int   color_context_lsmF = FALSE; /* do colored ls-F */
static int   color_as_referent = FALSE;	/* ln=target in LS_COLORS */
int	     color_force = FALSE;	/* allow colored ls without a tty */

static int getstring (char **, const Char **, Str *, int);
static void put_color (const Str *);
static void print_color (const Char *, size_t, COLORVAR);

/* Str_equal_literal():
 *	Does a Str equal a literal string?
 */
static int
Str_equal_literal(const Str * left, const char * right)
{
    const size_t rlen = strlen(right);
    if (left->len != rlen)
	return FALSE;
    return strncmp(left->s, right, rlen) == 0;
}

/* set_color_context():
 */
void
set_color_context(void)
{
    struct varent *vp = adrof(STRcolor);

    if (vp == NULL || vp->vec == NULL) {
	color_context_ls = FALSE;
	color_context_lsmF = FALSE;
    } else if (!vp->vec[0] || vp->vec[0][0] == '\0') {
	color_context_ls = TRUE;
	color_context_lsmF = TRUE;
    } else {
	size_t i;

	color_context_ls = FALSE;
	color_context_lsmF = FALSE;
	for (i = 0; vp->vec[i]; i++)
	    if (Strcmp(vp->vec[i], STRls) == 0)
		color_context_ls = TRUE;
	    else if (Strcmp(vp->vec[i], STRlsmF) == 0)
		color_context_lsmF = TRUE;
    }
}


/* getstring():
 */
static	int
getstring(char **dp, const Char **sp, Str *pd, int f)
{
    const Char *s = *sp;
    char *d = *dp;
    eChar sc;

    while (*s && (*s & CHAR) != (Char)f && (*s & CHAR) != ':') {
	if ((*s & CHAR) == '\\' || (*s & CHAR) == '^') {
	    if ((sc = parseescape(&s, TRUE)) == CHAR_ERR)
		return 0;
	}
	else
	    sc = *s++ & CHAR;
	d += one_wctomb(d, sc);
    }

    pd->s = *dp;
    pd->len = d - *dp;
    *sp = s;
    *dp = d;
    return *s == (Char)f;
}

static void
init(size_t colorlen, size_t extnum)
{
    size_t i;

    color_as_referent = FALSE;
    xfree(extensions);
    for (i = 0; i < nvariables; i++) {
	assert(i == variables[i].varindex);
	variables[i].color = variables[i].defaultcolor;
    }
    if (colorlen == 0 && extnum == 0) {
	extensions = NULL;
	colors = NULL;
    } else {
	extensions = xmalloc(colorlen + extnum * sizeof(*extensions));
	colors = extnum * sizeof(*extensions) + (char *)extensions;
    }
    nextensions = 0;
}

static int
color(Char x)
{
    int c = 0;
    static const char ccolors[] = "abcdefghx";
    char *p;
    if (Isupper(x)) {
	x = Tolower(x);
	c |= TCSH_BOLD;
    }

    if (x == '\0' || (p = strchr(ccolors, x)) == NULL)
	return -1;
    return (30 + (p - ccolors)) | c;
}

static void
makecolor(char **c, int fg, int bg, Str *v)
{
    int l;
    if (fg & 0x80) {
	l = xsnprintf(*c, 12, "%.2d;%.2d;%.2d;%.2d", ANSI_BOLD_ON,
	    fg & ~TCSH_BOLD, (10 + bg) & ~TCSH_BOLD, ANSI_BOLD_OFF);
    } else {
	l = xsnprintf(*c, 6, "%.2d;%.2d",
	    fg & ~TCSH_BOLD, (10 + bg) & ~TCSH_BOLD);
    }

    v->s = *c;
    v->len = l;
    *c += l + 1;
}

/* parseCLICOLOR_FORCE():
 *	Parse the CLICOLOR_FORCE environment variable.
 *	For compatibility with BSD ls(1), use the presence.
 */
void
parseCLICOLOR_FORCE(int is_setenv, const Char *value)
{
    USE(value);
    color_force = is_setenv;
}

/* parseLSCOLORS():
 * 	Parse the LSCOLORS environment variable.
 *	Suppress errors if silent is TRUE.
 */
static const Char *xv;	/* setjmp clobbering */
void
parseLSCOLORS(const Char *value, int silent)
{
    size_t i, len, clen;
    jmp_buf_t osetexit;
    size_t omark;
    xv = value;

    if (xv == NULL) {
	init(0, 0);
	return;
    }

    len = Strlen(xv);
    len >>= 1;
    clen = len * 12;	/* "??;??;??;??\0" */
    init(clen, 0);

    /* Prevent from crashing if unknown parameters are given. */
    omark = cleanup_push_mark();
    getexit(osetexit);

    /* init pointers */

    if (setexit() == 0) {
	const Char *v = xv;
	char *c = colors;

	int fg, bg;
	for (i = 0; i < len; i++) {
	    if (i >= (sizeof(lscolors_to_varindex)/sizeof(lscolors_to_varindex[0])))
		break;
	    fg = color(*v++);
	    if (fg == -1)
		stderror(ERR_BADCOLORVAR | (silent ? ERR_SILENT : 0),
		    "LSCOLORS", v[-1], '?');

	    bg = color(*v++);
	    if (bg == -1)
		stderror(ERR_BADCOLORVAR | (silent ? ERR_SILENT : 0),
		    "LSCOLORS", '?', v[-1]);
	    assert(lscolors_to_varindex[i] < nvariables);
	    makecolor(&c, fg, bg, &variables[lscolors_to_varindex[i]].color);
	}

    }
    cleanup_pop_mark(omark);
    resexit(osetexit);
}

/* parseLS_COLORS():
 *	Parse the LS_COLORS environment variable
 *	Suppress printing errors if silent is TRUE
 *	(although a non-zero exit status still occurs).
 */
void
parseLS_COLORS(const Char *value, int silent)
{
    size_t  i, len;
    const Char	 *v;		/* pointer in value */
    char   *c;			/* pointer in colors */
    Extension *volatile e;	/* pointer in extensions */
    jmp_buf_t osetexit;
    size_t omark;

    (void) &e;


    if (value == NULL) {
	init(0, 0);
	return;
    }

    len = Strlen(value);
    /* allocate memory */
    i = 1;
    for (v = value; *v; v++)
	if ((*v & CHAR) == ':')
	    i++;

    init(len, i);

    /* init pointers */
    v = value;
    c = colors;
    e = extensions;

    /* Prevent from crashing if unknown parameters are given. */

    omark = cleanup_push_mark();
    getexit(osetexit);

    if (setexit() == 0) {

	/* parse */
	while (*v) {
	    switch (*v & CHAR) {
	    case ':':
		v++;
		continue;

	    case '*':		/* :*ext=color: */
		v++;
		if (getstring(&c, &v, &e->extension, '=') &&
		    0 < e->extension.len) {
		    v++;
		    getstring(&c, &v, &e->color, ':');
		    e++;
		    continue;
		}
		break;

	    default:		/* :vl=color: */
		if (v[0] && v[1] && (v[2] & CHAR) == '=') {
		    for (i = 0; i < nvariables; i++)
			if ((Char)variables[i].variable[0] == (v[0] & CHAR) &&
			    (Char)variables[i].variable[1] == (v[1] & CHAR))
			    break;
		    if (i < nvariables) {
			v += 3;
			getstring(&c, &v, &variables[i].color, ':');
			if (i == Vln)
			    color_as_referent = Str_equal_literal(
				&variables[Vln].color, "target");
			continue;
		    }
		    else
			stderror(ERR_BADCOLORVAR | (silent ? ERR_SILENT : 0),
			    "LS_COLORS", v[0], v[1]);
		}
		break;
	    }
	    while (*v && (*v & CHAR) != ':')
		v++;
	}
    }

    cleanup_pop_mark(omark);
    resexit(osetexit);

    nextensions = e - extensions;
}

/* put_color():
 */
static void
put_color(const Str *colorp)
{
    size_t  i;
    const char	 *c = colorp->s;
    int	   original_output_raw = output_raw;

    output_raw = TRUE;
    cleanup_push(&original_output_raw, output_raw_restore);
    for (i = colorp->len; 0 < i; i--)
	xputchar(*c++);
    cleanup_until(&original_output_raw);
}


/* print_color():
 */
static void
print_color(const Char *fname, size_t len, COLORVAR colorvar)
{
    size_t  i;
    char   *filename = short2str(fname);
    char   *last = filename + len;
    Str    *colorp = &variables[Vfi].color;	/* default to fi color */

    if (colorvar == CV_LNK_DIR) {	/* symlink to dir? use di color */
	colorp = &variables[Vdi].color;
    } else if (colorvar == CV_FILE) {	/* file? lookup in extension */
	for (i = 0; i < nextensions; i++) {
	    if (len >= extensions[i].extension.len
		&& strncmp(last - extensions[i].extension.len,
			   extensions[i].extension.s,
			   extensions[i].extension.len) == 0) {
		colorp = &extensions[i].color;
	    }
	}
    } else {				/* all others? lookup variables */
	for (i = 0; i < nvariables; i++) {
	    if (variables[i].colorvar == colorvar) {
		colorp = &variables[i].color;
		break;
	    }
	}
    }

    put_color(&variables[Vlc].color);
    put_color(colorp);
    put_color(&variables[Vrc].color);
}


/* print_with_color():
 */
void
print_with_color(const Char *dir, const Char *filename, size_t len,
	struct filetype filetype)
{
    if (color_context_lsmF && (color_force ||
	(haderr ? (didfds ? is2atty : isdiagatty) :
	 (didfds ? is1atty : isoutatty)))) {

	if (color_as_referent &&
	    (filetype.colorvar == CV_LNK || filetype.colorvar == CV_LNK_DIR)) {
	    struct filetype ft = get_filetype(dir, filename, FALSE);
	    print_color(filename, len, ft.colorvar);
	} else
	    print_color(filename, len, filetype.colorvar);
	xprintf("%" TCSH_S, filename);
	if (0 < variables[Vec].color.len)
	    put_color(&variables[Vec].color);
	else {
	    put_color(&variables[Vlc].color);
	    put_color(&variables[Vno].color);
	    put_color(&variables[Vrc].color);
	}
    } else
	xprintf("%" TCSH_S, filename);
    xputwchar(filetype.suffix);
}


#endif /* COLOR_LS_F */

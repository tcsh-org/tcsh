/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/sh.char.h,v 1.2 1990/11/25 10:12:05 christos Exp $ */
/*
 * sh.char.h: Table for spotting special characters quickly
 * 	      Makes for very obscure but efficient coding.
 */
#ifndef _h_sh_char
#define _h_sh_char
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley Software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)sh.char.h	5.3 (Berkeley) 3/29/86
 */


extern unsigned short _cmap[];
#ifndef NLS
extern unsigned char _cmap_lower[], _cmap_upper[];
#endif

#define	_Q	0x0001		/* '" */
#define	_Q1	0x0002		/* ` */
#define	_SP	0x0004		/* space and tab */
#define	_NL	0x0008		/* \n */
#define	_META	0x0010		/* lex meta characters, sp #'`";&<>()|\t\n */
#define	_GLOB	0x0020		/* glob characters, *?{[` */
#define	_ESC	0x0040		/* \ */
#define	_DOL	0x0080		/* $ */
#define	_DIG  	0x0100		/* 0-9 */
#define	_LET  	0x0200		/* a-z, A-Z, _ */
#define	_UP   	0x0400		/* A-Z */
#define	_LOW  	0x0800		/* a-z */
#define	_XD 	0x1000		/* 0-9, a-f, A-F */
#define	_CMD	0x2000		/* lex end of command chars, ;&(|` */
#define _CTR	0x4000		/* control */

#define cmap(c, bits)	\
	(((c) & QUOTE) ? 0 : (_cmap[(unsigned char)(c)] & (bits)))

#define isglob(c)	cmap(c, _GLOB)
#define isspc(c)	cmap(c, _SP)
#define ismeta(c)	cmap(c, _META)
#define iscmdmeta(c)	cmap(c, _CMD)
#define letter(c)	(isalpha(c) || c == '_')
#define alnum(c)	(isalnum(c) || c == '_')
#ifdef NLS
#    include <ctype.h>
#else
#    define isspace(c)	cmap(c, _SP|_NL)
#    define isdigit(c)	cmap(c, _DIG)
#    define isalpha(c)	(cmap(c,_LET) && !(((c) & META) && AsciiOnly))
#    define islower(c)	(cmap(c,_LOW) && !(((c) & META) && AsciiOnly))
#    define isupper(c)	(cmap(c, _UP) && !(((c) & META) && AsciiOnly))
#    define tolower(c)  (_cmap_lower[(unsigned char)(c)])
#    define toupper(c)  (_cmap_upper[(unsigned char)(c)])
#    define isxdigit(c)	cmap(c, _XD)
#    define isalnum(c)	(cmap(c, _DIG|_LET) && !(((c) & META) && AsciiOnly))
#    define iscntrl(c)  (cmap(c,_CTR) && !(((c) & META) && AsciiOnly))
#    define isprint(c)  (!cmap(c,_CTR) && !(((c) & META) && AsciiOnly))
#endif

#endif /* _h_sh_char */

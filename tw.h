/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.01/RCS/tw.h,v 3.3 1992/01/16 13:04:21 christos Exp $ */
/*
 * tw.h: TwENEX functions headers
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
#ifndef _h_tw
#define _h_tw

#define TW_ZERO		-1
#define TW_COMMAND	0
#define TW_VARIABLE	1
#define TW_LOGNAME	2
#define TW_FILE		3
#define TW_DIRECTORY	4
#define TW_VARLIST	5
#define TW_USER		6
#define TW_LITERAL	7
#define TW_ALIAS	8
#define TW_SHELLVAR	9
#define TW_ENVVAR	10

#define TW_EXEC_OK	1
#define TW_DIR_OK	2

#ifndef TRUE
# define TRUE		1
#endif
#ifndef FALSE
# define FALSE		0
#endif
#define ON		1
#define OFF		0
#define FILSIZ		512	/* Max reasonable file name length */
#define ESC		'\033'
#define equal(a, b)	(strcmp(a, b) == 0)

#define is_set(var)	adrof(var)
#define ismetahash(a)	(ismeta(a) && (a) != '#')

#define SEARCHLIST "HPATH"	/* Env. param for helpfile searchlist */
#define DEFAULTLIST ":/usr/man/cat1:/usr/man/cat8:/usr/man/cat6:/usr/local/man/cat1:/usr/local/man/cat8:/usr/local/man/cat6"	/* if no HPATH */

extern Char PromptBuf[];

typedef enum {
    LIST, RECOGNIZE, PRINT_HELP, SPELL, GLOB, GLOB_EXPAND,
    VARS_EXPAND, PATH_NORMALIZE
}       COMMAND;

extern int non_unique_match;

#include "tw.decls.h"

#endif /* _h_tw */

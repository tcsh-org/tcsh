/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/ed.decls.h,v 2.0 1991/03/26 02:59:29 christos Exp $ */
/*
 * ed.decls.h: Editor external definitions
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
#ifndef _h_ed_decls
#define _h_ed_decls

/*
 * ed.chared.c
 */
extern	int	InsertStr		__P((Char *));
extern	void	DeleteBack		__P((int));

/*
 * ed.init.c
 */
extern	void	check_window_size	__P((int));
#ifdef SIG_WINDOW
extern	sigret_t window_change		__P((int));
#endif
extern	void	ed_Init			__P((void));
extern	int	Cookedmode		__P((void));
extern	int	Rawmode			__P((void));
extern	void	ed_set_tty_eight_bit	__P((void));

extern	void	QuoteModeOn		__P((void));
extern	void	QuoteModeOff		__P((void));
extern	void	ResetInLine		__P((void));
extern	int	Load_input_line		__P((void));

/*
 * ed.screen.c
 */
extern	void	SetAttributes		__P((int));
extern	void	so_write		__P((Char *, int));
extern	void	ClearScreen		__P((void));
extern	void	MoveToLine		__P((int));
extern	void	MoveToChar		__P((int));
extern	void	ClearEOL		__P((int));
extern	void	Insert_write		__P((Char *, int));
extern	void	DeleteChars		__P((int));
extern	void	TellTC			__P((char *));
extern	void	SetTC			__P((char *, char *));
extern	void	EchoTC			__P((Char **));
extern	void	BindArrowKeys		__P((void));
extern	void	Beep			__P((void));
extern	int	CanWeTab		__P((void));
extern	void	ChangeSize		__P((int, int));
extern	int	GetSize			__P((int *, int *));
extern	void	ClearToBottom		__P((void));
extern	void	GetTermCaps		__P((void));

/*
 * ed.defns.c
 */
extern	void	ed_InitNLSMaps		__P((void));
#ifdef DEBUG_EDIT
extern	void	CheckMaps		__P((void));
#endif
extern	void	ed_InitMaps		__P((void));
extern	void	ed_InitEmacsMaps	__P((void));
extern	void	ed_InitVIMaps		__P((void));

/*
 * ed.inputl.c
 */
extern	int	Inputl			__P((void));
extern	int	GetNextChar		__P((Char *));
extern	void	PushMacro		__P((Char *));

/*
 * ed.refresh.c
 */
extern	void	ClearLines		__P((void));
extern	void	ClearDisp		__P((void));
extern	void	Refresh			__P((void));
extern	void	RefCursor		__P((void));
extern	void	RefPlusOne		__P((void));
extern	void	PastBottom		__P((void));

/*
 * ed.xmap.c
 */
extern	void	AddXkeyCmd		__P((Char *, int));
extern	void	AddXkey			__P((Char *, Char *));
extern	void	ClearXkey		__P((KEYCMD *, Char *));
extern	int	GetXkey			__P((Char *, Char **));
extern	void	ResetXmap		__P((int));
extern	int	DeleteXkey		__P((Char *));
extern	void	PrintXkey		__P((Char *));

#endif				/* _h_ed_decls */

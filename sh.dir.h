/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.dir.h,v 2.0 1991/03/26 02:59:29 christos Exp $ */
/*
 * sh.dir.h: Directory data structures and globals
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef _h_sh_dir
#define _h_sh_dir
/*
 *	@(#)sh.dir.h	5.2 (Berkeley) 6/6/85
 */

/*
 * Structure for entries in directory stack.
 */
struct	directory	{
	struct	directory *di_next;	/* next in loop */
	struct	directory *di_prev;	/* prev in loop */
	unsigned short *di_count;	/* refcount of processes */
	Char	*di_name;		/* actual name */
};
struct directory *dcwd;		/* the one we are in now */

#endif /* _h_sh_dir */

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/sh.dir.h,v 1.2 1990/11/25 10:12:13 christos Exp $ */
/*
 * sh.dir.h: Directory data structures and globals
 */
#ifndef _h_sh_dir
#define _h_sh_dir

/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley Software License Agreement
 * specifies the terms and conditions for redistribution.
 *
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

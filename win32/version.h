/*$Header: /p/tcsh/cvsroot/tcsh/win32/version.h,v 1.17 2006/04/07 00:57:59 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.03-debug"
#else
#define LOCALSTR ",nt-rev-8.03" 
								//patches
#endif NTDBG

#endif VERSION_H

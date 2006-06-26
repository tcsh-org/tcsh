/*$Header: /p/tcsh/cvsroot/tcsh/win32/version.h,v 1.19 2006/04/29 07:19:09 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.05-debug"
#else
#define LOCALSTR ",nt-rev-8.05" 
								//patches
#endif NTDBG

#endif VERSION_H

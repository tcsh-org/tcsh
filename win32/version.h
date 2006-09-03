/*$Header: /p/tcsh/cvsroot/tcsh/win32/version.h,v 1.20 2006/06/26 04:41:48 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.06-debug"
#else
#define LOCALSTR ",nt-rev-8.06" 
								//patches
#endif NTDBG

#endif VERSION_H

/*$Header: /p/tcsh/cvsroot/tcsh/win32/version.h,v 1.22 2007/05/25 05:28:10 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.09-debug"
#else
#define LOCALSTR ",nt-rev-8.09" 
								//patches
#endif NTDBG

#endif VERSION_H

/*$Header: /p/tcsh/cvsroot/tcsh/win32/version.h,v 1.23 2008/08/24 23:29:32 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.10-debug"
#else
#define LOCALSTR ",nt-rev-8.10" 
								//patches
#endif NTDBG

#endif VERSION_H

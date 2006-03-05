/*$Header: /p/tcsh/cvsroot/tcsh/win32/version.h,v 1.15 2006/03/03 22:08:45 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.01-debug"
#else
#define LOCALSTR ",nt-rev-8.01" 
								//patches
#endif NTDBG

#endif VERSION_H

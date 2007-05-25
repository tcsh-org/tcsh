/*$Header: /p/tcsh/cvsroot/tcsh/win32/version.h,v 1.21 2006/09/03 16:11:46 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.07-debug"
#else
#define LOCALSTR ",nt-rev-8.07" 
								//patches
#endif NTDBG

#endif VERSION_H

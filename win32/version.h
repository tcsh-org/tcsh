/*$Header: /src/pub/tcsh/win32/version.h,v 1.11 2004/07/25 14:36:27 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-7.11-debug"
#else
#define LOCALSTR ",nt-rev-7.11" 
								//patches
#endif NTDBG

#endif VERSION_H

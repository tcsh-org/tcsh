/*$Header: /src/pub/tcsh/win32/version.h,v 1.14 2005/05/25 03:01:20 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.00-debug"
#else
#define LOCALSTR ",nt-rev-8.00" 
								//patches
#endif NTDBG

#endif VERSION_H

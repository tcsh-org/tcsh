/*$Header: /src/pub/tcsh/win32/version.h,v 1.12 2004/10/18 02:32:17 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-7.15-debug"
#else
#define LOCALSTR ",nt-rev-7.15" 
								//patches
#endif NTDBG

#endif VERSION_H

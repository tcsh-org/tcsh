/*$Header: /src/pub/tcsh/win32/version.h,v 1.10 2004/05/19 18:22:28 christos Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-7.10-debug"
#else
#define LOCALSTR ",nt-rev-7.10" 
								//patches
#endif NTDBG

#endif VERSION_H

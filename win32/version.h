/*$Header: /src/pub/tcsh/win32/version.h,v 1.13 2005/03/25 18:46:42 kim Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-7.19-debug"
#else
#define LOCALSTR ",nt-rev-7.19" 
								//patches
#endif NTDBG

#endif VERSION_H

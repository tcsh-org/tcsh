/*$Header: /src/pub/tcsh/win32/version.h,v 1.9 2004/02/15 00:43:17 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-7.09-debug"
#else
#define LOCALSTR ",nt-rev-7.09" //changed ordonly test in nt_open
								//patches
#endif NTDBG

#endif VERSION_H

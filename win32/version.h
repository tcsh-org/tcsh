/*$Header: /src/pub/tcsh/win32/version.h,v 1.7 2003/11/04 01:37:44 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-7.07-debug"
#else
#define LOCALSTR ",nt-rev-7.07" //changed ordonly test in nt_open
								//patches
#endif NTDBG

#endif VERSION_H

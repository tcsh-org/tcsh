/*$Header: /src/pub/tcsh/win32/version.h,v 1.2 2002/08/11 07:58:13 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-7.06-debug"
#else
#define LOCALSTR ",nt-rev-7.06" //changed ordonly test in nt_open
								//patches
#endif NTDBG

#endif VERSION_H

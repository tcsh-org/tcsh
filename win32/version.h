/*$Header: /p/tcsh/cvsroot/tcsh/win32/version.h,v 1.16 2006/03/05 08:59:36 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.02-debug"
#else
#define LOCALSTR ",nt-rev-8.02" 
								//patches
#endif NTDBG

#endif VERSION_H

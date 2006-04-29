/*$Header: /p/tcsh/cvsroot/tcsh/win32/version.h,v 1.18 2006/04/13 00:59:03 amold Exp $*/
#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-8.04-debug"
#else
#define LOCALSTR ",nt-rev-8.04" 
								//patches
#endif NTDBG

#endif VERSION_H

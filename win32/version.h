#ifndef VERSION_H
#define VERSION_H

/* remember to change both instance of the version -amol */

#ifdef NTDBG
#define LOCALSTR ",nt-rev-6.08-debug"
#else
#define LOCALSTR ",nt-rev-6.08" //changed ordonly test in nt_open
								//patches
#endif NTDBG

#endif VERSION_H

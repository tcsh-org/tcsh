/* $Header$ */
/*
 * pathnames.h: Location of things to find
 */
#ifndef _h_pathnames
#define _h_pathnames

#ifdef CMUCS
# define _PATH_LOCAL		"/usr/cs/bin"
#else
# define _PATH_LOCAL		"/usr/local/bin"
#endif

#define _PATH_USRBIN		"/usr/bin"
#define _PATH_USRUCB		"/usr/ucb"
#define _PATH_USRBSD		"/usr/bsd"
#define _PATH_BIN		"/bin"

#if defined(convex) || defined(__convex__) || defined(IRIS4D)
# define _PATH_DOTLOGIN		"/etc/login"
# define _PATH_DOTLOGOUT	"/etc/logout"
# define _PATH_DOTCSHRC		"/etc/cshrc"
#else
# define _PATH_DOTCSHRC		"/etc/csh.cshrc"
# define _PATH_DOTLOGIN		"/etc/csh.login"
# define _PATH_DOTLOGOUT	"/etc/csh.logout"
#endif

#define _PATH_DEVNULL		"/dev/null"

#define _PATH_BSHELL		"/bin/sh"
#ifdef notdef
# define _PATH_CSHELL 		"/bin/csh"
#endif
#define _PATH_TCSHELL		"/usr/local/bin/tcsh"

#define _PATH_LOGIN		"/bin/login"
#ifdef NEWGRP
# define _PATH_BIN_NEWGRP	"/bin/newgrp"
# define _PATH_USRBIN_NEWGRP	"/usr/bin/newgrp"
#endif

#endif /* _h_pathnames */


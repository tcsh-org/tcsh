/*
 * config_features.h -- configure various defines for tcsh
 *
 * This is included by config.h.
 *
 * Edit this to match your particular feelings; this is set up to the
 * way I like it.
 */

/****************** configurable hacks ****************/
/*
 * SHORT_STRINGS Use 16 bit characters instead of 8 bit chars
 * 	         This fixes up quoting problems and eases implementation
 *	         of nls...
 *
 */
#define SHORT_STRINGS

/*
 * NLS:		Use Native Language System
 *		Routines like setlocale() are needed 
 *		if you don't have <locale.h>, you don't want
 *		to define this.
 */
#define NLS 

/*
 * LOGINFIRST   Source ~/.login before ~/.cshrc
 */
#undef LOGINFIRST

/*
 * VIDEFAULT    Make the VI mode editor the default
 */
#undef VIDEFAULT

/*
 * KAI          use "bye" command and rename "log" to "watchlog"
 */
#undef KAI

/*
 * CSHDIRS    save a history like stack of directories
 */
#define CSHDIRS

/*
 * TESLA	drops DTR on logout. Historical note:
 *		tesla.ee.cornell.edu was a vax11/780 with a develcon
 *		switch that sometimes would not hang up.
 */
#undef TESLA

/*
 * DOTLAST      put "." last in the default path, for security reasons
 */
#define DOTLAST

/*
 * AUTOLOGOUT	tries to determine if it should set autologout depending
 *		on the name of the tty, and environment.
 *		Does not make sense in the modern window systems!
 */
#define AUTOLOGOUT

/*
 * SUSPENDED	Newer shells say 'Suspended' instead of 'Stopped'.
 *		Define to get the same type of messages.
 */
#define SUSPENDED

/*
 * KANJI	Ignore meta-next, and the ISO character set. Should
 *		be used with SHORT_STRINGS
 *
 */
#undef KANJI

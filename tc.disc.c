/* $Header$ */
/*
 * tc.disc.c: Functions to set/clear line disciplines
 *
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id$";

#endif

#include "sh.h"
#include "sh.local.h"

#ifdef OREO
#include <compat.h>
#endif				/* OREO */

static bool add_discipline = 0;	/* Did we add a line discipline	 */

#if defined(IRIS4D) || defined(OREO)
#define HAVE_DISC
#ifndef POSIX
static struct termio otermiob;

#else
static struct termios otermiob;

#endif
#endif				/* IRIS4D || OREO */

#ifdef _IBMR2
#define HAVE_DISC
char    strPOSIX[] = "posix";

#endif				/* _IBMR2 */

#if !defined(HAVEDISC) && defined(TIOCGETD) && defined(NTTYDISC)
static int oldisc;

#endif				/* !HAVEDISC && TIOCGETD && NTTYDISC */

int
/*ARGSUSED*/
setdisc(f)
int     f;
{
#ifdef IRIS4D
#define HAVE_DISC
#ifndef POSIX
    struct termio termiob;

#else
    struct termios termiob;

#endif

    if (ioctl(f, TCGETA, (ioctl_t) & termiob) == 0) {
	otermiob = termiob;
	if (termiob.c_line != NTTYDISC || termiob.c_cc[VSWTCH] == 0) {
	    termiob.c_line = NTTYDISC;
	    termiob.c_cc[VSWTCH] = CSWTCH;
	    if (ioctl(f, TCSETA, (ioctl_t) & termiob) != 0)
		return (-1);
	}
    }
    else
	return (-1);
    add_discipline = 1;
    return (0);
#endif				/* IRIS4D */


#ifdef OREO
#define HAVE_DISC
#ifndef POSIX
    struct termio termiob;

#else
    struct termios termiob;

#endif
    struct ltchars ltcbuf;

    if (ioctl(f, TCGETA, (ioctl_t) & termiob) == 0) {
	if ((getcompat(COMPAT_BSDTTY) & COMPAT_BSDTTY) != COMPAT_BSDTTY) {
	    setcompat(COMPAT_BSDTTY);
	    otermiob = termiob;
	    if (ioctl(f, TIOCGLTC, (ioctl_t) & ltcbuf) != 0)
		CSHprintf("Couldn't get local chars.\n");
	    else {
		ltcbuf.t_suspc = '\032';	/* ^Z */
		ltcbuf.t_dsuspc = '\031';	/* ^Y */
		ltcbuf.t_rprntc = '\022';	/* ^R */
		ltcbuf.t_flushc = '\017';	/* ^O */
		ltcbuf.t_werasc = '\027';	/* ^W */
		ltcbuf.t_lnextc = '\026';	/* ^V */
		if (ioctl(f, TIOCSLTC, (ioctl_t) & ltcbuf) != 0)
		    CSHprintf("Couldn't set local chars.\n");
	    }
	    termiob.c_cc[VSWTCH] = '\0';
	    if (ioctl(f, TCSETAF, (ioctl_t) & termiob) != 0)
		return (-1);
	}
    }
    else
	return (-1);
    add_discipline = 1;
    return (0);
#endif				/* OREO */


#ifdef _IBMR2
#define HAVE_DISC
    union txname tx;

    tx.tx_which = 0;

    if (ioctl(f, TXGETLD, (ioctl_t) & tx) == 0) {
	if (strcmp(tx.tx_name, strPOSIX) != 0)
	    if (ioctl(f, TXADDCD, (ioctl_t) strPOSIX) == 0) {
		add_discipline = 1;
		return (0);
	    }
	return (0);
    }
    else
	return (-1);
#endif				/* _IBMR2 */

#ifndef HAVE_DISC
# if defined(TIOCGETD) && defined(NTTYDISC)
#  define HAVE_DISC
    if (ioctl(f, TIOCGETD, (ioctl_t) & oldisc) == 0) {
	if (oldisc != NTTYDISC) {
	    int     ldisc = NTTYDISC;

	    if (ioctl(f, TIOCSETD, (ioctl_t) & ldisc) != 0)
		return (-1);
	}
	else
	    oldisc = -1;
	return (0);
    }
    else
	return (-1);
    add_discipline = 1;
    return (0);
#endif				/* TIOCGETD && NTTYDISC */
#endif				/* !HAVE_DISC */

#ifndef HAVE_DISC
# ifdef notdef
#  if defined(CSUSP) && defined(VSUSP)
#   ifdef POSIX
    struct termios termiob;

    if (tcgetattr(FSHTTY, &termiob) == 0)
	if (termiob.c_cc[VSUSP] != CSUSP) {
	    termiob.c_cc[VSUSP] = CSUSP;
	    (void) tcsetattr(FSHTTY, TCSANOW, &termiob);
	}
#   else				/* POSIX */
    struct termio termiob;

    if (ioctl(f, TCGETA, (ioctl_t) & termiob) == 0)
	if (termiob.c_cc[VSUSP] != CSUSP) {
	    termiob.c_cc[VSUSP] = CSUSP;
	    (void) ioctl(f, TCSETAF, (ioctl_t) & termiob);
	}
#   endif				/* POSIX */
#  endif				/* CSUSP && VSUSP */
# endif
    return (0);
#endif				/* !HAVE_DISC */

}


#undef HAVE_DISC

int
/*ARGSUSED*/
resetdisc(f)
int f;
{
    if (add_discipline) {
	add_discipline = 0;
#if defined(OREO) || defined(IRIS4D)
#define HAVE_DISC
	return (ioctl(f, TCSETAF, &otermiob));
#endif				/* OREO || IRIS4D */

#ifdef _IBMR2
#define HAVE_DISC
	return (ioctl(FSHTTY, TXDELCD, (ioctl_t) strPOSIX));
#endif				/* _IBMR2 */

#ifndef HAVE_DISC
#if defined(TIOCSETD) && defined(NTTYDISC)
#define HAVE_DISC
	return (ioctl(f, TIOCSETD, (ioctl_t) & oldisc));
#endif				/* TIOCSETD && NTTYDISC */
#endif				/* !HAVE_DISC */
    }

#ifndef HAVE_DISC
    return (0);
#endif				/* !HAVE_DISC */
}				/* end resetdisp */

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/pwprintf.c,v 1.5 1990/11/25 10:12:00 christos Exp $ */
/*
 * tc.printf.c: A public-domain, minimal printf/sprintf routine that prints 
 *	       through the putchar() routine.  Feel free to use for 
 *	       anything...  -- 7/17/87 Paul Placeway 
 */
#if !defined(CSH_SPRINTF) && !defined(CSH_PRINTF)
#ifndef lint
static char *rcsid = "$Id: pwprintf.c,v 1.5 1990/11/25 10:12:00 christos Exp $";
#endif

#include "config.h"

#include "sh.h"
#include "sh.char.h"
#include <varargs.h>

#ifdef lint
#undef va_arg
#define va_arg(a, b) (a ? (b) 0 : (b) 0)
#endif

/* use varargs since it's the RIGHT WAY, and assuming things about parameters
   on the stack will be wrong on a register passing machine (Pyramid) */

#define INF	32766		/* should be bigger than any field to print */

static unsigned char buf[128];
#define CSH_SPRINTF
#define PUTCHAR(a, b) *a++ = (char) b
#include "tc.printf.c"
#undef CSH_SPRINTF
#undef PUTCHAR
#define CSH_PRINTF
#define PUTCHAR(a, b) CSHputchar(b)
#include "tc.printf.c"
#undef CSH_PRINTF
#undef PUTCHAR
#endif /* !CSH_SPRINTF && !CSH_PRINTF */

#ifdef CSH_PRINTF
void
/*VARARGS*/
CSHprintf (sfmt, va_alist)
char *sfmt;
va_dcl
#endif /* CSH_PRINTF */

#ifdef CSH_SPRINTF
void
/*VARARGS*/
CSHsprintf (dest, sfmt, va_alist)
char *dest, *sfmt;
va_dcl
#endif /* CSH_SPRINTF */

#if defined(CSH_SPRINTF) || defined(CSH_PRINTF)
{
    va_list ap;
    register unsigned char *f, *bp;
    register long l;
    register unsigned long u;
    register int i;
    register int fmt;
    register unsigned char pad = ' ';
    int flush_left = 0, f_width = 0, prec = INF, hash = 0, do_long = 0;
    int sign = 0;
    int attributes = 0;

    va_start(ap);
    
    f = (unsigned char *)sfmt;
    for (; *f; f++) {
	if (*f != '%') {	/* then just out the char */
	    PUTCHAR(dest, (int) (*f | attributes));
	} else {
	    f++;		/* skip the % */

	    if (*f == '-') {	/* minus: flush left */
		flush_left = 1;
		f++;
	    }

	    if (*f == '0' || *f == '.') {	
		/* padding with 0 rather than blank */
		pad = '0';
		f++;
	    }
	    if (*f == '*') {	/* field width */
		f_width = va_arg(ap, int);
		f++;
	    } else if (isdigit(*f)) {
		f_width = atoi ((char *) f);
		while (isdigit(*f)) f++; /* skip the digits */
	    }
	   
	    if (*f == '.') {	/* precision */
		f++;
		if (*f == '*') {
		    prec = va_arg(ap, int);
		    f++;
		} else if (isdigit(*f)) {
		    prec = atoi ((char *) f);
		    while (isdigit(*f)) f++; /* skip the digits */
		}
	    }

	    if (*f == '#') {	/* alternate form */
		hash = 1;
		f++;
	    }

	    if (*f == 'l') {	/* long format */
		do_long = 1;
		f++;
	    }

	    fmt = *f;
	    if (isupper(fmt)) {
		do_long = 1;
		fmt = tolower(fmt);
	    }
	    bp = buf;
	    switch (fmt) {	/* do the format */
	      case 'd':
		if (do_long)
		    l = va_arg(ap, long);
		else
		    l = (long) ( va_arg(ap, int) );
		if (l < 0) {
		    sign = 1;		    
		    l = -l;
		}
		do {
		    *bp++ = l % 10 + '0';
		} while ((l /= 10) > 0);
		if (sign)
		    *bp++ = '-';
		f_width = f_width - (bp - buf);
		if (!flush_left)
		    while (f_width-- > 0)
			PUTCHAR(dest, (int) (pad | attributes));
		for (bp--; bp >= buf; bp--)
		    PUTCHAR(dest, (int) (*bp | attributes));
		if (flush_left)
		    while (f_width-- > 0)
			PUTCHAR(dest, (int) (' ' | attributes));
		break;

	      case 'o':
	      case 'x':
	      case 'u':
		if (do_long)
		    u = va_arg(ap, unsigned long);
		else
		    u = (unsigned long) ( va_arg(ap, unsigned) );
		if (fmt == 'u') { /* unsigned decimal */
		    do {
			*bp++ = u % 10 + '0';
		    } while ((u /= 10) > 0);
		} else if (fmt == 'o') { /* octal */
		    do {
			*bp++ = u % 8 + '0';
		    } while ((u /= 8) > 0);
		    if (hash)
			*bp++ = '0';
		} else if (fmt == 'x') { /* hex */
		    do {
			i = u % 16;
			if (i < 10)
			    *bp++ = i + '0';
			else
			    *bp++ = i - 10 + 'a';
		    } while ((u /= 16) > 0);
		    if (hash) {
			*bp++ = 'x';
			*bp++ = '0';
		    }
		}
		i = f_width - (bp - buf);
		if (!flush_left)
		    while (i-- > 0)
			PUTCHAR(dest, (int) (pad | attributes));
		for (bp--; bp >= buf; bp--)
		    PUTCHAR(dest, (int) (*bp | attributes));
		if (flush_left)
		    while (i-- > 0)
			PUTCHAR(dest, (int) (' ' | attributes));
		break;
		

	      case 'c':
		i = va_arg(ap, int);
		PUTCHAR(dest, (int) (i | attributes));
		break;

	      case 's':
		bp = va_arg(ap, unsigned char *);
		if (!bp) bp = (unsigned char *) "(nil)";
		f_width = f_width - strlen((char *) bp);
		if (!flush_left)
		    while (f_width-- > 0)
			PUTCHAR(dest, (int) (pad | attributes));
	        for (i = 0; *bp && i < prec; i++) {
		    PUTCHAR(dest, (int) (*bp | attributes));
		    bp++;
		}
		if (flush_left)
		    while (f_width-- > 0)
			PUTCHAR(dest, (int) (' ' | attributes));

		break;

              case 'a':
                attributes = va_arg(ap,int);
                break;

	      case '%':
		PUTCHAR(dest, (int) ('%' | attributes));
		break;
	    }
	    flush_left = 0, f_width = 0, prec = INF, hash = 0, do_long = 0;
	    sign = 0;
	    pad = ' ';
	}
    }
    va_end(ap);
#ifdef CSH_SPRINTF
    *dest = '\0';
#endif /* CSH_SPRINTF */
}
#endif /* CSH_SPRINTF || CSH_PRINTF */

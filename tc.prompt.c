/* $Header$ */
/*
 * tc.prompt.c: Prompt printing stuff
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id$";
#endif /* lint */

#include "sh.h"
#include "sh.local.h"

/*
 * kfk 21oct1983 -- add @ (time) and / ($cwd) in prompt.
 * PWP 4/27/87 -- rearange for tcsh.
 * mrdch@com.tau.edu.il 6/26/89 - added ~, T and .# - rearanged to switch()
 *                 instead of if/elseif
 */
 
char *month_list[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
			 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
void
printprompt (promptno, str)
int promptno;
Char *str;
{
    register Char *p, *z, *q;
    register Char attributes = 0;
    static int print_prompt_did_ding = 0;
    register char *cz;
    struct tm *t;
    long lclock;
    Char buff[BUFSIZ];
    Char *cp;
 
    (void) time (&lclock);
    t = localtime(&lclock);

    PromptBuf[0] = '\0';
    p = PromptBuf;
    switch (promptno) {
    default:
    case 0:
	cp = value(STRprompt);
	break;
    case 1:
	cp = value(STRprompt2);
	break;
    case 2:
	cp = value(STRprompt3);
	break;
    }
 
    for (; *cp; cp++) {
	if (*cp == '%') {
	    cp++;
	    switch (*cp) {
	      case 'R':
	        if (str != (Char *) 0)
		    while (*str)
			*p++ = attributes | *str++;
		break;
	      case '#':
		*p++ = attributes | ((uid == 0) ? '#' : '>');
		break;
	      case '!':
	      case 'h':
		Itoa(eventno + 1, buff);
		for (z = buff; *z; z++)
		    *p++ = attributes | *z;
		break;
	      case 'T':		/* 24 hour format	*/
	      case '@':
	      case 't':		/* 12 hour am/pm format */
		{
		    char ampm = 'a';
		    int hr = t->tm_hour;

		    /* addition by Hans J. Albertsson */
		    /* and another adapted from Justin Bur */
		    if ( adrof(STRampm) || *cp != 'T' ) {
			if (hr >= 12) {
			    if (hr > 12)
				hr -= 12;
			    ampm = 'p';
			} else if (hr == 0)
			    hr = 12;
		    }		/* else do a 24 hour clock */

		    /* "DING!" stuff by Hans also */
		    if ( t->tm_min || print_prompt_did_ding 
			 /* || !adrof(STRprompt_ding) */     ) {
			if ( t->tm_min ) print_prompt_did_ding = 0;
			Itoa(hr, buff);
			*p++ = attributes | buff[0];
			if (buff[1])
			    *p++ = attributes | buff[1];
			*p++ = attributes | ':';
			Itoa(t->tm_min, buff);
			if (buff[1]) {
			    *p++ = attributes | buff[0];
			    *p++ = attributes | buff[1];
			} else {
			    *p++ = attributes | '0';
			    *p++ = attributes | buff[0];
			}
			if ( adrof(STRampm) || *cp != 'T' ) {
			    *p++ = attributes | ampm;
			    *p++ = attributes | 'm';
			}
		    } else {	/* we need to ding */
			int i = 0;

			(void) Strcpy(buff, STRDING);
			while ( buff[i] ) {
			    *p++ = attributes | buff[i++];
			}
			print_prompt_did_ding = 1;
		    }
		}
		break;

	      case 'M':
		/* Bug pointed out by Laurent Dami <dami@cui.unige.ch>: don't
		   derefrence that NULL (if HOST is not set)... */
		if ((cz = getenv("HOST")) != (char *) 0 )
		    while (*cz)
			*p++ = attributes | *cz++;
		break;

	      case 'm':
		if ((cz = getenv("HOST")) != (char *) 0)
		    while (*cz && *cz != '.')
			*p++ = attributes | *cz++;
		break;

	      case '~':	/* show ~ whenever possible - a la dirs */
		{
		    register int j;

		    if (!(z = value(STRcwd)))
			break;	/* no cwd, so don't do anything */
		    q = value(STRhome);
		    (void) Strcpy (buff, z);
		    /* Bug fix from Mark Nagel (nagel@buckaroo.ICS.UCI.EDU):
		     * Don't match /home/news with /home/newsbin... */
		    if (q && ! Strncmp(buff, q, (j = Strlen(q))) &&
			(buff[j] == '/' || buff[j] == '\0')) {
			*p++ = attributes | '~';
			for (z = buff + j; *z; z++) 
			    *p++ = attributes  | *z;
			break;
		    }
		}
		/* fall through if ~ not matched */
	      case '/':
	      case 'd':
		if (z = value(STRcwd)) {
		    while (*z)
			*p++ = attributes | *z++;
		}
		break;
	      case '.':
	      case 'c':
	      case 'C':
		{
		    register int j, k;
		    Char scp;

		    scp = *cp;
		    /* option to determine fix number of dirs from path */
		    if (*(cp + 1) >= '1' && *(cp + 1) <= '9') {
			j = *(cp + 1) - '0';
			cp++;
		    } else {
			j = 1;
		    }
		    if (!(z = value(STRcwd)))
			break;
		    (void) Strcpy (buff, z);
		    if (!buff[1]) {	/* if CWD == / */
			*p++ = attributes | buff[0];
		    } else {
			if ((scp != 'C') && (q = value(STRhome)) &&
			    Strncmp(buff, q, (k = Strlen(q))) == 0 &&
			    (buff[k] == '/' || buff[k] == '\0'))
			{
			    buff[--k] = '~';
			    q = &buff[k];
			}
			else
			    q = buff;
			for (z = q; *z; z++)
				; /* find the end */
			while (j-- > 0 ) {
			    while ((z > q) && (*z != '/'))
				z--; /* back up */
			    if (j && z > q)
				z--;
			}
			if (*z == '/' && z != q) z++;
			while (*z)
			    *p++ = attributes | *z++;
		    }
		}
		break;
	      case 'n':
		if (z = value(STRuser))
		    while (*z)
			*p++ = attributes | *z++;
		break;
	      case 'l':
		if (z = value(STRtty))
		    while (*z)
			*p++ = attributes | *z++;
		break;
	      case 'w':
		for (cz = month_list[t->tm_mon]; *cz; )
		    *p++ = attributes | *cz++;
		*p++ = attributes | ' ';
		Itoa(t->tm_mday, buff);
		*p++ = attributes | buff[0];
		if (buff[1])
		    *p++ = attributes | buff[1];
		break;
	      case 'W':
		Itoa(t->tm_mon + 1, buff);
		if (buff[1]) {
		    *p++ = attributes | buff[0];
		    *p++ = attributes | buff[1];
		}
		else {
		    *p++ = attributes | '0';
		    *p++ = attributes | buff[0];
		}
		*p++ = attributes | '/';

		Itoa(t->tm_mday, buff);
		if (buff[1]) {
		    *p++ = attributes | buff[0];
		    *p++ = attributes | buff[1];
		}
		else {
		    *p++ = attributes | '0';
		    *p++ = attributes | buff[0];
		}
		*p++ = attributes | '/';

		Itoa(t->tm_year, buff);
		if (buff[1]) {
		    *p++ = attributes | buff[0];
		    *p++ = attributes | buff[1];
		}
		else {
		    *p++ = attributes | '0';
		    *p++ = attributes | buff[0];
		}
		break;
	      case 'D':
		Itoa(t->tm_year, buff);
		if (buff[1]) {
		    *p++ = attributes | buff[0];
		    *p++ = attributes | buff[1];
		}
		else {
		    *p++ = attributes | '0';
		    *p++ = attributes | buff[0];
		}
		*p++ = attributes | '-';

		Itoa(t->tm_mon + 1, buff);
		if (buff[1]) {
		    *p++ = attributes | buff[0];
		    *p++ = attributes | buff[1];
		}
		else {
		    *p++ = attributes | '0';
		    *p++ = attributes | buff[0];
		}
		*p++ = attributes | '-';

		Itoa(t->tm_mday, buff);
		if (buff[1]) {
		    *p++ = attributes | buff[0];
		    *p++ = attributes | buff[1];
		}
		else {
		    *p++ = attributes | '0';
		    *p++ = attributes | buff[0];
		}
		break;
	      case 'S': 	/* start standout */
		attributes |= SNODE_ANDOUT;
		break;
	      case 'B': 	/* start bold */
		attributes |= BOLD;
		break;
	      case 'U': 	/* start underline */
		attributes |= UNDER;
		break;
	      case 's': 	/* end standout */
		attributes &= ~SNODE_ANDOUT;
		break;
	      case 'b': 	/* end bold */
		attributes &= ~BOLD;
		break;
	      case 'u': 	/* end underline */
		attributes &= ~UNDER;
		break;
	      case 'L':
		ClearToBottom ();
		break;
	      case '?':
		if (z = value (STRstatus))
		    while (*z)
			*p++ = attributes | *z++;
		break;
	      case '%':
		*p++ = attributes | '%';
		break;
	      case '{':		/* literal characters start */
#if LITERAL == 0
		/*
		 * No literal capability, so skip all chars in the 
		 * literal string
		 */
		while (*cp != '\0' && (*cp != '%' || cp[1] != '}')) cp++;
#endif /* LITERAL == 0 */
		attributes |= LITERAL;
		break;
	      case '}':		/* literal characters end */
		attributes &= ~LITERAL;
		break;
	      default:
		*p++ = attributes | '%';
		*p++ = attributes | *cp;
		break;
	    }
	} else if (*cp == '\\' | *cp == '^') {
            *p++ = attributes | parseescape(&cp);
 	} else if ( *cp == '!' ) { /* EGS: handle '!'s in prompts */
	    Itoa( eventno + 1, buff );
	    for ( z=buff; *z; z++ )
		*p++ = attributes | *z;
	} else {
	    *p++ = attributes | *cp;		/* normal character */
	}
    }
    *p = '\0';
    if (!editing) {
	for (z = PromptBuf; z < p; z++)
	    (void) putraw (*z);
	flush();
    }
}

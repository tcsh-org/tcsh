/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/ed.init.c,v 3.3 1991/07/18 15:23:16 christos Exp christos $ */
/*
 * ed.init.c: Editor initializations
 */
/*-
 * Copyright (c) 1980, 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "config.h"
RCSID("$Id: ed.init.c,v 3.3 1991/07/18 15:23:16 christos Exp christos $")

#include "sh.h"
#define EXTERN			/* intern */
#include "ed.h"
#include "tc.h"
#include "ed.defns.h"

#if defined(TERMIO) || defined(POSIX)
/*
 * Aix compatible names
 */
# if defined(VWERSE) && !defined(VWERASE)
#  define VWERASE VWERSE
# endif				/* VWERSE && !VWERASE */

# if defined(VDISCRD) && !defined(VDISCARD)
#  define VDISCARD VDISCRD
# endif				/* VDISCRD && !VDISCARD */

# if defined(VSTRT) && !defined(VSTART)
#  define VSTART VSTRT
# endif				/* VSTRT && ! VSTART */

# if defined(VSTAT) && !defined(VSTATUS)
#  define VSTATUS VSTAT
# endif				/* VSTAT && ! VSTATUS */

# ifndef ONLRET
#  define ONLRET 0
# endif				/* ONLRET */

# ifndef TAB3
#  ifdef OXTABS
#   define TAB3 OXTABS
#  else
#   define TAB3 0
#  endif				/* OXTABS */
# endif				/* !TAB3 */

# ifndef ONLCR
#  define ONLCR 0
# endif				/* ONLCR */

# ifndef IEXTEN
#  define IEXTEN 0
# endif				/* IEXTEN */

# ifndef ECHOCTL
#  define ECHOCTL 0
# endif				/* ECHOCTL */

# ifndef PARENB
#  define PARENB 0
# endif				/* PARENB */

# ifndef EXTPROC
#  define EXTPROC 0
# endif				/* EXTPROC */

# ifndef FLUSHO
#  define FLUSHO  0
# endif				/* FLUSHO */

# if defined(VDISABLE) && !defined(_POSIX_VDISABLE)
#  define _POSIX_VDISABLE VDISABLE
# endif				/* VDISABLE && ! _POSIX_VDISABLE */

# ifndef _POSIX_VDISABLE
#  define _POSIX_VDISABLE -1
# endif				/* _POSIX_VDISABLE */

/*
 * Work around ISC's definition of IEXTEN which is
 * XCASE!
 */
# ifdef ISC
#  ifdef IEXTEN
#   undef IEXTEN
#  endif				/* IEXTEN */
#  define IEXTEN 0
# endif				/* ISC */
#endif				/* TERMIO || POSIX */


/* ed.init.c -- init routines for the line editor */

int     Tty_raw_mode = 0;	/* Last tty change was to raw mode */
int     MacroLvl = -1;		/* pointer to current macro nesting level; */
				/* (-1 == none) */
static int Tty_quote_mode = 0;	/* Last tty change was to quote mode */

int     Tty_eight_bit = -1;	/* does the tty handle eight bits */

extern bool GotTermCaps;

#ifdef _IBMR2
static bool edit_discipline = 0;
union txname tx_disc;
extern char strPOSIX[];
#endif				/* _IBMR2 */

static	int	dosetkey __P((char *, char *));

static struct tcshtty {
    char *t_name;
    int  t_setmask;
    int  t_clrmask;
} ttylist[] = {

#ifdef TERMIO
# define M_INPUT	0
    { "iflag", ICRNL, (INLCR|IGNCR) },
# define M_OUTPUT	1
    { "oflag", (OPOST|ONLCR), ONLRET },
# define M_CONTROL	2
    { "cflag", 0, 0 },
# define M_LINED	3
    { "lflag", (ISIG|ICANON|ECHO|ECHOE|ECHOCTL|IEXTEN),
	       (NOFLSH|ECHOK|ECHONL|EXTPROC|FLUSHO) },

#else /* GSTTY */

# define M_CONTROL	0
    { "", (ECHO|CRMOD|ANYP), (CBREAK|RAW|LCASE|VTDELAY|ALLDELAY) },
# define M_LOCAL	1
    { "", (LCRTBS|LCRTERA|LCRTKIL), (LPRTERA|LFLUSHO) };

#endif /* TERMIO */
};

static struct tcshmodes {
    char *m_name;
    int   m_value;
    int   m_type;
} modelist[] = {
#ifdef TERMIO

# ifdef	IGNBRK
    { "ignbrk",	IGNBRK,	M_INPUT },
# endif /* IGNBRK */
# ifdef	BRKINT
    { "brkint",	BRKINT,	M_INPUT },
# endif /* BRKINT */
# ifdef	IGNPAR
    { "ignpar",	IGNPAR,	M_INPUT },
# endif /* IGNPAR */
# ifdef	PARMRK
    { "parmrk",	PARMRK,	M_INPUT },
# endif /* PARMRK */
# ifdef	INPCK
    { "inpck",	INPCK,	M_INPUT },
# endif /* INPCK */
# ifdef	ISTRIP
    { "istrip",	ISTRIP,	M_INPUT },
# endif /* ISTRIP */
# ifdef	INLCR
    { "inlcr",	INLCR,	M_INPUT },
# endif /* INLCR */
# ifdef	IGNCR
    { "igncr",	IGNCR,	M_INPUT },
# endif /* IGNCR */
# ifdef	ICRNL
    { "icrnl",	ICRNL,	M_INPUT },
# endif /* ICRNL */
# ifdef	IUCLC
    { "iuclc",	IUCLC,	M_INPUT },
# endif /* IUCLC */
# ifdef	IXON
    { "ixon",	IXON,	M_INPUT },
# endif /* IXON */
# ifdef	IXANY
    { "ixany",	IXANY,	M_INPUT },
# endif /* IXANY */
# ifdef	IXOFF
    { "ixoff",	IXOFF,	M_INPUT },
# endif /* IXOFF */
# ifdef  IMAXBEL
    { "imaxbel",IMAXBEL,M_INPUT },
# endif /* IMAXBEL */

# ifdef	OPOST
    { "opost",	OPOST,	M_OUTPUT },
# endif /* OPOST */
# ifdef	OLCUC
    { "olcuc",	OLCUC,	M_OUTPUT },
# endif /* OLCUC */
# ifdef	ONLCR
    { "onlcr",	ONLCR,	M_OUTPUT },
# endif /* ONLCR */
# ifdef	OCRNL
    { "ocrnl",	OCRNL,	M_OUTPUT },
# endif /* OCRNL */
# ifdef	ONOCR
    { "onocr",	ONOCR,	M_OUTPUT },
# endif /* ONOCR */
# ifdef	ONLRET
    { "onlret",	ONLRET,	M_OUTPUT },
# endif /* ONLRET */
# ifdef	OFILL
    { "ofill",	OFILL,	M_OUTPUT },
# endif /* OFILL */
# ifdef	OFDEL
    { "ofdel",	OFDEL,	M_OUTPUT },
# endif /* OFDEL */
# ifdef	NLDLY
    { "nldly",	NLDLY,	M_OUTPUT },
# endif /* NLDLY */
# ifdef	CRDLY
    { "crdly",	CRDLY,	M_OUTPUT },
# endif /* CRDLY */
# ifdef	TABDLY
    { "tabdly",	TABDLY,	M_OUTPUT },
# endif /* TABDLY */
# ifdef	XTABS
    { "xtabs",	XTABS,	M_OUTPUT },
# endif /* XTABS */
# ifdef	BSDLY
    { "bsdly",	BSDLY,	M_OUTPUT },
# endif /* BSDLY */
# ifdef	VTDLY
    { "vtdly",	VTDLY,	M_OUTPUT },
# endif /* VTDLY */
# ifdef	FFDLY
    { "ffdly",	FFDLY,	M_OUTPUT },
# endif /* FFDLY */
# ifdef	PAGEOUT
    { "pageout",PAGEOUT,M_OUTPUT },
# endif /* PAGEOUT */
# ifdef	WRAP
    { "wrap",	WRAP,	M_OUTPUT },
# endif /* WRAP */

# ifdef	CBAUD
    { "cbaud",	CBAUD,	M_CONTROL },
# endif /* CBAUD */
# ifdef	CSTOPB
    { "cstopb",	CSTOPB,	M_CONTROL },
# endif /* CSTOPB */
# ifdef	CREAD
    { "cread",	CREAD,	M_CONTROL },
# endif /* CREAD */
# ifdef	PARENB
    { "parenb",	PARENB,	M_CONTROL },
# endif /* PARENB */
# ifdef	PARODD
    { "parodd",	PARODD,	M_CONTROL },
# endif /* PARODD */
# ifdef	HUPCL
    { "hupcl",	HUPCL,	M_CONTROL },
# endif /* HUPCL */
# ifdef	CLOCAL
    { "clocal",	CLOCAL,	M_CONTROL },
# endif /* CLOCAL */
# ifdef	LOBLK
    { "loblk",	LOBLK,	M_CONTROL },
# endif /* LOBLK */
# ifdef	CIBAUD
    { "cibaud",	CIBAUD,	M_CONTROL },
# endif /* CIBAUD */
# ifdef	CRTSCTS
    { "crtscts",CRTSCTS,M_CONTROL },
# endif /* CRTSCTS */

# ifdef	ISIG
    { "isig",	ISIG,	M_LINED },
# endif /* ISIG */
# ifdef	ICANON
    { "icanon",	ICANON,	M_LINED },
# endif /* ICANON */
# ifdef	XCASE
    { "xcase",	XCASE,	M_LINED },
# endif /* XCASE */
# ifdef	ECHO
    { "echo",	ECHO,	M_LINED },
# endif /* ECHO */
# ifdef	ECHOE
    { "echoe",	ECHOE,	M_LINED },
# endif /* ECHOE */
# ifdef	ECHOK
    { "echok",	ECHOK,	M_LINED },
# endif /* ECHOK */
# ifdef	ECHONL
    { "echonl",	ECHONL,	M_LINED },
# endif /* ECHONL */
# ifdef	NOFLSH
    { "noflsh",	NOFLSH,	M_LINED },
# endif /* NOFLSH */
# ifdef	TOSTOP
    { "tostop",	TOSTOP,	M_LINED },
# endif /* TOSTOP */
# ifdef	ECHOCTL
    { "echoctl",ECHOCTL,M_LINED },
# endif /* ECHOCTL */
# ifdef	ECHOPRT
    { "echoprt",ECHOPRT,M_LINED },
# endif /* ECHOPRT */
# ifdef	ECHOKE
    { "echoke",	ECHOKE,	M_LINED },
# endif /* ECHOKE */
# ifdef	DEFECHO
    { "defecho",DEFECHO,M_LINED },
# endif /* DEFECHO */
# ifdef	FLUSHO
    { "flusho",	FLUSHO,	M_LINED },
# endif /* FLUSHO */
# ifdef	PENDIN
    { "pendin",	PENDIN,	M_LINED },
# endif /* PENDIN */
# ifdef	IEXTEN
    { "iexten",	IEXTEN,	M_LINED },
# endif /* IEXTEN */


#else /* GSTTY */


# ifdef	TANDEM
    { "tandem",	TANDEM,	M_CONTROL },
# endif /* TANDEM */
# ifdef	CBREAK
    { "cbreak",	CBREAK,	M_CONTROL },
# endif /* CBREAK */
# ifdef	LCASE
    { "lcase",	LCASE,	M_CONTROL },
# endif /* LCASE */
# ifdef	ECHO
    { "echo",	ECHO,	M_CONTROL },
# endif /* ECHO */	
# ifdef	CRMOD
    { "crmod",	CRMOD,	M_CONTROL },
# endif /* CRMOD */
# ifdef	RAW
    { "raw",	RAW,	M_CONTROL },
# endif /* RAW */
# ifdef	ODDP
    { "oddp",	ODDP,	M_CONTROL },
# endif /* ODDP */
# ifdef	EVENP
    { "evenp",	EVENP,	M_CONTROL },
# endif /* EVENP */
# ifdef	ANYP
    { "anyp",	ANYP,	M_CONTROL },
# endif /* ANYP */
# ifdef	NLDELAY
    { "nldelay",NLDELAY,M_CONTROL },
# endif /* NLDELAY */
# ifdef	TBDELAY
    { "tbdelay",TBDELAY,M_CONTROL },
# endif /* TBDELAY */
# ifdef	XTABS
    { "xtabs",	XTABS,	M_CONTROL },
# endif /* XTABS */
# ifdef	CRDELAY
    { "crdelay",CRDELAY,M_CONTROL },
# endif /* CRDELAY */
# ifdef	VTDELAY
    { "vtdelay",VTDELAY,M_CONTROL },
# endif /* VTDELAY */
# ifdef	BSDELAY
    { "bsdelay",BSDELAY,M_CONTROL },
# endif /* BSDELAY */
# ifdef	CRTBS
    { "crtbs",	CRTBS,	M_CONTROL },
# endif /* CRTBS */
# ifdef	PRTERA
    { "prtera",	PRTERA,	M_CONTROL },
# endif /* PRTERA */
# ifdef	CRTERA
    { "crtera",	CRTERA,	M_CONTROL },
# endif /* CRTERA */
# ifdef	TILDE
    { "tilde",	TILDE,	M_CONTROL },
# endif /* TILDE */
# ifdef	MDMBUF
    { "mdmbuf",	MDMBUF,	M_CONTROL },
# endif /* MDMBUF */
# ifdef	LITOUT
    { "litout",	LITOUT,	M_CONTROL },
# endif /* LITOUT */
# ifdef	TOSTOP
    { "tostop",	TOSTOP,	M_CONTROL },
# endif /* TOSTOP */
# ifdef	FLUSHO
    { "flusho",	FLUSHO,	M_CONTROL },
# endif /* FLUSHO */
# ifdef	NOHANG
    { "nohang",	NOHANG,	M_CONTROL },
# endif /* NOHANG */
# ifdef	L001000
    { "l001000",	L001000,	M_CONTROL },
# endif /* L001000 */
# ifdef	CRTKIL
    { "crtkil",	CRTKIL,	M_CONTROL },
# endif /* CRTKIL */
# ifdef	PASS8
    { "pass8",	PASS8,	M_CONTROL },
# endif /* PASS8 */
# ifdef	CTLECH
    { "ctlech",	CTLECH,	M_CONTROL },
# endif /* CTLECH */
# ifdef	PENDIN
    { "pendin",	PENDIN,	M_CONTROL },
# endif /* PENDIN */
# ifdef	DECCTQ
    { "decctq",	DECCTQ,	M_CONTROL },
# endif /* DECCTQ */
# ifdef	NOFLSH
    { "noflsh",	NOFLSH,	M_CONTROL },
# endif /* NOFLSH */

# ifdef	LCRTBS
    { "lcrtbs",	LCRTBS,	M_LOCAL },
# endif /* LCRTBS */
# ifdef	LPRTERA
    { "lprtera",	LPRTERA,	M_LOCAL },
# endif /* LPRTERA */
# ifdef	LCRTERA
    { "lcrtera",	LCRTERA,	M_LOCAL },
# endif /* LCRTERA */
# ifdef	LTILDE
    { "ltilde",	LTILDE,	M_LOCAL },
# endif /* LTILDE */
# ifdef	LMDMBUF
    { "lmdmbuf",	LMDMBUF,	M_LOCAL },
# endif /* LMDMBUF */
# ifdef	LLITOUT
    { "llitout",	LLITOUT,	M_LOCAL },
# endif /* LLITOUT */
# ifdef	LTOSTOP
    { "ltostop",	LTOSTOP,	M_LOCAL },
# endif /* LTOSTOP */
# ifdef	LFLUSHO
    { "lflusho",	LFLUSHO,	M_LOCAL },
# endif /* LFLUSHO */
# ifdef	LNOHANG
    { "lnohang",	LNOHANG,	M_LOCAL },
# endif /* LNOHANG */
# ifdef	LCRTKIL
    { "lcrtkil",	LCRTKIL,	M_LOCAL },
# endif /* LCRTKIL */
# ifdef	LPASS8
    { "lpass8",	LPASS8,	M_LOCAL },
# endif /* LPASS8 */	
# ifdef	LCTLECH
    { "lctlech",	LCTLECH,	M_LOCAL },
# endif /* LCTLECH */
# ifdef	LPENDIN
    { "lpendin",	LPENDIN,	M_LOCAL },
# endif /* LPENDIN */
# ifdef	LDECCTQ
    { "ldecctq",	LDECCTQ,	M_LOCAL },
# endif /* LDECCTQ */
# ifdef	LNOFLSH
    { "lnoflsh",	LNOFLSH,	M_LOCAL },
# endif /* LNOFLSH */

#endif /* TERMIO */
    { NULL, 0, -1 },
};


void
dosetty(v, t)
    Char **v;
    struct command *t;
{
    struct tcshmodes *m;
    char x, *d;
    int aflag = 0;
    Char *s;

    setname(short2str(*v++));

    if (v && *v && v[0][0] == '-' && v[0][1] == 'a' && v[0][2] == '\0') {
	aflag = 1;
	v++;
    }
    if (!v || !*v) {
	int i = -1;
	int len = 0, st = 0, cu;
	for (m = modelist; m->m_name; m++) {
	    if (m->m_type != i) {
		xprintf("%s%s:", i != -1 ? "\n" : "", 
			ttylist[m->m_type].t_name);
		i = m->m_type;
		st = len = strlen(ttylist[m->m_type].t_name) + 1;
	    }

	    x = (ttylist[i].t_setmask & m->m_value) ? '+' : '\0';
	    x = (ttylist[i].t_clrmask & m->m_value) ? '-' : x;

	    if (x != '\0' || aflag) {
		cu = strlen(m->m_name) + (x != '\0') + 1;
		if (len + cu >= T_Cols) {
		    xprintf("\n%*s", st, "");
		    len = st + cu;
		}
		else 
		    len += cu;
		if (x != '\0')
		    xprintf("%c%s ", x, m->m_name);
		else
		    xprintf("%s ", m->m_name);
	    }
	}
	xprintf("\n");
	return;
    }
    while (v && (s = *v++)) {
	switch (*s) {
	case '+':
	case '-':
	    x = *s++;
	    break;
	default:
	    x = '\0';
	    break;
	}
	d = short2str(s);
	for (m = modelist; m->m_name; m++)
	    if (strcmp(m->m_name, d) == 0)
		break;
	if (!m->m_name) 
	    stderror(ERR_NAME | ERR_SYSTEM, d, "Invalid argument");

	switch (x) {
	case '+':
	    ttylist[m->m_type].t_setmask |= m->m_value;
	    ttylist[m->m_type].t_clrmask &= ~m->m_value;
	    break;
	case '-':
	    ttylist[m->m_type].t_setmask &= ~m->m_value;
	    ttylist[m->m_type].t_clrmask |= m->m_value;
	    break;
	default:
	    ttylist[m->m_type].t_setmask &= ~m->m_value;
	    ttylist[m->m_type].t_clrmask &= ~m->m_value;
	    break;
	}
    }
} /* end dosetty */


#ifdef SIG_WINDOW
void
check_window_size(force)
    int     force;
{
#ifdef BSDSIGS
    sigmask_t omask;
#endif				/* BSDSIGS */
    int     lins, cols;

    /* don't want to confuse things here */
#ifdef BSDSIGS
    omask = sigblock(sigmask(SIG_WINDOW)) & ~sigmask(SIG_WINDOW);
#else				/* BSDSIGS */
    (void) sighold(SIG_WINDOW);
#endif				/* BSDSIGS */

    /*
     * From: bret@shark.agps.lanl.gov (Bret Thaeler) Avoid sunview bug, where a
     * partially hidden window gets a SIG_WINDOW every time the text is
     * scrolled
     */
    if (GetSize(&lins, &cols) || force) {
	if (GettingInput) {
	    ClearLines();
	    ClearDisp();
	    MoveToLine(0);
	    MoveToChar(0);
	    ChangeSize(lins, cols);
	    Refresh();
	}
	else
	    ChangeSize(lins, cols);
    }

#ifdef BSDSIGS
    (void) sigsetmask(omask);	/* can change it again */
#else				/* BSDSIGS */
    (void) sigrelse(SIG_WINDOW);
#endif				/* BSDSIGS */
}

sigret_t
/*ARGSUSED*/
window_change(snum)
int snum;
{
#if (SVID > 0) && (SVID < 3)
    /* If we were called as a signal handler, restore it. */
    if (snum > 0)
      sigset(snum, window_change);
#endif /* SVID > 0 && SVID < 3 */
    check_window_size(0);
#ifndef SIGVOID
    return (snum);
#endif
}

#endif				/* SIG_WINDOW */

/* LPASS8 is new in 4.3, and makes cbreak mode provide all 8 bits.  */
#ifndef LPASS8
# define LPASS8 0		/* we don't have it.  Too bad!! */
#endif

#ifndef CTRL
# define	CTRL(c)	('c'&037)
#endif

void
ed_set_tty_eight_bit()
{
#ifndef POSIX
# ifdef TERMIO
    struct termio ttynio;
# else				/* GSTTY */
    int     ttynlb;
# endif				/* TERMIO */
#else				/* POSIX */
    struct termios ttynio;
#endif				/* POSIX */

#ifndef POSIX
# ifdef TERMIO
    (void) ioctl(SHIN, TCGETA, (ioctl_t) & ttynio);
# else				/* GSTTY */
    (void) ioctl(SHIN, TIOCLGET, (ioctl_t) & ttynlb);
# endif				/* TERMIO */
#else				/* POSIX */
    (void) tcgetattr(SHIN, &ttynio);
#endif				/* POSIX */

#if defined(TERMIO) || defined(POSIX)
    Tty_eight_bit = (ttynio.c_cflag & CSIZE) == CS8;
#else				/* GSTTY */
    Tty_eight_bit = ttynlb & (LPASS8 | LLITOUT);
#endif
}

void
ed_Init()
{
    static int havesetup = 0;

    ResetInLine();		/* reset the input pointers */
    GettingInput = 0;		/* just in case */
    LastKill = KillBuf;		/* no kill buffer */

#ifdef DEBUG_EDIT
    CheckMaps();		/* do a little error checking on key maps */
#endif

    if (!havesetup) {		/* if we have never been called */
	replacemode = 0;	/* start out in insert mode */
	ed_InitMaps();
	Hist_num = 0;
	Expand = 0;

#ifndef POSIX
# ifdef TERMIO
	(void) ioctl(SHIN, TCGETA, (ioctl_t) & nio);
#  ifdef CBAUD
	T_Speed = nio.c_cflag & CBAUD;
#  else
	T_Speed = 0;
#  endif
# else				/* GSTTY */
	(void) ioctl(SHIN, TIOCGETP, (ioctl_t) & nb);	/* normal setup */
	xb = nb;		/* new setup */
	(void) ioctl(SHIN, TIOCGETC, (ioctl_t) & ntc);
	xtc = ntc;
	(void) ioctl(SHIN, TIOCGLTC, (ioctl_t) & nlc);
	xlc = nlc;
#  ifdef TIOCGPAGE
	(void) ioctl(SHIN, TIOCGPAGE, (ioctl_t) & npc);
	xpc = npc;
#  endif				/* TIOCGPAGE */
	(void) ioctl(SHIN, TIOCLGET, (ioctl_t) & nlb);
	xlb = nlb;
	T_Speed = nb.sg_ispeed;
# endif				/* TERMIO */
#else				/* POSIX */
	(void) tcgetattr(SHIN, &nio);
	T_Speed = cfgetispeed(&nio);
#endif				/* POSIX */

#if defined(TERMIO) || defined(POSIX)
	xio = nio;

	if ((nio.c_oflag & TAB3) == TAB3) {	/* then no tabs */
	    T_Tabs = 0;
	}
	else {
	    T_Tabs = 1;
	}

#ifdef notdef
	nio.c_iflag &= ~(INLCR | IGNCR);
	nio.c_iflag |= (ICRNL);

	nio.c_oflag &= ~(ONLRET);
	nio.c_oflag |= (OPOST | ONLCR);
	/* don't muck with c_cflag */
	nio.c_lflag &= ~(NOFLSH | ECHOK | ECHONL | EXTPROC | FLUSHO);
	nio.c_lflag |= (ISIG | ICANON | ECHO | ECHOE | ECHOCTL | IEXTEN);
#endif
	nio.c_iflag &= ~ttylist[M_INPUT].t_clrmask;
	nio.c_iflag |=  ttylist[M_INPUT].t_setmask;

	nio.c_oflag &= ~ttylist[M_OUTPUT].t_clrmask;
	nio.c_oflag |=  ttylist[M_OUTPUT].t_setmask;

	nio.c_cflag &= ~ttylist[M_CONTROL].t_clrmask;
	nio.c_cflag |=  ttylist[M_CONTROL].t_setmask;

	nio.c_lflag &= ~ttylist[M_LINED].t_clrmask;
	nio.c_lflag |=  ttylist[M_LINED].t_setmask;

# ifdef IRIX3_3
	nio.c_line = NTTYDISC;
# endif				/* IRIX3_3 */
	nio.c_cc[VINTR] = '\003';	/* ^C */
	nio.c_cc[VQUIT] = '\034';	/* ^\ */
	nio.c_cc[VERASE] = '\177';	/* ^? */
	nio.c_cc[VKILL] = '\025';	/* ^U */
	nio.c_cc[VEOF] = '\004';/* ^D */
	nio.c_cc[VEOL] = _POSIX_VDISABLE;
# ifdef VEOL2
	nio.c_cc[VEOL2] = _POSIX_VDISABLE;
# endif
# ifdef VSWTCH
	nio.c_cc[VSWTCH] = _POSIX_VDISABLE;
# endif				/* VSWTCH */
# ifdef VSTART
	nio.c_cc[VSTART] = '\021';	/* ^Q */
# endif				/* VSTART */
# ifdef VSTOP
	nio.c_cc[VSTOP] = '\023';	/* ^S */
# endif				/* VSTOP */
# ifdef VWERASE
	nio.c_cc[VWERASE] = '\027';	/* ^W */
# endif				/* VWERASE */
# ifdef VSUSP
	nio.c_cc[VSUSP] = CSUSP;
# endif
# ifdef VDSUSP
	nio.c_cc[VDSUSP] = '\031';	/* ^Y */
# endif				/* VDSUSP */
# ifdef VREPRINT
	nio.c_cc[VREPRINT] = '\022';	/* ^R */
# endif				/* WREPRINT */
# ifdef VDISCARD
	nio.c_cc[VDISCARD] = '\017';	/* ^O */
# endif				/* VDISCARD */
# ifdef VLNEXT
	nio.c_cc[VLNEXT] = '\026';	/* ^V */
# endif				/* VLNEXT */
# ifdef VSTATUS
	nio.c_cc[VSTATUS] = '\024';	/* ^T */
# endif				/* VSTATUS */
# ifdef VPGOFF
	nio.c_cc[VPGOFF] = ' ';	/* " " */
# endif				/* VPGOFF */
# ifdef VPAGE
	nio.c_cc[VPAGE] = '\015';	/* ^M */
# endif				/* VPAGE */

# if (defined(OREO) || defined(hpux) || defined(_IBMR2)) && !defined(hp9000s500)

	(void) ioctl(SHIN, TIOCGLTC, (ioctl_t) & nlc);
	xlc = nlc;
#  ifdef VSUSP
	nlc.t_suspc = nio.c_cc[VSUSP];	/* stop process signal	 */
#  else
	nlc.t_suspc = '\032';	/* stop process signal	 */
#  endif
	nlc.t_dsuspc = '\031';	/* delayed stop process signal */
#  ifdef hpux
	/*
	 * These must be 0377. (Reserved)
	 */
	nlc.t_rprntc = _POSIX_VDISABLE;	/* reprint line */
	nlc.t_flushc = _POSIX_VDISABLE;	/* flush output (toggles) */
	nlc.t_werasc = _POSIX_VDISABLE;	/* word erase */
	nlc.t_lnextc = _POSIX_VDISABLE;	/* literal next character */
#  else
	nlc.t_rprntc = '\022';	/* reprint line */
	nlc.t_flushc = '\017';	/* flush output (toggles) */
	nlc.t_werasc = '\027';	/* word erase */
	nlc.t_lnextc = '\026';	/* literal next character */
#  endif				/* hpux */
# endif				/* OREO || hpux || _IBMR2 */
# ifdef SIG_WINDOW
	(void) sigset(SIG_WINDOW, window_change);	/* for window systems */
# endif
#else	/* GSTTY */		/* V7, Berkeley style tty */


	if ((xb.sg_flags & XTABS) == XTABS) {	/* check for no tabs mode */
	    T_Tabs = 0;
	}
	else {
	    T_Tabs = 1;
	}

	if (T_Tabs) {		/* order of &= and |= is important to XTABS */
#ifdef notdef
	    nb.sg_flags &= ~(CBREAK | RAW | LCASE | XTABS | VTDELAY | ALLDELAY);
	    nb.sg_flags |= (ECHO | CRMOD | ANYP);
#endif
	    nb.sg_flags &= ~(ttylist[M_CONTROL].t_clrmask | XTABS);
	    nb.sg_flags |=  ttylist[M_CONTROL].t_setmask;
	}
	else {
#ifdef notdef
	    nb.sg_flags &= ~(CBREAK | RAW | LCASE | VTDELAY | ALLDELAY);
	    nb.sg_flags |= (ECHO | CRMOD | XTABS | ANYP);
#endif
	    nb.sg_flags &= ~ttylist[M_CONTROL].t_clrmask;
	    nb.sg_flags |=  (ttylist[M_CONTROL].t_setmask | XTABS);
	}
#ifdef notdef
	nlb &= ~(LPRTERA);	/* let 8-bit mode stand as set */
	nlb |= (LCRTBS | LCRTERA | LCRTKIL);
#endif
	nlb &= ~ttylist[M_LOCAL].t_clrmask;
	nlb |=  ttylist[M_LOCAL].t_setmask;

	nb.sg_erase = '\177';	/* del prev. char == DEL */
	nb.sg_kill = '\025';	/* special case of del region */

	ntc.t_intrc = '\003';	/* SIGINTR */
	ntc.t_quitc = '\034';	/* SIGQUIT */
	ntc.t_startc = '\021';	/* start output */
	ntc.t_stopc = '\023';	/* stop output	 */
	ntc.t_eofc = '\004';	/* no eof char during input... */
# ifdef masscomp
	/* NUL is prefered to <undef> in this char, DAS DEC-90. */
	ntc.t_brkc = '\0';	/* input delimiter (like nl) */
# else
	ntc.t_brkc = -1;	/* input delimiter (like nl) */
# endif

	nlc.t_suspc = '\032';	/* stop process signal	 */
	nlc.t_dsuspc = '\031';	/* delayed stop process signal	 */
	nlc.t_rprntc = '\022';	/* reprint line */
	nlc.t_flushc = '\017';	/* flush output (toggles) */
	nlc.t_werasc = '\027';	/* word erase */
	nlc.t_lnextc = '\026';	/* literal next character */

# ifdef TIOCGPAGE
	npc.tps_length = 0;
	npc.tps_lpos = 0;
	npc.tps_statc = '\024';	/* Show status ^T */
	npc.tps_pagec = '\040';	/* show next page " " */
	npc.tps_pgoffc = '\015';/* Ignore paging until input ^M */
	npc.tps_flag = 0;
# endif				/* TIOCGPAGE */

# ifdef SIG_WINDOW
	(void) sigset(SIG_WINDOW, window_change);	/* for window systems */
# endif
#endif				/* TERMIO */
    }

    /*
     * if we have been called before but GotTermCaps isn't set, our TERM has
     * changed, so get new termcaps and try again
     */

    if (!GotTermCaps)
	GetTermCaps();		/* does the obvious, but gets term type each
				 * time */

#if defined(TERMIO) || defined(POSIX)
    xio.c_iflag &= ~(IGNCR);
    xio.c_iflag |= (INLCR | ICRNL);

    Tty_eight_bit = (xio.c_cflag & CSIZE) == CS8;

    xio.c_oflag &= ~(ONLRET);
    xio.c_oflag |= (OPOST | ONLCR);

    /* don't mask out ECHOE cause programs need it */
    xio.c_lflag &= ~(NOFLSH | ICANON | ECHO | ECHOK | ECHONL | 
		     EXTPROC | IEXTEN | FLUSHO);
    xio.c_lflag |= (ISIG);

# ifdef IRIX3_3
    xio.c_line = NTTYDISC;
# endif				/* IRIX3_3 */
    xio.c_cc[VINTR] = '\003';	/* ^C */
    xio.c_cc[VQUIT] = '\034';	/* ^\ */
    xio.c_cc[VERASE] = '\177';	/* ^? */
    xio.c_cc[VKILL] = '\025';	/* ^U */
    xio.c_cc[VMIN] = 1;		/* one char at a time input */
    xio.c_cc[VTIME] = 0;	/* don't time out */
# ifdef VEOL2
    xio.c_cc[VEOL2] = _POSIX_VDISABLE;
# endif				/* VEOL2 */
# ifdef VSWTCH
    xio.c_cc[VSWTCH] = _POSIX_VDISABLE;
# endif				/* VSWTCH */
# ifdef VSTART
    xio.c_cc[VSTART] = '\021';	/* ^Q */
# endif				/* VSTART */
# ifdef VSTOP
    xio.c_cc[VSTOP] = '\023';	/* ^S */
# endif				/* VSTOP */
# ifdef VWERASE
    xio.c_cc[VWERASE] = _POSIX_VDISABLE;
# endif				/* VWERASE */
# ifdef VSUSP
    xio.c_cc[VSUSP] = _POSIX_VDISABLE;
# endif				/* VSUSP */
# ifdef VDSUSP
    xio.c_cc[VDSUSP] = _POSIX_VDISABLE;
# endif				/* VDSUSP */
# ifdef VREPRINT
    xio.c_cc[VREPRINT] = _POSIX_VDISABLE;
# endif				/* VREPRINT */
# ifdef VDISCARD
    xio.c_cc[VDISCARD] = '\017';/* ^O */
# endif				/* VDISCARD */
# ifdef VLNEXT
    xio.c_cc[VLNEXT] = _POSIX_VDISABLE;
# endif				/* VLNEXT */
# ifdef VSTATUS
    xio.c_cc[VSTATUS] = '\024';	/* ^T */
# endif				/* VSTATUS */
# ifdef VPGOFF
    xio.c_cc[VPGOFF] = ' ';	/* " " */
# endif				/* VPGOFF */
# ifdef VPAGE
    xio.c_cc[VPAGE] = '\015';	/* ^M */
# endif				/* VPAGE */

# if (defined(OREO) || defined(hpux) || defined(_IBMR2)) && !defined(hp9000s500)
#  ifdef VSUSP
    xlc.t_suspc = xio.c_cc[VSUSP];	/* stop process signal	 */
#  else
    xlc.t_suspc = _POSIX_VDISABLE;	/* stop process signal	 */
#  endif 
    xlc.t_dsuspc = _POSIX_VDISABLE;	/* delayed stop process signal	 */
#  ifdef hpux
    /*
     * These must be _POSIX_VDISABLE. (Reserved)
     */
    xlc.t_rprntc = _POSIX_VDISABLE;	/* reprint line */
    xlc.t_flushc = _POSIX_VDISABLE;	/* flush output (toggles) */
    xlc.t_werasc = _POSIX_VDISABLE;	/* word erase */
    xlc.t_lnextc = _POSIX_VDISABLE;	/* literal next character */
#  else
    xlc.t_rprntc = _POSIX_VDISABLE;	/* reprint line */
    xlc.t_flushc = '\017';		/* flush output */
    xlc.t_werasc = _POSIX_VDISABLE;	/* word erase */
    xlc.t_lnextc = _POSIX_VDISABLE;	/* literal next character */
#  endif /* hpux */
# endif	/* OREO || hpux || _IBMR2 */
#else /* GSTTY */
    if (T_Tabs) {		/* order of &= and |= is important to XTABS */
	xb.sg_flags &= ~(RAW | ECHO | LCASE | XTABS | VTDELAY | ALLDELAY);
	xb.sg_flags |= (CBREAK | CRMOD | ANYP);
    }
    else {
	xb.sg_flags &= ~(RAW | ECHO | LCASE | VTDELAY | ALLDELAY);
	xb.sg_flags |= (CBREAK | CRMOD | ANYP | XTABS);
    }

    xb.sg_erase = '\177';	/* del prev. char == DEL */
    xb.sg_kill = '\025';	/* special case of del region */
    /* bugfix by Michael Boom */

    xtc.t_intrc = '\003';	/* SIGINTR */
    xtc.t_quitc = '\034';	/* SIGQUIT */
    xtc.t_startc = '\021';	/* start output */
    xtc.t_stopc = '\023';	/* stop output	 */
    xtc.t_eofc = -1;		/* no eof char during input... */
# ifdef masscomp
    /* NUL is prefered to <undef> in this char, DAS DEC-90. */
    xtc.t_brkc = '\0';		/* input delimiter (like nl) */
# else
    xtc.t_brkc = -1;		/* input delimiter (like nl) */
# endif

    xlc.t_suspc = -1;		/* stop process signal (was CTRL(z)) */
    xlc.t_dsuspc = -1;		/* delayed stop process signal	 */
    xlc.t_rprntc = -1;		/* reprint line */
    xlc.t_flushc = '\017';	/* flush output */
    xlc.t_werasc = -1;		/* word erase */
    xlc.t_lnextc = -1;		/* literal next character */

# ifdef TIOCGPAGE
    xpc.tps_length = 0;
    xpc.tps_lpos = 0;
    xpc.tps_statc = -1;		/* disable show status */
    xpc.tps_pagec = -1;		/* disable show next page */
    xpc.tps_pgoffc = -1;	/* disable Ignore paging until input */
    xpc.tps_flag = 0;
# endif				/* TIOCGPAGE */

    xlb &= ~LPRTERA;
    xlb |= (LCRTBS | LCRTERA | LCRTKIL);
    Tty_eight_bit = nlb & (LPASS8 | LLITOUT);
#endif				/* TERMIO || POSIX */
    havesetup = 1;

    /* Make sure the tty has a well-defined initial state */
    /* But don't bother to alter the settings if we are not editing */
    if (editing) {
	Tty_raw_mode = 0;
	(void) Rawmode();
    }
}

static int
dosetkey(tcp, ncp)
    char   *tcp, *ncp;
{
    if ((*tcp & 0377) > 0 && (*tcp & 0377) < 0377) {
	*ncp = *tcp;
	return 1;
    }
    return 0;
}

/* 
 * Check and re-init the line. set the terminal into 1 char at a time mode.
 */
int
Rawmode()
{
#ifdef POSIX
    speed_t tspeed;
#endif /* POSIX */

    if (Tty_raw_mode)
	return (0);
#ifdef _IBMR2
    tx_disc.tx_which = 0;
    if (ioctl(FSHTTY, TXGETLD, (ioctl_t) & tx_disc) < 0)
	return (-1);
    if (strcmp(tx_disc.tx_name, strPOSIX) != 0) {
	edit_discipline = 1;
	if (ioctl(FSHTTY, TXSETLD, (ioctl_t) strPOSIX) < 0)
	    return (-1);
    }
#endif				/* _IBMR2 */

#ifndef POSIX
# ifdef TERMIO
    if (ioctl(SHIN, TCGETA, (ioctl_t) & testio) < 0)
	return (-1);		/* SHIN has been closed */
# else				/* GSTTY */
    if (ioctl(SHIN, TIOCGETP, (ioctl_t) & testsgb) < 0)
	return (-1);		/* SHIN has been closed */
    /* test the normal flags */
# endif				/* TERMIO */
#else				/* POSIX */
    if (tcgetattr(SHIN, &testio) < 0)
	return (-1);		/* SHIN has been closed */
#endif				/* POSIX */

#if defined(POSIX) || defined(TERMIO)
    Tty_eight_bit = (testio.c_cflag & CSIZE) == CS8;
    if (testio.c_cflag != nio.c_cflag) { 
	nio.c_cflag = testio.c_cflag;
	nio.c_cflag &= ~ttylist[M_CONTROL].t_clrmask;
	nio.c_cflag |=  ttylist[M_CONTROL].t_setmask;

	xio.c_cflag = testio.c_cflag;
    }
# ifdef POSIX
    /*
     * Fix from: Steven (Steve) B. Green <xrsbg@charney.gsfc.nasa.gov>
     * Speed was not being set up correctly under POSIX.
     */
    if ((tspeed = cfgetispeed(&testio)) == 0)
        tspeed = cfgetospeed(&testio);
    if (tspeed != cfgetispeed(&nio)) {
	T_Speed = tspeed;
	(void) cfsetispeed(&nio, T_Speed);
	(void) cfsetospeed(&nio, T_Speed);
	(void) cfsetispeed(&xio, T_Speed);
	(void) cfsetospeed(&xio, T_Speed);
    }
# else /* !POSIX */
#  ifdef CBAUD
    T_Speed = nio.c_cflag & CBAUD;
#  else
    T_Speed = 0;
#  endif /* CBAUD */
# endif /* !POSIX */

    if ((testio.c_lflag != nio.c_lflag) &&
	(testio.c_lflag != xio.c_lflag)) {
	nio.c_lflag = testio.c_lflag;
#ifdef notdef
	nio.c_lflag &= ~(NOFLSH | ECHOK | ECHONL | EXTPROC | FLUSHO);
	nio.c_lflag |= (ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOCTL);
#endif
	nio.c_lflag &= ~ttylist[M_LINED].t_clrmask;
	nio.c_lflag |=  ttylist[M_LINED].t_setmask;

	xio.c_lflag = testio.c_lflag;
	/* don't mask out ECHOE cause programs need it */
	xio.c_lflag &= ~(NOFLSH | ICANON | IEXTEN | ECHO | ECHOK |
			 ECHONL | ECHOCTL | EXTPROC | FLUSHO);
	xio.c_lflag |= (ISIG);
    }

    if ((testio.c_iflag != nio.c_iflag) &&
	(testio.c_iflag != xio.c_iflag)) {
	nio.c_iflag = testio.c_iflag;
#ifdef notdef
	nio.c_iflag &= ~(INLCR | IGNCR);
	nio.c_iflag |= (ICRNL);
#endif
	nio.c_iflag &= ~ttylist[M_INPUT].t_clrmask;
	nio.c_iflag |=  ttylist[M_INPUT].t_setmask;

	xio.c_iflag = testio.c_iflag;
	xio.c_iflag &= ~(INLCR | IGNCR);
	xio.c_iflag |= (ICRNL);
    }

    if ((testio.c_oflag != nio.c_oflag) &&
	(testio.c_oflag != xio.c_oflag)) {
	/* Christos: There was and ifdef here that would set ONLRET!?? */
	nio.c_oflag = testio.c_oflag;
#ifdef notdef
	nio.c_oflag &= ~(ONLRET);
	nio.c_oflag |= (OPOST | ONLCR);
#endif
	nio.c_oflag &= ~ttylist[M_OUTPUT].t_clrmask;
	nio.c_oflag |=  ttylist[M_OUTPUT].t_setmask;

	xio.c_oflag = testio.c_oflag;
	xio.c_oflag &= ~(ONLRET);
	xio.c_oflag |= (OPOST | ONLCR);
    }
    if ((nio.c_oflag & TAB3) == TAB3) {	/* check for no tabs mode */
	T_Tabs = 0;
    }
    else {
	T_Tabs = CanWeTab();
    }
    if (dosetkey((char *) &testio.c_cc[VINTR], (char *) &nio.c_cc[VINTR]))
	(void) dosetkey((char *) &testio.c_cc[VINTR], 
			(char *) &xio.c_cc[VINTR]);
    if (dosetkey((char *) &testio.c_cc[VQUIT], (char *) &nio.c_cc[VQUIT]))
	(void) dosetkey((char *) &testio.c_cc[VQUIT], 
			(char *) &xio.c_cc[VQUIT]);
    if (dosetkey((char *) &testio.c_cc[VERASE], (char *) &nio.c_cc[VERASE]))
	(void) dosetkey((char *) &testio.c_cc[VERASE],
			(char *) &xio.c_cc[VERASE]);
    if (dosetkey((char *) &testio.c_cc[VKILL], (char *) &nio.c_cc[VKILL]))
	(void) dosetkey((char *) &testio.c_cc[VKILL], 
			(char *) &xio.c_cc[VKILL]);
    if (testio.c_cc[VEOF] != 1) {	/* 'cause VEOL == VNUM, and 1 is raw
					 * mode */
	(void) dosetkey((char *) &testio.c_cc[VEOF], (char *) &nio.c_cc[VEOF]);
	(void) dosetkey((char *) &testio.c_cc[VEOL], (char *) &nio.c_cc[VEOL]);
    }
# ifdef VSWTCH
    if (dosetkey((char *) &testio.c_cc[VSWTCH], (char *) &nio.c_cc[VSWTCH]))
	(void) dosetkey((char *) &testio.c_cc[VSWTCH],
			(char *) &xio.c_cc[VSWTCH]);
# endif				/* VSWTCH */
# ifdef VEOL2
    if (dosetkey((char *) &testio.c_cc[VEOL2], (char *) &nio.c_cc[VEOL2]))
	(void) dosetkey((char *) &testio.c_cc[VEOL2], 
			(char *) &xio.c_cc[VEOL2]);
# endif				/* VEOL2 */
# ifdef VSTART
    if (dosetkey((char *) &testio.c_cc[VSTART], (char *) &nio.c_cc[VSTART]))
	(void) dosetkey((char *) &testio.c_cc[VSTART],
			(char *) &xio.c_cc[VSTART]);
# endif				/* VSTART */
# ifdef VSTOP
    if (dosetkey((char *) &testio.c_cc[VSTOP], (char *) &nio.c_cc[VSTOP]))
	(void) dosetkey((char *) &testio.c_cc[VSTOP], 
			(char *) &xio.c_cc[VSTOP]);
# endif				/* VSTOP */
# ifdef VWERASE
    (void) dosetkey((char *) &testio.c_cc[VWERASE], 
		    (char *) &nio.c_cc[VWERASE]);
# endif				/* VWERASE */
# ifdef VREPRINT
    (void) dosetkey((char *) &testio.c_cc[VREPRINT],
		    (char *) &nio.c_cc[VREPRINT]);
# endif				/* VREPRINT */
# ifdef VSUSP
    (void) dosetkey((char *) &testio.c_cc[VSUSP], (char *) &nio.c_cc[VSUSP]);
# endif				/* VSUSP */
# ifdef VDSUSP
    (void) dosetkey((char *) &testio.c_cc[VDSUSP], (char *) &nio.c_cc[VDSUSP]);
# endif				/* VDSUSP */
# ifdef VLNEXT
    (void) dosetkey((char *) &testio.c_cc[VLNEXT], (char *) &nio.c_cc[VLNEXT]);
# endif				/* VLNEXT */
# ifdef VDISCARD
    if (dosetkey((char *) &testio.c_cc[VDISCARD], (char *) &nio.c_cc[VDISCARD]))
	(void) dosetkey((char *) &testio.c_cc[VDISCARD],
			(char *) &xio.c_cc[VDISCARD]);
# endif				/* VDISCARD */
# ifdef VSTATUS
    if (dosetkey((char *) &testio.c_cc[VSTATUS], (char *) &nio.c_cc[VSTATUS]))
	(void) dosetkey((char *) &testio.c_cc[VSTATUS],
			(char *) &xio.c_cc[VSTATUS]);
# endif				/* VSTATUS */
# ifdef VPGOFF
    if (dosetkey((char *) &testio.c_cc[VPGOFF], (char *) &nio.c_cc[VPGOFF]))
	(void) dosetkey((char *) &testio.c_cc[VPGOFF],
			(char *) &xio.c_cc[VPGOFF]);
# endif				/* VPGOFF */
# ifdef VPAGE
    if (dosetkey((char *) &testio.c_cc[VPAGE], (char *) &nio.c_cc[VPAGE]))
	(void) dosetkey((char *) &testio.c_cc[VPAGE],
			(char *) &xio.c_cc[VPAGE]);
# endif				/* VPAGE */

# ifndef POSIX
    if (ioctl(SHIN, TCSETAW, (ioctl_t) & xio) < 0)
	return (-1);
# else /* POSIX */
    if (tcsetattr(SHIN, TCSADRAIN, &xio) < 0)
	return (-1);
# endif	/* POSIX */

# if (defined(OREO) || defined(hpux) || defined(_IBMR2)) && !defined(hp9000s500)
    /* get and set the new local chars */
    if (ioctl(SHIN, TIOCGLTC, (ioctl_t) & testlc) < 0)
	return (-1);
#ifdef VSUSP
    testlc.t_suspc = nio.c_cc[VSUSP];	/* stop process signal	 */
#endif
    (void) dosetkey((char *) &testlc.t_suspc, (char *) &nlc.t_suspc);
    (void) dosetkey((char *) &testlc.t_dsuspc, (char *) &nlc.t_dsuspc);
# ifndef hpux
    (void) dosetkey((char *) &testlc.t_rprntc, (char *) &nlc.t_rprntc);
    if (dosetkey((char *) &testlc.t_flushc, (char *) &nlc.t_flushc))
	xlc.t_flushc = nlc.t_flushc;
    (void) dosetkey((char *) &testlc.t_werasc, (char *) &nlc.t_werasc);
    (void) dosetkey((char *) &testlc.t_lnextc, (char *) &nlc.t_lnextc);
# endif /* hpux */
    if (ioctl(SHIN, TIOCSLTC, (ioctl_t) & xlc) < 0)
	return (-1);
# endif	/* OREO || hpux || _IBMR2 */

#else /* GSTTY */		/* for BSD... */

    if (testsgb.sg_ispeed != nb.sg_ispeed) {
	nb.sg_ispeed = testsgb.sg_ispeed;
	xb.sg_ispeed = testsgb.sg_ispeed;
    }
    if (testsgb.sg_ospeed != nb.sg_ospeed) {
	nb.sg_ospeed = testsgb.sg_ospeed;
	xb.sg_ospeed = testsgb.sg_ospeed;
    }
    T_Speed = nb.sg_ispeed;

    /* get and set the terminal characters */
    /* bugfix by Bruce Robertson <bruce%tigger.gwd.tek.com@relay.cs.net> */
    /* include erase and kill in the tests */
    if (dosetkey((char *) &testsgb.sg_erase, (char *) &nb.sg_erase))
	xb.sg_erase = nb.sg_erase;
    if (dosetkey((char *) &testsgb.sg_kill, (char *) &nb.sg_kill))
	xb.sg_kill = nb.sg_kill;

    (void) ioctl(SHIN, TIOCGETC, (ioctl_t) & testtc);
    if (dosetkey((char *) &testtc.t_intrc, (char *) &ntc.t_intrc))
	xtc.t_intrc = ntc.t_intrc;
    if (dosetkey((char *) &testtc.t_quitc, (char *) &ntc.t_quitc))
	xtc.t_quitc = ntc.t_quitc;
    if (dosetkey((char *) &testtc.t_startc, (char *) &ntc.t_startc))
	xtc.t_startc = ntc.t_startc;
    if (dosetkey((char *) &testtc.t_stopc, (char *) &ntc.t_stopc))
	xtc.t_stopc = ntc.t_stopc;
    if (dosetkey((char *) &testtc.t_eofc, (char *) &ntc.t_eofc))
	xtc.t_eofc = ntc.t_eofc;/* someone might want to change EOF */
    /* in general, t_brkc shouldn't change */
    (void) dosetkey((char *) &testtc.t_brkc, (char *) &ntc.t_brkc);

    if (ioctl(SHIN, TIOCLGET, (ioctl_t) & testnlb) < 0)
	return (-1);

    Tty_eight_bit = testnlb & (LPASS8 | LLITOUT);

    /* If they have changed any tty settings, we have to keep up with them. */
    /* PWP: but only believe them if changes are made to cooked mode! */
    if (((testsgb.sg_flags != nb.sg_flags) || (testnlb != nlb)) &&
	((testsgb.sg_flags != xb.sg_flags) || (testnlb != xlb)) &&
	!(testsgb.sg_flags & (RAW | CBREAK))) {

	nb.sg_flags = testsgb.sg_flags;

	/*
	 * re-test for some things here (like maybe the user typed "stty -tabs"
	 */
	if ((nb.sg_flags & XTABS) == XTABS) {	/* check for no tabs mode */
	    T_Tabs = 0;
	}
	else {
	    T_Tabs = CanWeTab();
	}

#ifdef notdef
	nb.sg_flags &= ~(CBREAK | RAW | LCASE | VTDELAY | ALLDELAY);
	nb.sg_flags |= (ECHO | CRMOD | ANYP);
#endif
	nb.sg_flags &= ~ttylist[M_CONTROL].t_clrmask;
	nb.sg_flags |=  ttylist[M_CONTROL].t_setmask;

	if (T_Tabs) {		/* order of &= and |= is important to XTABS */
	    nb.sg_flags &= ~XTABS;
	}
	else {
	    nb.sg_flags |= XTABS;
	}

	xb.sg_flags = testsgb.sg_flags;
	if (T_Tabs) {
	    xb.sg_flags &= ~(RAW | ECHO | LCASE | XTABS | VTDELAY | ALLDELAY);
	    xb.sg_flags |= (CBREAK | CRMOD | ANYP);
	}
	else {
	    xb.sg_flags &= ~(RAW | ECHO | LCASE | VTDELAY | ALLDELAY);
	    xb.sg_flags |= (CBREAK | CRMOD | ANYP | XTABS);
	}

	nlb = testnlb;
/*
 * Bugfix: turn off output flushing.  By:
 * Justin Bur
 * Universite de Montreal - IRO		uunet!utai!musocs!iros1!justin
 * Montreal (Qc) Canada H3C 3J7		  <bur%iro.udem.cdn@ubc.csnet>
 * Thanks!
 */
#ifdef notdef
	nlb &= ~(LPRTERA | LFLUSHO);
	nlb |= (LCRTBS | LCRTERA | LCRTKIL);
#endif
	nlb &= ~ttylist[M_LOCAL].t_clrmask;
	nlb |=  ttylist[M_LOCAL].t_setmask;

	xlb = nlb;
    }

    /* get and set the new local chars */
    if (ioctl(SHIN, TIOCGLTC, (ioctl_t) & testlc) < 0)
	return (-1);
# ifdef TIOCGPAGE
    if (ioctl(SHIN, TIOCGPAGE, (ioctl_t) & testpc) < 0)
	return (-1);
    (void) dosetkey((char *) &testpc.tps_statc, (char *) &npc.tps_statc);
    (void) dosetkey((char *) &testpc.tps_pagec, (char *) &npc.tps_pagec);
    (void) dosetkey((char *) &testpc.tps_pgoffc, (char *) &npc.tps_pgoffc);
# endif				/* TIOCGPAGE */

    (void) dosetkey((char *) &testlc.t_suspc, (char *) &nlc.t_suspc);
    (void) dosetkey((char *) &testlc.t_dsuspc, (char *) &nlc.t_dsuspc);
    if (dosetkey((char *) &testlc.t_flushc, (char *) &nlc.t_flushc))
	xlc.t_flushc = nlc.t_flushc;
    (void) dosetkey((char *) &testlc.t_werasc, (char *) &nlc.t_werasc);
    (void) dosetkey((char *) &testlc.t_rprntc, (char *) &nlc.t_rprntc);
    (void) dosetkey((char *) &testlc.t_lnextc, (char *) &nlc.t_lnextc);

    if (ioctl(SHIN, TIOCSETN, (ioctl_t) & xb) < 0)
	return (-1);
    if (ioctl(SHIN, TIOCSETC, (ioctl_t) & xtc) < 0)
	return (-1);
    if (ioctl(SHIN, TIOCSLTC, (ioctl_t) & xlc) < 0)
	return (-1);
# ifdef TIOCGPAGE
    if (ioctl(SHIN, TIOCSPAGE, (ioctl_t) & xpc) < 0)
	return (-1);
# endif				/* TIOCGPAGE */
    if (ioctl(SHIN, TIOCLSET, (ioctl_t) & xlb) < 0)
	return (-1);
# endif				/* TERMIO || POSIX */
    Tty_raw_mode = 1;
    flush();			/* flush any buffered output */
    return (0);
}

int
Cookedmode()
{				/* set tty in normal setup */
    sigret_t(*orig_intr) ();

#ifdef _IBMR2
    if (edit_discipline) {
	if (ioctl(SHIN, TXSETLD, (ioctl_t) & tx_disc) < 0)
	    return (-1);
	edit_discipline = 0;
    }
#endif				/* _IBMR2 */
    if (!Tty_raw_mode)
	return (0);

#ifdef BSDSIGS
    orig_intr = signal(SIGINT, SIG_IGN);	/* hold this for reseting tty */
#else
    orig_intr = sigset(SIGINT, SIG_IGN);	/* hold this for reseting tty */
#endif				/* BSDSIGS */
#ifndef POSIX
# ifdef TERMIO
    if (ioctl(SHIN, TCSETAW, (ioctl_t) & nio) < 0)
	return (-1);
# if (defined(OREO) || defined(hpux) || defined(_IBMR2)) && !defined(hp9000s500)
    if (ioctl(SHIN, TIOCSLTC, (ioctl_t) & nlc) < 0)
	return (-1);
#  endif				/* OREO || hpux || _IBMR2 */
# else	/* GSTTY */		/* for BSD... */
    if (ioctl(SHIN, TIOCSETN, (ioctl_t) & nb) < 0)
	return (-1);
    if (ioctl(SHIN, TIOCSETC, (ioctl_t) & ntc) < 0)
	return (-1);
    if (ioctl(SHIN, TIOCSLTC, (ioctl_t) & nlc) < 0)
	return (-1);
# ifdef TIOCGPAGE
    if (ioctl(SHIN, TIOCSPAGE, (ioctl_t) & npc) < 0)
	return (-1);
# endif				/* TIOCGPAGE */
    if (ioctl(SHIN, TIOCLSET, (ioctl_t) & nlb) < 0)
	return (-1);
# endif				/* TERMIO */
#else				/* POSIX */
    if (tcsetattr(SHIN, TCSADRAIN, &nio) < 0)
	return (-1);
# if defined(OREO) || defined(hpux) || defined(_IBMR2)
    if (ioctl(SHIN, TIOCSLTC, (ioctl_t) & nlc) < 0)
	return (-1);
# endif				/* OREO || hpux || _IBMR2 */
#endif				/* POSIX */
    Tty_raw_mode = 0;
#ifdef BSDSIGS
    (void) signal(SIGINT, orig_intr);	/* take these again */
#else
    (void) sigset(SIGINT, orig_intr);	/* take these again */
#endif				/* BSDSIGS */
    return (0);
}

void
ResetInLine()
{
    Cursor = InputBuf;		/* reset cursor */
    LastChar = InputBuf;
    InputLim = &InputBuf[INBUFSIZ - 2];
    Mark = InputBuf;
    MetaNext = 0;
    CurrentKeyMap = CcKeyMap;
    AltKeyMap = 0;
    Hist_num = 0;
    DoingArg = 0;
    Argument = 1;
#ifdef notdef
    LastKill = KillBuf;		/* no kill buffer */
#endif
    LastCmd = F_UNASSIGNED;	/* previous command executed */
    MacroLvl = -1;		/* no currently active macros */
}

static Char *Input_Line = NULL;
int
Load_input_line()
{
    long    chrs = 0;

    if (Input_Line)
	xfree((ptr_t) Input_Line);
    Input_Line = NULL;

    if (Tty_raw_mode)
	return 0;

#ifdef FIONREAD
    (void) ioctl(SHIN, FIONREAD, &chrs);
    if (chrs > 0) {
	char    buf[BUFSIZ];

	chrs = read(SHIN, buf, (size_t) min(chrs, BUFSIZ - 1));
	if (chrs > 0) {
	    buf[chrs] = '\0';
	    Input_Line = Strsave(str2short(buf));
	    PushMacro(Input_Line);
	}
    }
#endif
    return chrs > 0;
}

/*
 * Bugfix (in Swedish) by:
 * Johan Widen
 * SICS, PO Box 1263, S-163 13 SPANGA, SWEDEN
 * {mcvax,munnari,cernvax,diku,inria,prlb2,penet,ukc,unido}!enea!sics.se!jw
 * Internet: jw@sics.se
 *
 * (via Hans J Albertsson (thanks))
 */

void
QuoteModeOn()
{
#ifndef POSIX
# ifndef TERMIO
    struct sgttyb rawb;
# else
    struct termio rawb;
# endif				/* TERMIO */
#else				/* POSIX */
    struct termios rawb;
#endif				/* POSIX */

    if (MacroLvl >= 0)
	return;

#if defined(TERMIO) || defined(POSIX)
    rawb = xio;
    rawb.c_lflag &= ~(ISIG | IEXTEN | PARENB);
    rawb.c_iflag &= ~(IXON | IXOFF);
# ifndef POSIX
    if (ioctl(SHIN, TCSETAW, (ioctl_t) & rawb) < 0)
	return;
# else				/* POSIX */
    if (tcsetattr(SHIN, TCSADRAIN, &rawb) < 0)
	return;
# endif				/* POSIX */
#else				/* GSTTYB */

    rawb = xb;
    rawb.sg_flags &= ~(CBREAK);
    rawb.sg_flags |= RAW;
    if (ioctl(SHIN, TIOCSETN, (ioctl_t) & rawb) < 0)
	return;
#endif				/* TERMIO || POSIX */
    Tty_quote_mode = 1;
    return;
}

void
QuoteModeOff()
{
    if (!Tty_quote_mode)
	return;
    Tty_quote_mode = 0;
#if defined(TERMIO) || defined(POSIX)
# ifndef POSIX
    if (ioctl(SHIN, TCSETAW, (ioctl_t) & xio) < 0)
	return;
# else				/* POSIX */
    if (tcsetattr(SHIN, TCSADRAIN, &xio) < 0)
	return;
# endif				/* POSIX */
#else				/* GSTTYB */
    if (ioctl(SHIN, TIOCSETN, (ioctl_t) & xb) < 0)
	return;
#endif				/* !TERMIO && !POSIX */
    return;
}

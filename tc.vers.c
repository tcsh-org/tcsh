/* $Header$ */
/*
 * tc.vers.c: Version dependent stuff
 *
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id$";

#endif				/* !lint */

#include "sh.h"
#include "patchlevel.h"


Char   *
gethosttype()
{
    Char *hosttype;

#ifdef vax
    hosttype = SAVE("vax");
#endif				/* vax */

#ifdef hp9000			/* hp9000 running MORE/bsd */
#ifdef hp300
    hosttype = SAVE("hp300");
#endif
#ifdef hp800
    hosttype = SAVE("hp800");
#endif
#endif				/* hp9000 */

#ifdef sun
#ifdef mc68010
    hosttype = SAVE("sun2");
#endif				/* mc68010 */
#ifdef mc68020
    hosttype = SAVE("sun3");
#endif				/* mc68020 */
#ifdef sparc
    hosttype = SAVE("sun4");
#endif				/* sparc */
#ifdef i386
    hosttype = SAVE("sun386i");
#endif				/* i386 */
#endif				/* sun */

#ifdef pyr			/* pyramid */
    hosttype = SAVE("pyramid");
#endif				/* pyr */

#ifdef ibm032			/* from Jak Kirman
				 * <jak%cs.brown.edu@RELAY.CS.NET>. */
    hosttype = SAVE("rt");
#endif				/* ibm032 */

#ifdef aiws			/* not to be confused with the above */
    hosttype = SAVE("rtpc");
#endif				/* aiws */

#ifdef _AIX370
    hosttype = SAVE("aix370");
#endif				/* _AIX370 */

#ifdef _IBMR2
    hosttype = SAVE("rs6000");
#endif				/* _IBMR2 */

#ifdef _AIXPS2			/* AIX on a PS/2 */
    hosttype = SAVE("ps2");
#endif				/* _AIXPS2 */

#ifdef OREO
    hosttype = SAVE("mac2");
#endif				/* OREO */

#ifdef hpux
#ifdef hp9000s800
    hosttype = SAVE("hp9000s800");	/* maybe "spectrum" */
#else				/* hp9000s800 */
#ifdef hp9000s300
    hosttype = SAVE("hp9000s300");
#else				/* hp9000s300 */
    hosttype = SAVE("hp");
#endif				/* hp9000s300 */
#endif				/* hp9000s800 */
#endif				/* hpux */

#ifdef u3b20d
    hosttype = SAVE("att3b20");
#endif				/* u3b20d */

#ifdef u3b15
    hosttype = SAVE("att3b15");
#endif				/* u3b15 */

#ifdef u3b5
    hosttype = SAVE("att3b5");
#endif				/* u3b5 */

#ifdef u3b2
    hosttype = SAVE("att3b2");
#endif				/* u3b2 */

#if defined(i386) && SVID > 0
    hosttype = SAVE("iAPX386");
#endif

#ifdef alliant
    hosttype = SAVE("alliant");	/* for Alliant FX Series */
#endif

#if defined(i386) && defined(MACH)
    hosttype = SAVE("i386-mach");
#endif

#if defined(sequent) || defined(_SEQUENT_)
#ifdef i386
#ifdef sequent
    hosttype = SAVE("symmetry");/* Sequent Symmetry Dynix/3 */
#define LOCALSTR	" (Dynix/3)"
#else
    hosttype = SAVE("ptx");	/* Sequent Symmetry Dynix/ptx */
#define LOCALSTR	" (Dynix/ptx)"
#endif
#else
    hosttype = SAVE("balance");	/* for Sequent Balance Series */
#define LOCALSTR	" (Dynix/3)"
#endif
#else				/* !sequent */
#ifdef ns32000
#ifdef CMUCS			/* hack for Mach (in the true spirit of CMU) */
    hosttype = SAVE("multimax");
#else				/* CMUCS */
    hosttype = SAVE((!access("/Umax.image", F_OK) ? "multimax" : "ns32000"));
#endif				/* CMUCS */
#endif				/* ns32000 */
#endif				/* sequent */

#if defined(convex) || defined(__convex__)
    /* From: Brian Allison <uiucdcs!convex!allison@RUTGERS.EDU> */
    hosttype = SAVE("convex");
#endif				/* convex */

#ifdef butterfly
    /* this will work _until_ the bfly with 88000s comes out */
    hosttype = SAVE("butterfly");	/* BBN Butterfly 1000 */
#endif				/* butterfly */

#ifdef NeXT
    hosttype = SAVE("next");
#endif				/* NeXT */

/* From Kazuhiro Honda <honda@mt.cs.keio.ac.jp> */
#ifdef sony_news
#ifdef mips			/* Sony NEWS based on a r3000 */
    hosttype = SAVE("news_mips");
#else
    hosttype = SAVE("news");
#endif
#endif				/* sony_news */

#ifdef mips
#ifdef MIPSEL
#ifdef ultrix
    /* decstation XXXX */
    hosttype = SAVE("decstation");
#else
    hosttype = SAVE("mips");
#endif				/* ultrix */
#endif				/* MIPSEL */

#ifdef MIPSEB
#ifdef sgi			/* sgi iris 4d */
    hosttype = SAVE("iris4d");
#else
#ifdef sony_news
    hosttype = SAVE("news_mips");
#else
    hosttype = SAVE("mips");
#endif				/* sony_news */
#endif				/* sgi */
#endif				/* MIPSEB */
#endif				/* mips */

#ifdef m88k
    hosttype = SAVE("m88k");	/* Motorola 88100 system */
#endif

#ifdef masscomp			/* Added, DAS DEC-90. */
    hosttype = SAVE("masscomp");/* masscomp == concurrent */
#endif				/* masscomp */

#ifdef GOULD_NP1
    hosttype = SAVE("gould_np1");
#endif				/* GOULD_NP1 */

#ifdef SXA
    hosttype = SAVE("pfa50");
#ifdef  _BSDX_
#define LOCALSTR	" (SX/A E60+BSDX)"
#else
#define LOCALSTR	" (SX/A E60)"
#endif
#endif				/* PFU/Fujitsu A-xx computer */
    return hosttype;
}				/* end gethosttype */


/* fix_version():
 *	Print a reasonable version string, printing all compile time
 *	options that might affect the user.
 */
void
fix_version()
{
    char    version[BUFSIZ];

#ifdef SHORT_STRINGS
#define SSSTR "8b"
#else
#define SSSTR "7b"
#endif
#ifdef NLS
#define NLSSTR ",nls"
#else
#define NLSSTR ""
#endif
#ifdef LOGINFIRST
#define LFSTR ",lf"
#else
#define LFSTR ""
#endif
#ifdef DOTLAST
#define DLSTR ",dl"
#else
#define DLSTR ""
#endif
#ifdef VIDEFAULT
#define VISTR ",vi"
#else
#define VISTR ""
#endif
#ifdef TESLA
#define DTRSTR ",dtr"
#else
#define DTRSTR ""
#endif
#ifdef KAI
#define BYESTR ",bye"
#else
#define BYESTR ""
#endif
#ifdef AUTOLOGOUT
#define ALSTR ",al"
#else
#define ALSTR ""
#endif
#ifdef CSHDIRS
#define DIRSTR ",dir"
#else
#define DIRSTR ""
#endif
#ifdef KANJI
#define KANSTR ",kan"
#else
#define KANSTR ""
#endif
/* if you want your local version to say something */
#ifndef LOCALSTR
#define LOCALSTR ""
#endif				/* LOCALSTR */

    CSHsprintf(version,
	       "tcsh %d.%.2d.%.2d (%s) %s options %s%s%s%s%s%s%s%s%s%s%s",
	       REV, VERS, PATCHLEVEL, ORIGIN, DATE,
	       SSSTR, NLSSTR, LFSTR, DLSTR, VISTR, DTRSTR,
	       BYESTR, ALSTR, DIRSTR, KANSTR, LOCALSTR);
    set(STRversion, SAVE(version));
    CSHsprintf(version, "%d.%.2d.%.2d", REV, VERS, PATCHLEVEL);
    set(STRtcsh, SAVE(version));
}

/* $Header: /u/christos/src/tcsh-6.02/RCS/tc.vers.c,v 3.19 1992/06/16 20:46:26 christos Exp $ */
/*
 * tc.vers.c: Version dependent stuff
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
#include "sh.h"

RCSID("$Id: tc.vers.c,v 3.19 1992/06/16 20:46:26 christos Exp $")

#include "patchlevel.h"


char *
gethosttype()
{
    char *hosttype;
    
#ifdef HOSTTYPE	/* Override any system determined hosttypes */
    hosttype = HOSTTYPE;
#else

# ifdef AMIX /* Amiga UNIX */
#  define _havehosttype_
    hosttype = "amiga";
# endif /* AMIX */

# if defined(vax) || defined(__vax)
#  define _havehosttype_
    hosttype = "vax";
# endif /* vax || __vax */

# ifdef hp9000 /* hp9000 running MORE/bsd */
#  ifdef hp300
#   define _havehosttype_
    hosttype = "hp300";
#  endif 
#  ifdef hp800
#   define _havehosttype_
    hosttype = "hp800";
#  endif 
#  ifndef _havehosttype_
#   define _havehosttype_
    hosttype = "hp9000";	
#  endif 
# endif /* hp9000 */

# if defined(sun) || defined(__sun__)
#  if defined(mc68010) || defined(__mc68010__)
#   define _havehosttype_
    hosttype = "sun2";
#  endif /* mc68010 */
#  if defined(mc68020) || defined(__mc68010__)
#   define _havehosttype_
    hosttype = "sun3";
#  endif /* mc68020 */
#  if defined(sparc) || defined(__sparc__)
#   define _havehosttype_
    hosttype = "sun4";
#  endif /* sparc */
#  if defined(i386) || defined(__i386__)
#   define _havehosttype_
    hosttype = "sun386i";
#  endif /* i386 */
#  ifndef _havehosttype_
#   define _havehosttype_
    hosttype = "sun";	
#  endif 
# endif /* sun */

# ifdef pyr /* pyramid */
#  define _havehosttype_
    hosttype = "pyramid";
# endif /* pyr */

# ifdef tahoe /* tahoe */
#  define _havehosttype_
    hosttype = "tahoe";
# endif /* tahoe */

# ifdef ibm032 /* from Jak Kirman */
#  define _havehosttype_
    hosttype = "rt";
# endif /* ibm032 */

# ifdef aiws /* not to be confused with the above */
#  define _havehosttype_
    hosttype = "rtpc";
# endif /* aiws */

# ifdef _AIX370
#  define _havehosttype_
    hosttype = "aix370";
# endif /* _AIX370 */

# ifdef _IBMESA
#  define _havehosttype_
    hosttype = "aixESA";
# endif /* _IBMESA */

# ifdef _IBMR2
#  define _havehosttype_
    hosttype = "rs6000";
# endif /* _IBMR2 */

# ifdef _AIXPS2 /* AIX on a PS/2 */
#  define _havehosttype_
    hosttype = "ps2";
# endif /* _AIXPS2 */

# ifdef OREO
#  define _havehosttype_
    hosttype = "mac2";
# endif /* OREO */

# ifdef hpux
#  if defined(__hp9000s700) && !defined(_havehosttype_)
#   define _havehosttype_
   hosttype = "hp9000s700";
#  endif /* __hp9000s700 */
#  if (defined(__hp9000s800) || defined(hp9000s800)) && !defined(_havehosttype_)
#   define _havehosttype_
   hosttype = "hp9000s800";	/* maybe "spectrum" */
#  endif /* __hp9000s800 || hp9000s800 */
#  if (defined(__hp9000s300) || defined(hp9000s300)) && !defined(_havehosttype_)
#   define _havehosttype_
   hosttype = "hp9000s300";
#  endif /* __hp9000s800 || hp9000s300 */
# if defined(hp9000s500) && !defined(_havehosttype_)
#  define _havehosttype_
   hosttype = "hp9000s500";
# endif /* hp9000s500 */
#  ifndef _havehosttype_
#   define _havehosttype_
   hosttype = "hp";
#  endif /* _havehosttype_ */
# endif /* hpux */

# ifdef apollo
#  define _havehosttype_
    hosttype = "apollo";
# endif 

# ifdef u3b20d
#  define _havehosttype_
    hosttype = "att3b20";
# endif /* u3b20d */

# ifdef u3b15
#  define _havehosttype_
    hosttype = "att3b15";
# endif /* u3b15 */

# ifdef u3b5
#  define _havehosttype_
    hosttype = "att3b5";
# endif /* u3b5 */

# ifdef u3b2
#  define _havehosttype_
    hosttype = "att3b2";
# endif /* u3b2 */

#ifdef _MINIX
# define _havehosttype_
# ifdef i386
    hosttype = "minix386";
# else /* minix ? amoeba or mac? */
    hosttype = "minix";
# endif /* i386 */
#endif /* _MINIX */

# if defined(i386) && SYSVREL > 0

#  if !defined(_havehosttype_) && (defined(ISC) || defined(ISC202))
#   define _havehosttype_
    hosttype = "isc386";
#  endif /* !_havehosttype_ && (ISC || ISC202) */

#  if !defined(_havehosttype_) && defined(SCO)
#   define _havehosttype_
    hosttype = "sco386";
#  endif /* !_havehosttype_ && SCO */

#  if !defined(_havehosttype_) && defined(INTEL)
#   define _havehosttype_
    hosttype = "intel386";
#  endif /* !_havehosttype_ && INTEL */

#  ifndef _havehosttype_
#   define _havehosttype_
    hosttype = "i386";
#  endif /* _havehosttype_ */

# endif 

#ifdef UNIXPC
# define _havehosttype_
    hosttype = "unixpc";
#endif /* UNIXPC/att3b1/att7300 */

# ifdef alliant
#  define _havehosttype_
    hosttype = "alliant";	/* for Alliant FX Series */
# endif 

# if defined(i386) && defined(MACH)
#  define _havehosttype_
    hosttype = "i386-mach";
# endif 

# if defined(sequent) || defined(_SEQUENT_)
#  define _havehosttype_
#  ifdef i386
#   ifdef sequent
    hosttype = "symmetry";	/* Sequent Symmetry Dynix/3 */
#    ifndef LOCALSTR
#     define LOCALSTR	" (Dynix/3)"
#    endif /* LOCALSTR */
#   else
    hosttype = "ptx";	/* Sequent Symmetry Dynix/ptx */
#    ifndef LOCALSTR
#     define LOCALSTR	" (Dynix/ptx)"
#    endif /* LOCALSTR */
#   endif 
#  else
    hosttype = "balance";	/* for Sequent Balance Series */
#   ifndef LOCALSTR
#    define LOCALSTR	" (Dynix/3)"
#   endif /* LOCALSTR */
#  endif 
# else /* !sequent */
#  ifdef ns32000
#   define _havehosttype_
#   ifdef CMUCS			/* hack for Mach (in the true spirit of CMU) */
    hosttype = "multimax";
#   else /* CMUCS */
    hosttype = (!access("/Umax.image", F_OK) ? "multimax" : "ns32000";
#   endif /* CMUCS */
#  endif /* ns32000 */
# endif /* sequent */

# if defined(convex) || defined(__convex__)
#  define _havehosttype_
    /* From: Brian Allison <uiucdcs!convex!allison@RUTGERS.EDU> */
    hosttype = "convex";
# endif /* convex */

# ifdef butterfly
#  define _havehosttype_
    /* this will work _until_ the bfly with 88000s comes out */
    hosttype = "butterfly";	/* BBN Butterfly 1000 */
# endif /* butterfly */

# ifdef NeXT
#  define _havehosttype_
    hosttype = "next";
# endif /* NeXT */

/* From Kazuhiro Honda <honda@mt.cs.keio.ac.jp> */
# ifdef sony_news
#  define _havehosttype_
#  ifdef mips /* Sony NEWS based on a r3000 */
    hosttype = "news_mips";
#  else
    hosttype = "news";
#  endif 
# endif /* sony_news */

# if defined(mips) || defined(__mips)
#  define _havehosttype_
#  if defined(MIPSEL) || defined(__MIPSEL)
#   if defined(ultrix) || defined(__ultrix)
    hosttype = "decstation";
#   else
    hosttype = "mips";
#   endif /* ultrix || __ultrix */
#  endif /* MIPSEL || __MIPSEL */
#  if defined(MIPSEB) || defined(__MIPSEB)
#   if defined(ultrix) || defined(__ultrix)
    hosttype = "decmips";
#   else
#    ifdef sgi /* sgi iris 4d */
    hosttype = "iris4d";
#    else
#     ifdef sony_news
    hosttype = "news_mips";
#     else
    hosttype = "mips";
#     endif /* sony_news */
#    endif /* sgi */
#   endif /* ultrix || __ultrix */
#  endif /* MIPSEB || __MIPSEB */
# endif /* mips || __mips */

#if defined(__alpha)
#  define _havehosttype_
    hosttype = "alpha";
#endif

# if defined(m88k) || defined(__m88k__)
#  define _havehosttype_
    hosttype = "m88k";	/* Motorola 88100 system */
# endif 

# ifdef masscomp			/* Added, DAS DEC-90. */
#  define _havehosttype_
    hosttype = "masscomp";/* masscomp == concurrent */
# endif /* masscomp */

# ifdef GOULD_NP1
#  define _havehosttype_
    hosttype = "gould_np1";
# endif /* GOULD_NP1 */

# ifdef SXA
#  define _havehosttype_
    hosttype = "pfa50";
#  ifdef  _BSDX_
#   ifndef LOCALSTR
#    define LOCALSTR	" (SX/A E60+BSDX)"
#   endif /* LOCALSTR */
#  else
#   ifndef LOCALSTR
#    define LOCALSTR	" (SX/A E60)"
#   endif /* LOCALSTR */
#  endif 
# endif /* PFU/Fujitsu A-xx computer */

# ifdef titan
#  define _havehosttype_
    /* Ken Laprade <laprade@trantor.harris-atd.com> */
    hosttype = "titan";
# endif /* titan */

# ifdef stellar
#  define _havehosttype_
    hosttype = "stellar";
# endif /* stellar */

# ifdef sgi
/* Iris 4D is in the mips section; these are the 68k machines. */
#  ifdef m68000
#   define _havehosttype_
    /* Vince Del Vecchio <vd09@andrew.cmu.edu> */
    hosttype = "iris3d";
#  endif
# endif /* sgi */

# ifdef uts
#  define _havehosttype_
    hosttype = "amdahl";
# endif /* uts */
  
# ifdef OPUS
#  define _havehosttype_
    hosttype = "opus";
# endif /* OPUS */

# ifdef eta10
#  define _havehosttype_
   /* Bruce Woodcock <woodcock@mentor.cc.purdue.edu> */
   hosttype = "eta10";
# endif /* eta10 */

# ifdef cray
#  define _havehosttype_
   hosttype = "cray";
# endif /* cray */

# ifdef NDIX
#  define _havehosttype_
   /* B|rje Josefsson <bj@dc.luth.se> */
   hosttype = "nd500";
# endif /* NDIX */

# if defined(sysV68)
#  define _havehosttype_
    hosttype = "sysV68";
# endif /* sysV68 */

# if defined(i860) && !defined(_havehosttype_)
#  define _havehosttype_
   /* Tasos Kotsikonas <tasos@avs.com> */
   hosttype = "vistra800"; /* Stardent Vistra */
# endif /* i860  && !_havehosttype_ */

# ifndef _havehosttype_
#  if defined(mc68000) || defined(__mc68000__) || defined(mc68k32)
#   define _havehosttype_
     hosttype = "m68k";	/* Motorola 68000 system */
#  endif 
# endif

# ifndef _havehosttype_
#  define _havehosttype_
    /* Default to something reasonable */
    hosttype = "unknown";
# endif 
# undef _havehosttype_
#endif /* HOSTTYPE */
    return hosttype;
} /* end gethosttype */


/* fix_version():
 *	Print a reasonable version string, printing all compile time
 *	options that might affect the user.
 */
void
fix_version()
{
    char    version[BUFSIZE];

#ifdef SHORT_STRINGS
# define SSSTR "8b"
#else
# define SSSTR "7b"
#endif 
#ifdef NLS
# define NLSSTR ",nls"
#else
# define NLSSTR ""
#endif 
#ifdef LOGINFIRST
# define LFSTR ",lf"
#else
# define LFSTR ""
#endif 
#ifdef DOTLAST
# define DLSTR ",dl"
#else
# define DLSTR ""
#endif 
#ifdef VIDEFAULT
# define VISTR ",vi"
#else
# define VISTR ""
#endif 
#ifdef TESLA
# define DTRSTR ",dtr"
#else
# define DTRSTR ""
#endif 
#ifdef KAI
# define BYESTR ",bye"
#else
# define BYESTR ""
#endif 
#ifdef AUTOLOGOUT
# define ALSTR ",al"
#else
# define ALSTR ""
#endif 
#ifdef KANJI
# define KANSTR ",kan"
#else
# define KANSTR ""
#endif 
#ifdef SYSMALLOC
# define SMSTR	",sm"
#else
# define SMSTR  ""
#endif 
/* if you want your local version to say something */
#ifndef LOCALSTR
# define LOCALSTR ""
#endif /* LOCALSTR */

    xsprintf(version,
	     "tcsh %d.%.2d.%.2d (%s) %s (%s) options %s%s%s%s%s%s%s%s%s%s%s",
	     REV, VERS, PATCHLEVEL, ORIGIN, DATE, gethosttype(),
	     SSSTR, NLSSTR, LFSTR, DLSTR, VISTR, DTRSTR,
	     BYESTR, ALSTR, KANSTR, SMSTR, LOCALSTR);
    set(STRversion, SAVE(version));
    xsprintf(version, "%d.%.2d.%.2d", REV, VERS, PATCHLEVEL);
    set(STRtcsh, SAVE(version));
}

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/sh.hist.c,v 2.0 1991/03/26 02:59:29 christos Exp $ */
/*
 * sh.hist.c: Shell history expansions and substitutions
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "config.h"

#ifdef notdef
static char *sccsid = "@(#)sh.hist.c	5.2 (Berkeley) 6/6/85";
#endif
#ifndef lint
static char *rcsid = "$Id: sh.hist.c,v 2.0 1991/03/26 02:59:29 christos Exp $";
#endif

#include "sh.h"

extern bool histvalid;
extern Char histline[];
static Char HistLit = 0;

static void hfree();
static void dohist1();
static void phist();

/*
 * C shell
 */

void
savehist(sp)
	struct wordent *sp;
{
	register struct Hist *hp, *np;
	register int histlen = 0;
	Char *cp;

	/* throw away null lines */
	if (sp->next->word[0] == '\n')
		return;
	cp = value(STRhistory);
	if (*cp) {
		register Char *p = cp;

		while (*p) {
			if (!isdigit(*p)) {
				histlen = 0;
				break;
			}
			histlen = histlen * 10 + *p++ - '0';
		}
	}
	for (hp = &Histlist; np = hp->Hnext;)
		if (eventno - np->Href >= histlen || histlen == 0)
			hp->Hnext = np->Hnext, hfree(np);
		else
			hp = np;
	(void) enthist(++eventno, sp, 1);
}

struct Hist *
enthist(event, lp, docopy)
	int event;
	register struct wordent *lp;
	bool docopy;
{
	register struct Hist *np;

	np = (struct Hist *) xalloc((size_t) sizeof(*np));
	(void) time(&(np->Htime));
	np->Hnum = np->Href = event;
	if (docopy) {
		copylex(&np->Hlex, lp);
		if (histvalid)
			np->histline = Strsave(histline);
		else
			np->histline = NULL;
	} else {
		np->Hlex.next = lp->next;
		lp->next->prev = &np->Hlex;
		np->Hlex.prev = lp->prev;
		lp->prev->next = &np->Hlex;
		np->histline = NULL;
	}
	np->Hnext = Histlist.Hnext;
	Histlist.Hnext = np;
	return (np);
}

static void
hfree(hp)
	register struct Hist *hp;
{

	freelex(&hp->Hlex);
	if (hp->histline)
		xfree((ptr_t)hp->histline);
	xfree((ptr_t) hp);
}

void
dohist(vp)
	Char **vp;
{
      int n, rflg = 0, hflg = 0, tflg = 0;
	if (getn(value(STRhistory)) == 0)
		return;
	if (setintr)
#ifdef BSDSIGS
		(void) sigsetmask(sigblock(0) & ~sigmask(SIGINT));
#else
		sigrelse(SIGINT);
#endif
 	while (*++vp && **vp == '-') {
 		Char *vp2 = *vp;
 
 		while (*++vp2)
 			switch (*vp2) {
 			case 'h':
 				hflg++;
 				break;
 			case 'r':
 				rflg++;
 				break;
			case 't':
				tflg++;
				break;
 			case '-':	/* ignore multiple '-'s */
 				break;
 			default:
 				CSHprintf("Unknown flag: -%c\n", *vp2);
				error(
				"Usage: history [-rht] [# number of events]");
				break;
			}
	}
	if (*vp)
		n = getn(*vp);
	else {
		n = getn(value(STRhistory));
	}
	dohist1(Histlist.Hnext, &n, rflg, hflg, tflg);
}

static void
dohist1(hp, np, rflg, hflg, tflg)
	struct Hist *hp;
      int *np, rflg, hflg, tflg;
{
	bool print = (*np) > 0;

	for (; hp != 0; hp = hp->Hnext) {
		(*np)--;
		hp->Href++;
		if (rflg == 0) {
		        dohist1(hp->Hnext, np, rflg, hflg, tflg);
			if (print)
				phist(hp, hflg, tflg);
			return;
		}
		if (*np >= 0)
              		phist(hp, hflg, tflg);
	}
}

static void
phist(hp, hflg, tflg)
	register struct Hist *hp;
      int hflg, tflg;
{
	struct tm *t;
	char ampm = 'a';

	if (hflg == 0)
	{
		CSHprintf("%6d\t", hp->Hnum);
              if (tflg == 0) {
                      t = localtime(&hp->Htime);
                      if ( adrof(STRampm) ) { /* addition by Hans J. Albertsson */
                          if (t->tm_hour >= 12)
                              {
                                  if (t->tm_hour > 12)
                                      t->tm_hour -= 12;
                                  ampm = 'p';
                              }
                          else if (t->tm_hour == 0)
                              t->tm_hour = 12;
                          CSHprintf ("%2d:%02d%cm\t", t->tm_hour, t->tm_min, ampm);
                      } else {
                          CSHprintf ("%2d:%02d\t", t->tm_hour, t->tm_min);
                  }
		}
	}
	if (HistLit && hp->histline)
		CSHprintf("%s\n",short2str(hp->histline));
	else
		prlex(&hp->Hlex);
}

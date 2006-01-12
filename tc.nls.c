/* $Header: /src/pub/tcsh/tc.nls.c,v 3.16 2006/01/12 18:15:25 christos Exp $ */
/*
 * tc.nls.c: NLS handling
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
 * 3. Neither the name of the University nor the names of its contributors
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

RCSID("$Id: tc.nls.c,v 3.16 2006/01/12 18:15:25 christos Exp $")

#if defined(SHORT_STRINGS) && defined(NLS)
int
NLSWidth(NLSChar c)
{
# ifdef HAVE_WCWIDTH
    int l; 
    if (c & NLS_ILLEGAL)
	return 1;
    l = wcwidth(c);
    return l >= 0 ? l : 0;
# else
    return Iswprint(c) != 0;
# endif
}
#endif

#if defined(WIDE_STRINGS) || (defined(SHORT_STRINGS) && !defined(NLS))
Char *
NLSChangeCase(const Char *p, int mode)
{
    Char c, *n, c2 = 0;
    const Char *op = p;

    for (; (c = *p) != 0; p++) {
        if (mode == 0 && Islower(c)) {
	    c2 = Toupper(c);
	    break;
        } else if (mode && Isupper(c)) {
	    c2 = Tolower(c);
	    break;
	}
    }
    if (!*p)
	return 0;
    n = Strsave(op);
    n[p - op] = c2;
    return n;
}

int
NLSExtend(const Char *from, int max, int num)
{
    (void)from;
    num = abs (num);
    if (num > max)
	num = max;
    return num;
}
#endif

#ifdef WIDE_STRINGS

int
NLSStringWidth(const Char *s)
{
    int w = 0, c, l;
    while (*s) {
	c = *s++;
#ifdef HAVE_WCWIDTH
	if ((l = wcwidth(c)) < 0)
		l = 2;
#else
	l = Iswprint(c) != 0;
#endif
	w += l;
    }
    return w;
}

#elif defined (SHORT_STRINGS) && defined(NLS)

int
NLSFrom(const Char *p, size_t l, NLSChar *cp)
{
    size_t i;
    int len;
    wchar_t c;
    char b[MB_LEN_MAX];

    if (l == NLSZEROT) {
        for (i = 0; i < MB_CUR_MAX && *p; i++)
	    b[i] = p[i] & CHAR;
    } else {
        for (i = 0; i < MB_CUR_MAX && i < l; i++)
	    b[i] = p[i] & CHAR;
    }
    mbtowc(0, 0, 0);
    len = rt_mbtowc(&c, b, i);
    if (len <= 0) {
	if (cp)
	  *cp = *p ? *p | NLS_ILLEGAL : 0;
        return 1;
    }
    if (cp) 
        *cp = (int)c; 
    return len;
}

int
NLSFinished(const Char *p, size_t l, eChar extra)
{
#ifdef HAVE_MBRTOWC
    size_t i, r; 
    wchar_t c;
    char b[MB_LEN_MAX + 1], back[MB_LEN_MAX];
    mbstate_t state;
    for (i = 0; i < MB_CUR_MAX && i < l; i++)
	b[i] = p[i]; 
    if (extra != CHAR_ERR)
        b[i++] = extra;
    memset(&state, 0, sizeof(state));
    r = mbrtowc(&c, b, i, &state);
    if (r == (size_t)-2)
	return 0;
    if (r == (size_t)-1 || (size_t)wctomb(back, c) != r ||
	memcmp(b, back, r) != 0)
	return -1;
    return r == i ? 1 : 2;
#else
    return *p ? 2 : 1;
#endif
}

int
NLSChars(const Char *s)
{
    int l;
    for (l = 0; *s; l++)
        s += NLSSize(s, NLSZEROT);
    return l;
}

int
NLSStringWidth(const Char *s)
{
    int w = 0;
    NLSChar c;
    while (*s) {
        s += NLSFrom(s, NLSZEROT, &c);
	w += NLSWidth(c);
    }
    return w;
}

int
NLSTo(Char *p, NLSChar c)
{
    char b[MB_LEN_MAX];
    int i, j;

    if (c & NLS_ILLEGAL) {
        if (p)
	    *p = c;
	return 1;
    }
    i = wctomb(b, (wchar_t)c);
    if (i == -1)
        return 0;
    if (p)
        for (j = 0; j < i; j++)
            p[j] = b[j];
    return i;
}


int
NLSExtend(const Char *from, int max, int num)
{
    int l, n, i;
    Char *p;

    if (num == 0)
	return 0;
    if (num > 0) {
	n = 0;
	while (num > 0 && max > 0) {
	    l = NLSSize(from, max);
	    n += l;
	    from += l;
	    max -= l;
	    num--;
	}
	return n;
    }
    from -= max;
    p = from;
    i = max;
    n = 0;
    while (i > 0) {
	l = NLSSize(p, i);
	p += l;
        i -= l;
	n++;
    }
    if (n >= -num)
	n += num;
    else
	n = 0;
    i = max;
    while (n > 0) {
	l = NLSSize(from, max);
	from += l;
	max -= l;
	i -= l;
	n--;
    }
    return i;
}

void
NLSQuote(Char *cp)
{
    int l;
    while (*cp) {
	l = NLSSize(cp, NLSZEROT);
	cp++;
	while (l-- > 1)
	    *cp++ |= QUOTE;
    }
}

Char *
NLSChangeCase(const Char *p, int mode)
{
#ifdef HAVE_WINT_T
    Char *n;
    const Char *op = p;
    NLSChar c, c2 = 0;
    size_t l;
    int l2;

    while (*p) {
	l = NLSFrom(p, NLSZEROT, &c);
	if (mode == 0 && iswlower((wint_t)c)) {
	    c2 = (int)towupper((wint_t)c);
	    break;
	} else if (mode && iswupper((wint_t)c)) {
	    c2 = (int)towlower((wint_t)c);
	    break;
	}
	p += l;
    }
    if (!*p)
	return 0;
    l2 = NLSTo((Char *)0, c2);
    n = xmalloc(((p - op + l2 + Strlen(p + l) + 1) * sizeof(Char)));
    memcpy(n, op, (p - op) * sizeof(Char));
    NLSTo(n + (p - op), c2);
    Strcpy(n + (p - op + l2), p + l);
    return n;
#else
    Char *n = Strsave(p);

    if (mode == 0) {
	for (p = n; *p; p++) 
	    if (Islower(*p)) {
		*p = Toupper(*p);
		return n;
	    }
    } else {
	for (p = n; *p; p++) 
	    if (Isupper(*p)) {
		*p = Tolower(*p);
		return n;
	    }
    }
    return n;
#endif
}
#endif

int
NLSClassify(NLSChar c, int nocomb)
{
    int w;
    if (c & NLS_ILLEGAL)
	return NLSCLASS_ILLEGAL;
    w = NLSWidth(c);
    if (w > 0 || (Iswprint(c) && !nocomb))
	return w;
    if (Iswcntrl(c) && c < 0x100) {
	if (c == '\n')
	    return NLSCLASS_NL;
	if (c == '\t')
	    return NLSCLASS_TAB;
#ifndef ASCII
	if (!Isupper(_toebcdic[_toascii[c]|0100]) && !strchr("@[\\]^_", _toebcdic[_toascii[c]|0100]))
	    return NLSCLASS_ILLEGAL;
#endif
	return NLSCLASS_CTRL;
    }
    if (c >= 0x1000000)
	return NLSCLASS_ILLEGAL4;
    if (c >= 0x10000)
	return NLSCLASS_ILLEGAL3;
    if (c >= 0x100)
	return NLSCLASS_ILLEGAL2;
    return NLSCLASS_ILLEGAL;
}

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-6.00/RCS/ed.xmap.c,v 3.0 1991/07/04 21:49:28 christos Exp $ */
/*
 * ed.xmap.c: This module contains the procedures for maintaining
 *	      the extended-key map.
 *
 * 	      An extended-key (Xkey) is a sequence of keystrokes
 *	      introduced with an sequence introducer and consisting
 *	      of an arbitrary number of characters.  This module maintains
 *	      a map (the Xmap) to convert these extended-key sequences
 * 	      into input strings or editor functions. It contains the
 *	      following externally visible functions.
 *
 *		int GetXkey(ch,code);
 *		Char *ch;
 *		Char **code;
 *
 *	      Looks up *ch in map and then reads characters until a
 *	      complete match is found or a mismatch occurs.  Returns 1
 *	      for command and 0 for a string. Returns NULL in code for
 *	      no match and 0 as value.  The last character read is returned
 *	      in *ch.
 *
 *		void AddXkey(Xkey, code);
 *		Char *Xkey;
 *		Char * code;
 *
 *		void AddXkeyCmd(Xkey, CmdCode);
 *		Char *Xkey;
 *		Char CmdCode;
 *
 *	      Adds Xkey to the Xmap and associates the code with it.  If
 *	      Xkey is already is in Xmap, the new code is applied to the
 *	      existing Xkey.
 *
 *	        int DeleteXkey(Xkey);
 *	        Char *Xkey;
 *
 *	      Delete the Xkey and all longer Xkeys staring with Xkey, if
 *	      they exists.
 *
 *	      Warning:
 *		If Xkey is a substring of some other Xkeys, then the longer
 *		Xkeys are lost!!  That is, if the Xkeys "abcd" and "abcef"
 *		are in Xmap, adding the key "abc" will cause the first two
 *		definitions to be lost.
 *
 *		void ResetXmap();
 *
 *	      Removes all entries from Xmap and resets the defaults.
 *
 *		void PrintXkey(Xkey);
 *		Char *Xkey;
 *
 *	      Prints all extended keys prefixed by Xkey and their associated
 *	      commands.
 *
 *	      Restrictions:
 *	      -------------
 *	        1) It is not possible to have one Xkey that is a
 *		   substring of another.
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
RCSID("$Id$")

#include "sh.h"
#include "ed.h"
#include "ed.defns.h"

#ifndef NULL
#define NULL 0
#endif

/* Internal Data types and declarations */

/* The Nodes of the Xmap.  The Xmap is a linked list of these node
 * elements
 */
typedef struct Xmapnode {
    Char    ch;			/* single character of Xkey */
    Char   *code;		/* command code or pointer to string, if this
				 * is a leaf */
    struct Xmapnode *next;	/* ptr to next char of this Xkey */
    struct Xmapnode *sibling;	/* ptr to another Xkey with same prefix */
}       XmapNode;

static XmapNode *Xmap = NULL;	/* the current Xmap */

static Char CurCode;
static XmapNode CurCmd = {0, &CurCode, NULL, NULL};

/* Some declarations of procedures */
static	int       	 TraverseMap	__P((XmapNode *, Char *, Char **));
static	int       	 TryNode	__P((XmapNode *, Char *, Char *, int));
static	XmapNode	*GetFreeNode	__P((int));
static	void		 PutFreeNode	__P((XmapNode *));
static	int		 TryDeleteNode	__P((XmapNode **, Char *));


/* ResetXmap():
 *	Takes all nodes on Xmap and puts them on free list.  Then
 *	initializes Xmap with arrow keys
 */
void
ResetXmap(vi)
    int     vi;
{
    static Char strA[] = {033, '[', 'A', '\0'};
    static Char strB[] = {033, '[', 'B', '\0'};
    static Char strC[] = {033, '[', 'C', '\0'};
    static Char strD[] = {033, '[', 'D', '\0'};
    static Char stOA[] = {033, 'O', 'A', '\0'};
    static Char stOB[] = {033, 'O', 'B', '\0'};
    static Char stOC[] = {033, 'O', 'C', '\0'};
    static Char stOD[] = {033, 'O', 'D', '\0'};

    PutFreeNode(Xmap);
    Xmap = NULL;
    AddXkeyCmd(strA, F_UP_HIST);
    AddXkeyCmd(strB, F_DOWN_HIST);
    AddXkeyCmd(strC, F_CHARFWD);
    AddXkeyCmd(strD, F_CHARBACK);
    AddXkeyCmd(stOA, F_UP_HIST);
    AddXkeyCmd(stOB, F_DOWN_HIST);
    AddXkeyCmd(stOC, F_CHARFWD);
    AddXkeyCmd(stOD, F_CHARBACK);
    if (vi) {
	AddXkeyCmd(&strA[1], F_UP_HIST);
	AddXkeyCmd(&strB[1], F_DOWN_HIST);
	AddXkeyCmd(&strC[1], F_CHARFWD);
	AddXkeyCmd(&strD[1], F_CHARBACK);
	AddXkeyCmd(&stOA[1], F_UP_HIST);
	AddXkeyCmd(&stOB[1], F_DOWN_HIST);
	AddXkeyCmd(&stOC[1], F_CHARFWD);
	AddXkeyCmd(&stOD[1], F_CHARBACK);
    }
    return;
}


/* GetXkey():
 *	Calls the recursive function with entry point Xmap
 */
int
GetXkey(ch, code)
    Char   *ch;
    Char  **code;
{
    return (TraverseMap(Xmap, ch, code));
}

/* TraverseMap():
 *	recursively traverses node in tree until match or mismatch is
 * 	found.  May read in more characters.
 */
static int
TraverseMap(ptr, ch, code)
    XmapNode *ptr;
    Char   *ch;
    Char  **code;
{
    Char    tch;

    if (ptr->ch == *ch) {
	/* match found */
	if (ptr->next) {
	    if (ptr->next != &CurCmd) {
		/* Xkey not complete so get next char */
		if (GetNextChar(&tch) != 1) {	/* if EOF or error */
		    *ch = 0;
		    CurCmd.code[0] = F_SEND_EOF;
		    *code = CurCmd.code;
		    return 1;	/* PWP: Pretend we just read an end-of-file */
		}
		*ch = tch;
		return (TraverseMap(ptr->next, ch, code));
	    }
	    else {
		CurCmd.code[0] = (Char) ptr->code;
		*code = CurCmd.code;
		return 1;
	    }
	}
	else {
	    /* next is null so this is leaf node and a string */
	    *ch = 0;
	    *code = ptr->code;
	    return 0;
	}
    }
    else {
	/* no match found here */
	if (ptr->sibling) {
	    /* try next sibling */
	    return (TraverseMap(ptr->sibling, ch, code));
	}
	else {
	    /* no next sibling -- mismatch */
	    *code = NULL;
	    return 0;
	}
    }
}

void
AddXkey(Xkey, code)
    Char   *Xkey;
    Char   *code;
{
    if (Xkey[0] == '\0') {
	xprintf("AddXkey: Null extended-key not allowed.\n");
	return;
    }

    if (Xmap == NULL)
	/* tree is initially empty.  Set up new node to match Xkey[0] */
	Xmap = GetFreeNode(Xkey[0]);	/* it is properly initialized */

    /* Now recurse through Xmap */
    (void) TryNode(Xmap, Xkey, code, 1);	/* string */
    return;
}

void
AddXkeyCmd(Xkey, CmdCode)
    Char   *Xkey;
    int    CmdCode;
{
    /* Gould does not like casts... */
    unsigned int compiler_bug;

    if (Xkey[0] == '\0') {
	xprintf("AddXkeyCmd: Null extended-key not allowed.\n");
	return;
    }
    if (CmdCode == F_XKEY) {
	xprintf("AddXkeyCmd: sequence-lead-in command not allowed\n");
	return;
    }

    if (Xmap == NULL)
	/* tree is initially empty.  Set up new node to match Xkey[0] */
	Xmap = GetFreeNode(Xkey[0]);	/* it is properly initialized */

    /* Now recurse through Xmap */
    compiler_bug = (unsigned int) CmdCode;
    (void) TryNode(Xmap, Xkey, (Char *) compiler_bug, 0);	/* command */
    return;
}


static int
TryNode(ptr, string, code, IsString)
    XmapNode *ptr;
    Char   *string;
    Char   *code;
    int     IsString;
{
    /*
     * Find a node that matches *string or allocate a new one
     */
    if (ptr->ch != *string) {
	XmapNode *xm;

	for (xm = ptr; xm->sibling != NULL; xm = xm->sibling)
	    if (xm->sibling->ch == *string)
		break;
	if (xm->sibling == NULL)
	    xm->sibling = GetFreeNode(*string);	/* setup new node */
	ptr = xm->sibling;
    }

    if (*++string == '\0') {
	/* we're there */
	if (ptr->next != NULL && ptr->next != &CurCmd) {
	    PutFreeNode(ptr->next);	/* lose longer Xkeys with this prefix */
	    ptr->next = NULL;
	}
	if (ptr->next == NULL && ptr->code)
	    xfree((ptr_t) ptr->code);
	if (IsString) {
	    ptr->next = NULL;
	    ptr->code = Strsave(code);
	}
	else {
	    ptr->next = &CurCmd;
	    ptr->code = code;
	}
    }
    else {
	/* still more chars to go */
	/*
	 * christos: We need to allocate a new XmapNode also if the next
	 * XmapNode is the CurCmd, cause the previous XmapNode definition was
	 * only one char long!
	 */
	if (ptr->next == NULL || ptr->next == &CurCmd)
	    ptr->next = GetFreeNode(*string);	/* setup new node */
	(void) TryNode(ptr->next, string, code, IsString);
    }
    return (0);
}

void
ClearXkey(map, in)
    KEYCMD *map;
    Char   *in;
{
    if ((map[(unsigned char) *in] == F_XKEY) &&
	((map == CcKeyMap && CcAltMap[(unsigned char) *in] != F_XKEY) ||
	 (map == CcAltMap && CcKeyMap[(unsigned char) *in] != F_XKEY)))
	(void) DeleteXkey(in);
}

int
DeleteXkey(Xkey)
    Char   *Xkey;
{
    if (Xkey[0] == '\0') {
	xprintf("DeleteXkey: Null extended-key not allowed.\n");
	return (-1);
    }

    if (Xmap == NULL)
	return (0);

    (void) TryDeleteNode(&Xmap, Xkey);
    return (0);
}

static int
TryDeleteNode(inptr, string)
    XmapNode **inptr;
    Char   *string;
{
    XmapNode *ptr;
    XmapNode *prev_ptr = NULL;

    ptr = *inptr;
    /*
     * Find a node that matches *string or allocate a new one
     */
    if (ptr->ch != *string) {
	XmapNode *xm;

	for (xm = ptr; xm->sibling != NULL; xm = xm->sibling)
	    if (xm->sibling->ch == *string)
		break;
	if (xm->sibling == NULL)
	    return (0);
	prev_ptr = xm;
	ptr = xm->sibling;
    }

    if (*++string == '\0') {
	/* we're there */
	if (prev_ptr == NULL)
	    *inptr = ptr->sibling;
	else
	    prev_ptr->sibling = ptr->sibling;
	ptr->sibling = NULL;
	PutFreeNode(ptr);
	return (1);
    }
    else if (ptr->next != NULL && TryDeleteNode(&ptr->next, string) == 1) {
	if (ptr->next != NULL)
	    return (0);
	if (prev_ptr == NULL)
	    *inptr = ptr->sibling;
	else
	    prev_ptr->sibling = ptr->sibling;
	ptr->sibling = NULL;
	PutFreeNode(ptr);
	return (1);
    }
    else {
	return (0);
    }
}





/* PutFreeNode():
 *	Puts a tree of nodes onto free list using free(3).
 */
static void
PutFreeNode(ptr)
    XmapNode *ptr;
{
    if (ptr == NULL)
	return;

    if (ptr->next && ptr->next != &CurCmd)
	PutFreeNode(ptr->next);
    PutFreeNode(ptr->sibling);
    if (ptr->next == NULL && ptr->code)
	xfree((ptr_t) ptr->code);
    xfree((ptr_t) ptr);
}


/* GetFreeNode():
 *	Returns pointer to an XmapNode for ch.
 */
static XmapNode *
GetFreeNode(ch)
    int    ch;
{
    XmapNode *ptr;

    ptr = (XmapNode *) xmalloc((size_t) sizeof(XmapNode));
    ptr->ch = ch;
    ptr->code = NULL;
    ptr->next = NULL;
    ptr->sibling = NULL;
    return (ptr);
}

/* Now The Print Routine */
#define maxXkey 100		/* max length of a Xkey for print putposes */
static Char printbuf[maxXkey];	/* buffer for printing */

static int Lookup();
static int Enumerate();
static int printOne();
static int unparsech();
extern unsigned char *unparsestring();

/* PrintXKey():
 *	Print the binding associated with Xkey key.
 *	Print entire Xmap if null
 */
void
PrintXkey(key)
    Char   *key;
{
    /* do nothing if Xmap is empty and null key specified */
    if (Xmap == NULL && *key == 0)
	return;

    printbuf[0] = '"';
    if (Lookup(key, Xmap, 1) <= -1)
	/* key is not bound */
	xprintf("Unbound extended key \"%s\"\n", short2str(key));
    return;
}

/* Lookup():
 *	look for the string starting at node ptr.
 *	Print if last node
 */
static int
Lookup(string, ptr, cnt)
    int     cnt;
    Char   *string;
    XmapNode *ptr;
{
    int     ncnt;

    if (ptr == NULL)
	return (-1);		/* cannot have null ptr */

    if (*string == 0) {
	/* no more chars in string.  Enumerate from here. */
	(void) Enumerate(ptr, cnt);
	return (0);
    }
    else {
	/* If match put this char into printbuf.  Recurse */
	if (ptr->ch == *string) {
	    /* match found */
	    ncnt = unparsech(cnt, ptr->ch);
	    if (ptr->next && ptr->next != &CurCmd)
		/* not yet at leaf */
		return (Lookup(string + 1, ptr->next, ncnt + 1));
	    else {
		/* next node is null  or &CurCmd so key should be complete */
		if (string[1] == 0) {
		    printbuf[ncnt + 1] = '"';
		    printbuf[ncnt + 2] = '\0';
		    (void) printOne(printbuf, ptr->code, ptr->next == NULL);
		    return (0);
		}
		else
		    return (-1);/* mismatch -- string still has chars */
	    }
	}
	else {
	    /* no match found try sibling */
	    if (ptr->sibling)
		return (Lookup(string, ptr->sibling, cnt));
	    else
		return (-1);
	}
    }
}

static int
Enumerate(ptr, cnt)
    int     cnt;
    XmapNode *ptr;
{
    int     ncnt;

    if (cnt >= maxXkey - 5) {	/* buffer too small */
	printbuf[++cnt] = '"';
	printbuf[++cnt] = '\0';
	xprintf("Some extended keys too long for internal print buffer");
	xprintf(" \"%s...\"\n", short2str(printbuf));
	return (0);
    }

    if (ptr == NULL) {
#ifdef DEBUG_EDIT
	xprintf("Enumerate: BUG!! Null ptr passed\n!");
#endif
	return (-1);
    }

    ncnt = unparsech(cnt, ptr->ch);	/* put this char at end of string */
    if (ptr->next == NULL || ptr->next == &CurCmd) {
	/* print this Xkey and function */
	printbuf[ncnt + 1] = '"';
	printbuf[ncnt + 2] = '\0';
	(void) printOne(printbuf, ptr->code, ptr->next == NULL);
    }
    else
	(void) Enumerate(ptr->next, ncnt + 1);

    /* go to sibling if there is one */
    if (ptr->sibling)
	(void) Enumerate(ptr->sibling, cnt);
    return (0);
}


/* PrintOne():
 *	Print the specified key and its associated
 *	function specified by code
 */
static int
printOne(key, code, prstring)
    Char   *key;
    Char   *code;
    int     prstring;
{
    struct KeyFuncs *fp;
    unsigned char unparsbuf[200];
    static char *fmt = "%-15s->  %s\n";

    if (code) {
	if (prstring)
	    xprintf(fmt, short2str(key), unparsestring(code, unparsbuf));
	else {
	    for (fp = FuncNames; fp->name; fp++) {
		if ((int) code == fp->func)
		    xprintf(fmt, short2str(key), fp->name);
	    }
	}
    }
    else
	xprintf(fmt, short2str(key), "no input");
    return (0);
}

static int
unparsech(cnt, ch)
    int     cnt;
    Char    ch;
{
    if (ch == 0) {
	printbuf[cnt++] = '^';
	printbuf[cnt] = '@';
	return cnt;
    }

    if (Iscntrl(ch)) {
	printbuf[cnt++] = '^';
	if (ch == '\177')
	    printbuf[cnt] = '?';
	else
	    printbuf[cnt] = ch | 0100;
    }
    else if (ch == '^') {
	printbuf[cnt++] = '\\';
	printbuf[cnt] = '^';
    }
    else if (ch == '\\') {
	printbuf[cnt++] = '\\';
	printbuf[cnt] = '\\';
    }
    else if (ch == ' ' || (Isprint(ch) && !Isspace(ch))) {
	printbuf[cnt] = ch;
    }
    else {
	printbuf[cnt++] = '\\';
	printbuf[cnt++] = ((ch >> 6) & 7) + '0';
	printbuf[cnt++] = ((ch >> 3) & 7) + '0';
	printbuf[cnt] = (ch & 7) + '0';
    }
    return cnt;
}

/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/tw.init.c,v 1.7 1990/11/29 19:40:31 christos Exp $ */
/*
 * tw.init.c: TwENEX initializations
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id: tw.init.c,v 1.7 1990/11/29 19:40:31 christos Exp $";
#endif

#include "sh.h"
#include "tw.h"

/*
 * Build the command name list (and print the results).  This is a HACK until
 * I can get the rehash command to include its results in the command list.
 */

int	fcompare();
static int maxcommands = 0;


void
tw_clear_comm_list() {
    register int i;

    have_sorted = 0;
    if (numcommands != 0) {
/*        for (i = 0; command_list[i]; i++) { */
        for (i = 0; i < numcommands; i++) {
	    xfree ((ptr_t) command_list[i]);
	    command_list[i] = NULL;
	}
	numcommands = 0;
    }
}

void
tw_sort_comms () {		/* could be re-written */
    register int i,forward;
    
    /* sort the list. */
    qsort ((ptr_t) command_list, (size_t) numcommands, 
	   sizeof (command_list[0]), fcompare);

    /* get rid of multiple entries */
    for (i = 0, forward = 0; i < numcommands - 1; i++) {
	if (Strcmp (command_list[i], command_list[i+1]) == 0) {	/* garbage */
	    xfree ((ptr_t) command_list[i]);
	    forward++;		/* increase the forward ref. count */
	} else if (forward) {
	    command_list[i-forward] = command_list[i];
	}
    }
    /* Fix fencepost error -- Theodore Ts'o <tytso@athena.mit.edu> */
    if (forward)
	    command_list[i-forward] = command_list[i];
    numcommands -= forward;
    command_list[numcommands] = (Char *)NULL;

    have_sorted = 1;
}

void
tw_add_comm_name (name)      /* this is going to be called a LOT at startup */
Char *name;
{
    register int length;
    register long i;
    register Char **ncl, **p1, **p2;

    if (maxcommands == 0) {
	command_list = (Char **) xalloc((size_t) (NUMCMDS_START * 
				        sizeof(command_list[0])));
	maxcommands = NUMCMDS_START;
	for (i = 0, p2 = command_list; i < maxcommands; i++)
	    *p2 = NULL;
    } else if (numcommands >= maxcommands) {
	ncl = (Char **) xalloc((size_t) ((maxcommands + NUMCMDS_INCR) *
			       sizeof (command_list[0])));
	for (i = 0, p1 = command_list, p2 = ncl; i < numcommands; i++)
	    *p2++ = *p1++;
	for (; i < maxcommands+NUMCMDS_INCR; i++)
	    *p2++ = NULL;
	xfree((ptr_t) command_list);
	command_list = ncl;
#ifdef COMMENT
	command_list = (Char **) xralloc(command_list, (maxcommands +
		       NUMCMDS_INCR) * sizeof (command_list[0]));
#endif
	maxcommands += NUMCMDS_INCR;
    }

    if (name[0] == '.') return;	/* no dot commands... */
    if (name[0] == '#') return;	/* no Emacs buffer checkpoints */

    length = Strlen(name) + 1;

    if (name[length-2] == '~') return; /* and no Emacs backups either */

    command_list[numcommands] = (Char *) xalloc ((size_t) (length * 
						 sizeof(Char)));

    copyn (command_list[numcommands], name, MAXNAMLEN);
    numcommands++;
}

void
tw_add_builtins() {
    register struct biltins *bptr;

    for (bptr = bfunc; bptr < &bfunc[nbfunc]; bptr++) {
	if (bptr->bname)
	    tw_add_comm_name (str2short(bptr->bname));
    }
}

void
tw_add_aliases ()
{
    register struct varent *p;
    register struct varent *c;

    p = &aliases;
    for (;;) {
	while (p->v_left)
	    p = p->v_left;
      x:
	if (p->v_parent == 0)		/* is it the header? */
	    return;
	if (p->v_name)
	    tw_add_comm_name(p->v_name);
	if (p->v_right) {
	    p = p->v_right;
	    continue;
	}
	do {
	    c = p;
	    p = p->v_parent;
	} while (p->v_right == c);
	goto x;
    }

}

struct varent *
tw_start_shell_list()
{
    register struct varent *p;
    register struct varent *c;

    p = &shvhed;	/* start at beginning of variable list */

    for (;;) {
	while (p->v_left)
	    p = p->v_left;
      x:
	if (p->v_parent == 0)		/* is it the header? */
	    return (NULL);
	if (p->v_name)
	    return (p);		/* found first one */
	if (p->v_right) {
	    p = p->v_right;
	    continue;
	}
	do {
	    c = p;
	    p = p->v_parent;
	} while (p->v_right == c);
	goto x;
    }
}

Char *
tw_next_shell_var (vptr)
struct varent **vptr;
{
    register struct varent *p;
    register struct varent *c;
    register Char *cp;

    if ((p = *vptr) == NULL)
	return (NULL);		/* just in case */

    cp = p->v_name;		/* we know that this name is here now */

    /* now find the next one */
    for (;;) {
	if (p->v_right) {	/* if we can go right */
	    p = p->v_right;
	    while (p->v_left)
		p = p->v_left;
	} else {		/* else go up */
	    do {
		c = p;
		p = p->v_parent;
	    } while (p->v_right == c);
	}
	if (p->v_parent == 0) {		/* is it the header? */
	    *vptr = NULL;
	    return (cp);
	}
	if (p->v_name) {
	    *vptr = p;		/* save state for the next call */
	    return (cp);
	}
    }

}

Char **
tw_start_env_list()
{
    return(STR_environ);
}

Char *Getenv(str)
Char *str;
{
    Char **var;
    int len, res;
    len = Strlen(str);
    for (var = STR_environ; var != (Char **) 0 && *var != (Char *) 0; var++)
	if ((*var)[len] == '=') {
	   (*var)[len] = '\0';
	   res = StrQcmp(*var, str);
	   (*var)[len] = '=';
	   if (res == 0)
		return(&((*var)[len+1]));
	}
    return((Char *) 0);
}

Char *
tw_next_env_var(evp)
Char *** evp;
{
    static Char buffer[MAXVARLEN + 1];
    Char *ps, *pd;
    if (*evp == (Char **) 0 || **evp == (Char *) 0)
	return ((Char *) 0);
    for (ps = **evp, pd = buffer; 
	 *ps && *ps != '=' && pd <= &buffer[MAXVARLEN]; *pd++ = *ps++);
    *pd = '\0';
    (*evp)++;
    return(buffer);
}

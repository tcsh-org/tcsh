/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/nmalloc.c,v 1.7 1991/03/20 19:04:50 christos Exp $ */
/*
 * malloc.c (Caltech) 2/21/82
 * Chris Kingsley, kingsley@cit-20.
 *
 * This is a very fast storage allocator.  It allocates blocks of a small 
 * number of different sizes, and keeps free lists of each size.  Blocks that
 * don't exactly fit are passed up to the next larger size.  In this 
 * implementation, the available sizes are 2^n-4 (or 2^n-12) bytes long.
 * This is designed for use in a program that uses vast quantities of memory,
 * but bombs when it runs out. 
 */
#include "config.h"
#ifndef lint
static char *rcsid = "$Id: nmalloc.c,v 1.7 1991/03/20 19:04:50 christos Exp $";
#endif
#define MSTATS
#undef RCHECK
#undef debug

#include "sh.h"

#ifndef NULL
# define	NULL 0
#endif

typedef unsigned char U_char;	/* we don't really have signed chars */
typedef unsigned int U_int;
typedef unsigned short U_short;
static int findbucket();
static void morecore();

/*
 * The overhead on a block is at least 4 bytes.  When free, this space
 * contains a pointer to the next free block, and the bottom two bits must
 * be zero.  When in use, the first byte is set to MAGIC, and the second
 * byte is the size index.  The remaining bytes are for alignment.
 * If range checking is enabled and the size of the block fits
 * in two bytes, then the top two bytes hold the size of the requested block
 * plus the range checking words, and the header word MINUS ONE.
 */

union	overhead {
	union	overhead *ov_next;	/* when free */
	struct {
		U_char	ovU_magic;	/* magic number */
		U_char	ovU_index;	/* bucket # */
#ifdef RCHECK
		U_short	ovU_size;	/* actual block size */
		U_int	ovU_rmagic;	/* range magic number */
#endif
	} ovu;
#define	ov_magic	ovu.ovU_magic
#define	ov_index	ovu.ovU_index
#define	ov_size		ovu.ovU_size
#define	ov_rmagic	ovu.ovU_rmagic
};

#define	MAGIC		0xfd		/* magic # on accounting info */
#define RMAGIC		0x55555555	/* magic # on range info */
#ifdef RCHECK
#define	RSLOP		sizeof (U_int)
#else
#define	RSLOP		0
#endif

/*
 * nextf[i] is the pointer to the next free block of size 2^(i+3).  The
 * smallest allocatable block is 8 bytes.  The overhead information
 * precedes the data area returned to the user.
 */
#define	NBUCKETS 30
static union overhead *nextf[NBUCKETS];

#ifdef notdef
extern char *sbrk();
#endif
static char *memtop = NULL;	/* PWP: top of current memory */
static char *membot = NULL;	/* PWP: bottom of allocatable memory */

#ifdef MSTATS
/*
 * nmalloc[i] is the difference between the number of mallocs and frees
 * for a given block size.
 */
static	U_int nmalloc[NBUCKETS];
#endif


#include "sh.local.h"
#ifdef debug
#define	ASSERT(p)   if (!(p)) botch("p"); else
static void
botch(s)
	char *s;
{

	CSHprintf("assertion botched: %s\n", s);
	abort();
}
#else
#define	ASSERT(p)
#endif


memalign_t 
malloc(nbytes)
	register size_t nbytes;
{
#ifndef lint
  	register union overhead *p;
  	register int bucket = 0;
  	register unsigned shiftr;

	/*
	 * Convert amount of memory requested into
	 * closest block size stored in hash buckets
	 * which satisfies request.  Account for
	 * space used per block for accounting.
	 */
  	nbytes += sizeof (union overhead) + RSLOP;
  	nbytes = (nbytes + 3) &~ 3; 
  	shiftr = (nbytes - 1) >> 2;
	/* apart from this loop, this is O(1) */
  	while (shiftr >>= 1)
  		bucket++;
	/*
	 * If nothing in hash bucket right now,
	 * request more memory from the system.
	 */
  	if (nextf[bucket] == NULL)    
  		morecore(bucket);
  	if ((p = (union overhead *)nextf[bucket]) == NULL)
  		return (NULL);
	/* remove from linked list */
  	nextf[bucket] = nextf[bucket]->ov_next;
	p->ov_magic = MAGIC;
	p->ov_index= bucket;
# ifdef MSTATS
  	nmalloc[bucket]++;
# endif
# ifdef RCHECK
	/*
	 * Record allocated size of block and
	 * bound space with magic numbers.
	 */
  	if (nbytes <= 0x10000)
		p->ov_size = nbytes - 1;
	p->ov_rmagic = RMAGIC;
  	*((U_int *)((caddr_t)p + nbytes - RSLOP)) = RMAGIC;
# endif
  	return ((char *)(p + 1));
#else
    if (nbytes)
	return((memalign_t) 0);
    else
	return((memalign_t) 0);
#endif /* !lint */
}

#ifndef lint
/*
 * Allocate more memory to the indicated bucket.
 */
static void
morecore(bucket)
	register bucket;
{
  	register union overhead *op;
  	register int rnu;       /* 2^rnu bytes will be requested */
  	register int nblks;     /* become nblks blocks of the desired size */
	register int siz;

  	if (nextf[bucket])
  		return;
	/*
	 * Insure memory is allocated
	 * on a page boundary.  Should
	 * make getpageize call?
	 */
  	op = (union overhead *)sbrk(0);
	memtop = (char *) op;	/* PWP */
	if (membot == NULL)
	    membot = memtop;	/* PWP */
  	if ((int)op & 0x3ff) {
  		memtop = (char *) sbrk(1024 - ((int)op & 0x3ff)); /* PWP */
		memtop += 1024 - ((int)op & 0x3ff);
	}
	/* take 2k unless the block is bigger than that */
  	rnu = (bucket <= 8) ? 11 : bucket + 3;
  	nblks = 1 << (rnu - (bucket + 3));  /* how many blocks to get */
  	if (rnu < bucket)
		rnu = bucket;
	memtop = (char *) sbrk(1 << rnu); /* PWP */
	op = (union overhead *) memtop;
	memtop += 1 << rnu;
	/* no more room! */
  	if ((int)op == -1)
  		return;
	/*
	 * Round up to minimum allocation size boundary
	 * and deduct from block count to reflect.
	 */
  	if ((int)op & 7) {
  		op = (union overhead *)(((int)op + 8) &~ 7);
  		nblks--;
  	}
	/*
	 * Add new memory allocated to that on
	 * free list for this hash bucket.
	 */
  	nextf[bucket] = op;
  	siz = 1 << (bucket + 3);
  	while (--nblks > 0) {
		op->ov_next = (union overhead *)((caddr_t)op + siz);
		op = (union overhead *)((caddr_t)op + siz);
  	}
}
#endif

void
free(cp)
	ptr_t cp;
{   
#ifndef lint
  	register int size;
	register union overhead *op;

  	if (cp == NULL)
  		return;
	if (!memtop || !membot) {
# ifdef debug
	    CSHprintf ("free(%lx) called before any allocations\n", cp);
# endif
	    return;
	}

	if (cp > (ptr_t) memtop) {
# ifdef debug
	    CSHprintf ("free(%lx) above top of memory: %lx\n", cp, memtop);
# endif
	    return;
	}

	if (cp < (ptr_t) membot) {
# ifdef debug
	    CSHprintf ("free(%lx) above top of memory: %lx\n", cp, memtop);
# endif
	    return;
	}

	op = (union overhead *)((caddr_t)cp - sizeof (union overhead));
# ifdef debug
  	ASSERT(op->ov_magic == MAGIC);		/* make sure it was in use */
# else
	if (op->ov_magic != MAGIC)
		return;				/* sanity */
# endif
# ifdef RCHECK
  	ASSERT(op->ov_rmagic == RMAGIC);
	if (op->ov_index <= 13)
		ASSERT(*(U_int *)((caddr_t)op + op->ov_size + 1 - RSLOP) == RMAGIC);
# endif
# ifdef debug
	if (op->ov_index >= NBUCKETS)
	    CSHprintf ("op->ov_index == %d, NBUCKETS == %d\n",
		    op->ov_index, NBUCKETS);
# endif
  	ASSERT(op->ov_index < NBUCKETS);
  	size = op->ov_index;
	op->ov_next = nextf[size];
  	nextf[size] = op;
# ifdef MSTATS
  	nmalloc[size]--;
# endif
#else
	if (cp == NULL)
		return;
#endif
}

#ifndef lint
/* PWP: a bcopy that does overlapping extents correctly */
static void
mybcopy(from, to, len)
char *from, *to;
register unsigned len;
{
    register char *sp, *dp;

    if (from == to)
	return;
    if (from < to) {
	/* len is unsigned, len > 0 is equivalent to len != 0 */
	for (sp = &from[len-1], dp = &to[len-1]; len != 0; len--, sp--, dp--)
	    *dp = *sp;
    } else {
	/* len is unsigned, len > 0 is equivalent to len != 0 */
	for (sp = from, dp = to; len != 0; len--, sp++, dp++)
	    *dp = *sp;
    }
}
#endif

/*
 * When a program attempts "storage compaction" as mentioned in the
 * old malloc man page, it realloc's an already freed block.  Usually
 * this is the last block it freed; occasionally it might be farther
 * back.  We have to search all the free lists for the block in order
 * to determine its bucket: 1st we make one pass thru the lists
 * checking only the first block in each; if that fails we search
 * ``realloc_srchlen'' blocks in each list for a match (the variable
 * is extern so the caller can modify it).  If that fails we just copy
 * however many bytes was given to realloc() and hope it's not huge.
 */
#ifndef lint
static int realloc_srchlen = 4;	/* 4 should be plenty, -1 =>'s whole list */
#endif

memalign_t
realloc(cp, nbytes)
	ptr_t cp; 
	size_t nbytes;
{   
#ifndef lint
  	register U_int onb;
	union overhead *op;
  	char *res;
	register int i;
	int was_alloced = 0;

  	if (cp == NULL)
  		return (malloc(nbytes));
	op = (union overhead *)((caddr_t)cp - sizeof (union overhead));
	if (op->ov_magic == MAGIC) {
		was_alloced++;
		i = op->ov_index;
	} else {
		/*
		 * Already free, doing "compaction".
		 *
		 * Search for the old block of memory on the
		 * free list.  First, check the most common
		 * case (last element free'd), then (this failing)
		 * the last ``realloc_srchlen'' items free'd.
		 * If all lookups fail, then assume the size of
		 * the memory block being realloc'd is the
		 * smallest possible.
		 */
		if ((i = findbucket(op, 1)) < 0 &&
		    (i = findbucket(op, realloc_srchlen)) < 0)
			i = 0;
	}
	onb = (1 << (i + 3)) - sizeof (*op) - RSLOP;
	/* avoid the copy if same size block */
	if (was_alloced &&
	    nbytes <= onb && nbytes > (onb >> 1) - sizeof(*op) - RSLOP)
		return((char *) cp);
  	if ((res = malloc(nbytes)) == NULL)
  		return (NULL);
  	if (cp != res)			/* common optimization */
		mybcopy(cp, res, (nbytes < onb) ? nbytes : onb);
  	if (was_alloced)
		free(cp);
  	return (res);
#else
    if (cp && nbytes)
	return((memalign_t) 0);
    else
	return((memalign_t) 0);
#endif /* !lint */
}



#ifndef lint
/*
 * Search ``srchlen'' elements of each free list for a block whose
 * header starts at ``freep''.  If srchlen is -1 search the whole list.
 * Return bucket number, or -1 if not found.
 */
static int
findbucket(freep, srchlen)
	union overhead *freep;
	int srchlen;
{
	register union overhead *p;
	register int i, j;

	for (i = 0; i < NBUCKETS; i++) {
		j = 0;
		for (p = nextf[i]; p && j != srchlen; p = p->ov_next) {
			if (p == freep)
				return (i);
			j++;
		}
	}
	return (-1);
}
#endif

#ifdef MSTATS
/*
 * mstats - print out statistics about malloc
 * 
 * Prints two lines of numbers, one showing the length of the free list
 * for each size category, the second showing the number of mallocs -
 * frees for each size category.
 */
void
showall()
{
  	register int i, j;
  	register union overhead *p;
  	int totfree = 0,
  	totused = 0;

  	CSHprintf("tcsh current memory allocation:\nfree:\t");
  	for (i = 0; i < NBUCKETS; i++) {
  		for (j = 0, p = nextf[i]; p; p = p->ov_next, j++)
  			;
  		CSHprintf(" %4d", j);
  		totfree += j * (1 << (i + 3));
  	}
  	CSHprintf("\nused:\t");
  	for (i = 0; i < NBUCKETS; i++) {
  		CSHprintf(" %4d", nmalloc[i]);
  		totused += nmalloc[i] * (1 << (i + 3));
  	}
  	CSHprintf("\n\tTotal in use: %d, total free: %d\n",
	    totused, totfree);
	CSHprintf("\tAllocated memory from 0x%lx to 0x%lx.  Real top at 0x%lx\n",
	       membot, memtop, (char *) sbrk(0));
}
#endif

#ifndef BUDDY

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

typedef long Align; /* for alignment to long boundary */
union header
{ /* block header */
    struct
    {
        union header *ptr; /* next block if on free list */
        unsigned size;     /* size of this block */
    } s;
    Align x; /* force alignment of blocks */
};
typedef union header Header;

// static Header base;          /* empty list to get started */
static Header *base;          /* empty list to get started */
static Header *freep = NULL; /* start of free list */

char initMemoryManager(void * managedMemory) {
    // freep = (Header *) managedMemory;
    // freep->s.ptr = NULL;
    // freep->s.size = ;

    base = (Header *) managedMemory;
}

/* memMalloc: general-purpose storage allocator */
void *
memMalloc(unsigned nbytes)
{
    Header *p, *prevp;
    Header *moreroce(unsigned);
    unsigned nunits;
    // nunits = (nbytes + sizeof(Header) - 1) / sizeof(header) + 1;
    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    if ((prevp = freep) == NULL)
    { /* no free list yet */
        // base.s.ptr = freeptr = prevptr = &base;
        // base.s.ptr = &base;
        // base.s.size = 0;
        base->s.ptr = base;
        base->s.size = 0;
    }
    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr)
    {
        if (p->s.size >= nunits)
        {                            /* big enough */
            if (p->s.size == nunits) /* exactly */
                prevp->s.ptr = p->s.ptr;
            else
            { /* allocate tail end */
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)(p + 1);
        }
        // if (p == freep) /* wrapped around free list */
        //     if ((p = morecore(nunits)) == NULL)
        //         return NULL; /* none left */
    }
}

// #define NALLOC 1024 /* minimum #units to request */

// /* morecore: ask system for more memory */
// static Header *morecore(unsigned nu)
// {
//     char *cp, *sbrk(int);
//     Header *up;
//     if (nu < NALLOC)
//         nu = NALLOC;
//     cp = sbrk(nu * sizeof(Header));
//     if (cp == (char *)-1) /* no space at all */
//         return NULL;
//     up = (Header *)cp;
//     up->s.size = nu;
//     free((void *)(up + 1));
//     return freep;
// }

/* free: put block ap in free list */
void free(void *ap)
{
    Header *bp, *p;
    bp = (Header *)ap - 1; /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break; /* freed block at start or end of arena */
    if (bp + bp->s.size == p->s.ptr)
    { /* join to upper nbr */
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    }
    else
        bp->s.ptr = p->s.ptr;
    if (p + p->s.size == bp)
    { /* join to lower nbr */
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    }
    else
        p->s.ptr = bp;
    freep = p;
}

char testOne() {
    void * alloc1 = memMalloc(100);
    void * alloc2 = memMalloc(200);
    void * alloc3 = memMalloc(300);

    memset(alloc1, 1, 100);
    memset(alloc2, 2, 200);
    memset(alloc3, 3, 300);

    for (int i = 0; i < 600; i++) {
        if (i < 100) {
            assert(*((char *) alloc1+i) == 1);
        } else if (i < 300) {
            assert(*((char *) alloc1+i) == 2);
        } else if (i < 600) {
            assert(*((char *) alloc1+i) == 3);
        }
    }

    return EXIT_SUCCESS;
}

static unsigned long int next = 1;
int rand(void) // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

/*
char testTwo() {
    void * ptr;
    while (ptr != NULL){
        ptr = memMalloc((rand() % 2000) + 1);
        if (!((char *) memoryManager->nextAddress >= memoryManager->initialAddress)) {
    			printStringLen(13, "allocRand1 -- ERROR", 31); 
				new_line();
				return EXIT_FAILURE;
		}
        if (!((char *) memoryManager->nextAddress <= memoryManager->initialAddress + MANAGED_MEMORY_SIZE)) {
    			printStringLen(13, "allocRand2 -- ERROR", 31); 
				new_line();
				return EXIT_FAILURE;
        }
		// if (!((char *) memoryManager->lastAddress >= memoryManager->initialAddress)) {
        // }
        // if (!((char *) memoryManager->lastAddress <= memoryManager->initialAddress + MANAGED_MEMORY_SIZE)) {
        // }
    }

    return EXIT_SUCCESS;

}
*/

char mem[1024];

int main() {
    initMemoryManager(mem);
	if (testOne() == EXIT_FAILURE)
		return EXIT_FAILURE;
	// if (testTwo() == EXIT_FAILURE)
		// return EXIT_FAILURE;
}

#endif
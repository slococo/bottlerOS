// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef BUDDY

#include "memManagerFRT4.h"

static uint8_t *ucHeap;

void initMemoryManager(void *managedMemory) {
    ucHeap = managedMemory;
}

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize = (sizeof(BlockLink_t) + ((size_t)(portBYTE_ALIGNMENT - 1))) & ~((size_t)portBYTE_ALIGNMENT_MASK);

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t xStart, *pxEnd = NULL;

/* Keeps track of the number of calls to allocate and free memory as well as the
number of free bytes remaining, but says nothing about fragmentation. */
static size_t xFreeBytesRemaining = 0U;
static size_t xMinimumEverFreeBytesRemaining = 0U;
static size_t xNumberOfSuccessfulAllocations = 0;
static size_t xNumberOfSuccessfulFrees = 0;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
member of an BlockLink_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
static size_t xBlockAllocatedBit = 0;

/*-----------------------------------------------------------*/

void *pvPortMalloc(size_t xWantedSize) {
    BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
    void *pvReturn = NULL;

    /* If this is the first call to malloc then the heap will require
    initialisation to setup the list of free blocks. */
    if (pxEnd == NULL) {
        prvHeapInit();
    }

    /* Check the requested block size is not so large that the top bit is
    set.  The top bit of the block size member of the BlockLink_t structure
    is used to determine who owns the block - the application or the
    kernel, so it must be free. */
    if ((xWantedSize & xBlockAllocatedBit) == 0) {
        /* The wanted size is increased so it can contain a BlockLink_t
        structure in addition to the requested amount of bytes. */
        if (xWantedSize > 0) {
            xWantedSize += xHeapStructSize;

            /* Ensure that blocks are always aligned to the required number
            of bytes. */
            if ((xWantedSize & portBYTE_ALIGNMENT_MASK) != 0x00) {
                /* Byte alignment required. */
                xWantedSize += (portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK));
            }
        }

        if ((xWantedSize > 0) && (xWantedSize <= xFreeBytesRemaining)) {
            /* Traverse the list from the start	(lowest address) block until
            one	of adequate size is found. */
            pxPreviousBlock = &xStart;
            pxBlock = xStart.pxNextFreeBlock;
            while ((pxBlock->xBlockSize < xWantedSize) && (pxBlock->pxNextFreeBlock != NULL)) {
                pxPreviousBlock = pxBlock;
                pxBlock = pxBlock->pxNextFreeBlock;
            }

            /* If the end marker was reached then a block of adequate size
            was	not found. */
            if (pxBlock != pxEnd) {
                /* Return the memory space pointed to - jumping over the
                BlockLink_t structure at its start. */
                pvReturn = (void *) (((uint8_t *) pxPreviousBlock->pxNextFreeBlock) + xHeapStructSize);

                /* This block is being returned for use so must be taken out
                of the list of free blocks. */
                pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

                /* If the block is larger than required it can be split into
                two. */
                if ((pxBlock->xBlockSize - xWantedSize) > heapMINIMUM_BLOCK_SIZE) {
                    /* This block is to be split into two.  Create a new
                    block following the number of bytes requested. The void
                    cast is used to prevent byte alignment warnings from the
                    compiler. */
                    pxNewBlockLink = (void *) (((uint8_t *) pxBlock) + xWantedSize);

                    /* Calculate the sizes of two blocks split from the
                    single block. */
                    pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
                    pxBlock->xBlockSize = xWantedSize;

                    /* Insert the new block into the list of free blocks. */
                    prvInsertBlockIntoFreeList(pxNewBlockLink);
                }

                xFreeBytesRemaining -= pxBlock->xBlockSize;

                if (xFreeBytesRemaining < xMinimumEverFreeBytesRemaining) {
                    xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
                }

                /* The block is being returned - it is allocated and owned
                by the application and has no "next" block. */
                pxBlock->xBlockSize |= xBlockAllocatedBit;
                pxBlock->pxNextFreeBlock = NULL;
                xNumberOfSuccessfulAllocations++;
            }
        } else {

        }
    }

    return pvReturn;
}

/*-----------------------------------------------------------*/
void vPortFree(void *pv) {
    uint8_t *puc = (uint8_t *) pv;
    BlockLink_t *pxLink;

    if (pv != NULL) {
        /* The memory being freed will have an BlockLink_t structure immediately
        before it. */
        puc -= xHeapStructSize;

        /* This casting is to keep the compiler from issuing warnings. */
        pxLink = (void *) puc;

        /* Check the block is actually allocated. */
        // configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );
        // configASSERT( pxLink->pxNextFreeBlock == NULL );

        if ((pxLink->xBlockSize & xBlockAllocatedBit) != 0) {
            if (pxLink->pxNextFreeBlock == NULL) {
                /* The block is being returned to the heap - it is no longer
                allocated. */
                pxLink->xBlockSize &= ~xBlockAllocatedBit;

                    /* Add this block to the list of free blocks. */
                    xFreeBytesRemaining += pxLink->xBlockSize;
                    prvInsertBlockIntoFreeList(((BlockLink_t *) pxLink));
                    xNumberOfSuccessfulFrees++;
            }
        }
    }
}

/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize(void) {
    return xFreeBytesRemaining;
}

/*-----------------------------------------------------------*/

size_t xPortGetMinimumEverFreeHeapSize(void) {
    return xMinimumEverFreeBytesRemaining;
}

/*-----------------------------------------------------------*/

void vPortInitialiseBlocks(void) {
    /* This just exists to keep the linker quiet. */
}

/*-----------------------------------------------------------*/

static void prvHeapInit(void) {
    BlockLink_t *pxFirstFreeBlock;
    uint8_t *pucAlignedHeap;
    size_t uxAddress;
    size_t xTotalHeapSize = configTOTAL_HEAP_SIZE;

    /* Ensure the heap starts on a correctly aligned boundary. */
    uxAddress = (size_t) ucHeap;

    if ((uxAddress & portBYTE_ALIGNMENT_MASK) != 0) {
        uxAddress += (portBYTE_ALIGNMENT - 1);
        uxAddress &= ~((size_t)portBYTE_ALIGNMENT_MASK);
        xTotalHeapSize -= uxAddress - (size_t) ucHeap;
    }

    pucAlignedHeap = (uint8_t *) uxAddress;

    /* xStart is used to hold a pointer to the first item in the list of free
    blocks.  The void cast is used to prevent compiler warnings. */
    xStart.pxNextFreeBlock = (void *) pucAlignedHeap;
    xStart.xBlockSize = (size_t) 0;

    /* pxEnd is used to mark the end of the list of free blocks and is inserted
    at the end of the heap space. */
    uxAddress = ((size_t) pucAlignedHeap) + xTotalHeapSize;
    uxAddress -= xHeapStructSize;
    uxAddress &= ~((size_t)portBYTE_ALIGNMENT_MASK);
    pxEnd = (void *) uxAddress;
    pxEnd->xBlockSize = 0;
    pxEnd->pxNextFreeBlock = NULL;

    /* To start with there is a single free block that is sized to take up the
    entire heap space, minus the space taken by pxEnd. */
    pxFirstFreeBlock = (void *) pucAlignedHeap;
    pxFirstFreeBlock->xBlockSize = uxAddress - (size_t) pxFirstFreeBlock;
    pxFirstFreeBlock->pxNextFreeBlock = pxEnd;

    /* Only one block exists - and it covers the entire usable heap space. */
    xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;
    xFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;

    /* Work out the position of the top bit in a size_t variable. */
    xBlockAllocatedBit = ((size_t) 1) << ((sizeof(size_t) * heapBITS_PER_BYTE) - 1);
}

/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert) {
    BlockLink_t *pxIterator;
    uint8_t *puc;

    /* Iterate through the list until a block is found that has a higher address
    than the block being inserted. */
    for (pxIterator = &xStart;
         pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock) {
        /* Nothing to do here, just iterate to the right position. */
    }

    /* Do the block being inserted, and the block it is being inserted after
    make a contiguous block of memory? */
    puc = (uint8_t *) pxIterator;
    if ((puc + pxIterator->xBlockSize) == (uint8_t *) pxBlockToInsert) {
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
        pxBlockToInsert = pxIterator;
    }

    /* Do the block being inserted, and the block it is being inserted before
    make a contiguous block of memory? */
    puc = (uint8_t *) pxBlockToInsert;
    if ((puc + pxBlockToInsert->xBlockSize) == (uint8_t *) pxIterator->pxNextFreeBlock) {
        if (pxIterator->pxNextFreeBlock != pxEnd) {
            /* Form one big block from the two blocks. */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        } else {
            pxBlockToInsert->pxNextFreeBlock = pxEnd;
        }
    } else {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    /* If the block being inserted plugged a gab, so was merged with the block
    before and the block after, then it's pxNextFreeBlock pointer will have
    already been set, and should not be set here as that would make it point
    to itself. */
    if (pxIterator != pxBlockToInsert) {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    }
}

/*-----------------------------------------------------------*/

void vPortGetHeapStats(HeapStats_t *pxHeapStats) {
    BlockLink_t *pxBlock;
    size_t xBlocks = 0, xMaxSize = 0, xMinSize =
            configTOTAL_HEAP_SIZE;//portMAX_DELAY; /* portMAX_DELAY used as a portable way of getting the maximum value. */

    pxBlock = xStart.pxNextFreeBlock;

    /* pxBlock will be NULL if the heap has not been initialised.  The heap
    is initialised automatically when the first allocation is made. */
    if (pxBlock != NULL) {
        do {
            /* Increment the number of blocks and record the largest block seen
            so far. */
            xBlocks++;

            if (pxBlock->xBlockSize > xMaxSize) {
                xMaxSize = pxBlock->xBlockSize;
            }

            if (pxBlock->xBlockSize < xMinSize) {
                xMinSize = pxBlock->xBlockSize;
            }

            /* Move to the next block in the chain until the last block is
            reached. */
            pxBlock = pxBlock->pxNextFreeBlock;
        } while (pxBlock != pxEnd);
    }

    pxHeapStats->xSizeOfLargestFreeBlockInBytes = xMaxSize;
    pxHeapStats->xSizeOfSmallestFreeBlockInBytes = xMinSize;
    pxHeapStats->xNumberOfFreeBlocks = xBlocks;

    pxHeapStats->xAvailableHeapSpaceInBytes = xFreeBytesRemaining;
    pxHeapStats->xNumberOfSuccessfulAllocations = xNumberOfSuccessfulAllocations;
    pxHeapStats->xNumberOfSuccessfulFrees = xNumberOfSuccessfulFrees;
    pxHeapStats->xMinimumEverFreeBytesRemaining = xMinimumEverFreeBytesRemaining;
}


/*
// Definition of the Heap_stats_t structure.
typedef struct xHeapStats {
    size_t xAvailableHeapSpaceInBytes;      // The total heap size currently available - this is the sum of all the free blocks, not the largest block that can be allocated.
    size_t xSizeOfLargestFreeBlockInBytes;    // The maximum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. 
    size_t xSizeOfSmallestFreeBlockInBytes; // The minimum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. 
    size_t xNumberOfFreeBlocks;        // The number of free memory blocks within the heap at the time vPortGetHeapStats() is called. 
    size_t xMinimumEverFreeBytesRemaining;    // The minimum amount of total free memory (sum of all free blocks) there has been in the heap since the system booted. 
    size_t xNumberOfSuccessfulAllocations;    // The number of calls to pvPortMalloc() that have returned a valid memory block. 
    size_t xNumberOfSuccessfulFrees;    // The number of calls to vPortFree() that has successfully freed a block of memory. 
} HeapStats_t;
*/


char *dumpMM() {
    char *ans = pvPortMalloc(DUMP_MAX_SIZE);
    char *ret = ans;

    HeapStats_t * heapStats = pvPortMalloc(sizeof(HeapStats_t));
    vPortGetHeapStats(heapStats);

    char buffer[20] = {0};
    ans += strcpy(ans, "Free memory: ");
    ans += strcpy(ans, itoa(heapStats->xAvailableHeapSpaceInBytes, buffer, 10, 20));
    ans += strcpy(ans, "\nTotal memory: ");
    ans += strcpy(ans, itoa(configTOTAL_HEAP_SIZE, buffer, 10, 20));
    ans += strcpy(ans, "\nmallocs: ");
    ans += strcpy(ans, itoa(heapStats->xNumberOfSuccessfulAllocations, buffer, 10, 20));
    ans += strcpy(ans, "\nfrees: ");
    ans += strcpy(ans, itoa(heapStats->xNumberOfSuccessfulFrees, buffer, 10, 20));   
    vPortFree(heapStats);
    return ret;
}


#endif /* #ifdef configHEAP_ALLOCATION_SCHEME */
// #endif /* #if(configHEAP_ALLOCATION_SCHEME == HEAP_ALLOCATION_TYPE4) */
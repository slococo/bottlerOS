#ifndef MMFRT4_H
#define MMFRT4_H

#include "memManager.h"
#include <stddef.h>
#include <stdint.h>
#include "lib.h"

#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#define portCHAR        char
#define portFLOAT        float
#define portDOUBLE        double
#define portLONG        long
#define portSHORT        short
#define portSTACK_TYPE    uint32_t
#define portBASE_TYPE    long

#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   1024 * 1024 * 512
#define configAPPLICATION_ALLOCATED_HEAP        0
#define portBYTE_ALIGNMENT 8
#define portBYTE_ALIGNMENT_MASK ( 0x0007 )
#define pdFALSE            ( ( BaseType_t ) 0 )
#define pdTRUE            ( ( BaseType_t ) 1 )
#define portPOINTER_SIZE_TYPE uint32_t

/* A few bytes might be lost to byte aligning the heap start address. */
#define configADJUSTED_HEAP_SIZE    ( configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT )
#define heapBITS_PER_BYTE        ( ( size_t ) 8 )
#define heapMINIMUM_BLOCK_SIZE    ( ( size_t ) ( xHeapStructSize << 1 ) )

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK {
    struct A_BLOCK_LINK *pxNextFreeBlock;    /*<< The next free block in the list. */
    size_t xBlockSize;                        /*<< The size of the free block. */
} BlockLink_t;

/* Definition of the Heap_stats_t structure. */
typedef struct xHeapStats {
    size_t xAvailableHeapSpaceInBytes;      /* The total heap size currently available - this is the sum of all the free blocks, not the largest block that can be allocated. */
    size_t xSizeOfLargestFreeBlockInBytes;    /* The maximum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    size_t xSizeOfSmallestFreeBlockInBytes; /* The minimum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    size_t xNumberOfFreeBlocks;        /* The number of free memory blocks within the heap at the time vPortGetHeapStats() is called. */
    size_t xMinimumEverFreeBytesRemaining;    /* The minimum amount of total free memory (sum of all free blocks) there has been in the heap since the system booted. */
    size_t xNumberOfSuccessfulAllocations;    /* The number of calls to pvPortMalloc() that have returned a valid memory block. */
    size_t xNumberOfSuccessfulFrees;    /* The number of calls to vPortFree() that has successfully freed a block of memory. */
} HeapStats_t;

/* Prototype of the vPortGetHeapStats() function. */
void vPortGetHeapStats(HeapStats_t *xHeapStats);

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void prvInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert);

/*
 * Called automatically to setup the required heap structures the first time
 * pvPortMalloc() is called.
 */
static void prvHeapInit(void);

#endif
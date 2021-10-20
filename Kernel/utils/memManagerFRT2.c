// #ifndef BUDDY

// #include <stdlib.h>
// #include <stdint.h>
// #include <assert.h>
// #include <string.h>
// #include <naiveConsole.h>

// #define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

// #define portCHAR		char
// #define portFLOAT		float
// #define portDOUBLE		double
// #define portLONG		long
// #define portSHORT		short
// #define portSTACK_TYPE	uint32_t
// #define portBASE_TYPE	long

// typedef portSTACK_TYPE StackType_t;
// typedef long BaseType_t;
// typedef unsigned long UBaseType_t;

// #define configSUPPORT_STATIC_ALLOCATION         1
// #define configSUPPORT_DYNAMIC_ALLOCATION        1
// #define configTOTAL_HEAP_SIZE                   1024
// #define configAPPLICATION_ALLOCATED_HEAP        0
// #define portBYTE_ALIGNMENT 8
// #define portBYTE_ALIGNMENT_MASK ( 0x0007 ) // 8
// #define pdFALSE			( ( BaseType_t ) 0 )
// #define pdTRUE			( ( BaseType_t ) 1 )
// #define portPOINTER_SIZE_TYPE uint32_t

// /* A few bytes might be lost to byte aligning the heap start address. */
// #define configADJUSTED_HEAP_SIZE	( configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT )

// /*
//  * Initialises the heap structures before their first use.
//  */
// static void prvHeapInit(void);

// /* Allocate the memory for the heap. */
// // #if( configAPPLICATION_ALLOCATED_HEAP == 1 )
// // 	/* The application writer has already defined the array used for the RTOS
// // 	heap - probably so it can be placed in a special segment or address. */
// // 	extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
// // #else
// // 	static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
// // #endif /* configAPPLICATION_ALLOCATED_HEAP */

// static uint8_t *ucHeap;
// void initMemoryManager(void * managedMemory) {
// 	ucHeap = managedMemory;
// }

// /* Define the linked list structure.  This is used to link free blocks in order
// of their size. */
// typedef struct A_BLOCK_LINK
// {
// 	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
// 	size_t xBlockSize;						/*<< The size of the free block. */
// } BlockLink_t;


// static const uint16_t heapSTRUCT_SIZE	= ( ( sizeof ( BlockLink_t ) + ( portBYTE_ALIGNMENT - 1 ) ) & ~portBYTE_ALIGNMENT_MASK );
// #define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( heapSTRUCT_SIZE * 2 ) )

// /* Create a couple of list links to mark the start and end of the list. */
// static BlockLink_t xStart, xEnd;

// /* Keeps track of the number of free bytes remaining, but says nothing about
// fragmentation. */
// static size_t xFreeBytesRemaining = configADJUSTED_HEAP_SIZE;

// /* STATIC FUNCTIONS ARE DEFINED AS MACROS TO MINIMIZE THE FUNCTION CALL DEPTH. */

// /*
//  * Insert a block into the list of free blocks - which is ordered by size of
//  * the block.  Small blocks at the start of the list and large blocks at the end
//  * of the list.
//  */
// #define prvInsertBlockIntoFreeList( pxBlockToInsert )								\
// {																					\
// BlockLink_t *pxIterator;															\
// size_t xBlockSize;																	\
// 																					\
// 	xBlockSize = pxBlockToInsert->xBlockSize;										\
// 																					\
// 	/* Iterate through the list until a block is found that has a larger size */	\
// 	/* than the block we are inserting. */											\
// 	for( pxIterator = &xStart; pxIterator->pxNextFreeBlock->xBlockSize < xBlockSize; pxIterator = pxIterator->pxNextFreeBlock )	\
// 	{																				\
// 		/* There is nothing to do here - just iterate to the correct position. */	\
// 	}																				\
// 																					\
// 	/* Update the list to include the block being inserted in the correct */		\
// 	/* position. */																	\
// 	pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;					\
// 	pxIterator->pxNextFreeBlock = pxBlockToInsert;									\
// }
// /*-----------------------------------------------------------*/
// void *pvPortMalloc( size_t xWantedSize )
// {
// BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
// static BaseType_t xHeapHasBeenInitialised = pdFALSE;
// void *pvReturn = NULL;
// 	// vTaskSuspendAll();
// 	{
// 		/* If this is the first call to malloc then the heap will require
// 		initialisation to setup the list of free blocks. */
// 		if( xHeapHasBeenInitialised == pdFALSE )
// 		{
// 			prvHeapInit();
// 			xHeapHasBeenInitialised = pdTRUE;
// 		}

// 		/* The wanted size is increased so it can contain a BlockLink_t
// 		structure in addition to the requested amount of bytes. */
// 		if( xWantedSize > 0 )
// 		{
// 			xWantedSize += heapSTRUCT_SIZE;

// 			/* Ensure that blocks are always aligned to the required number of bytes. */
// 			if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0 )
// 			{
// 				/* Byte alignment required. */
// 				xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
// 			}
// 		}

// 				ncPrint("MALLOC: ");
// 				ncPrintDec(xFreeBytesRemaining);
// 				ncPrint(" ");
// 				ncPrintDec(xWantedSize);
// 				ncPrint(" ");
// 				ncPrintDec(configADJUSTED_HEAP_SIZE);
// 				ncPrint(" ---- ");
// 		if( ( xWantedSize > 0 ) && ( xWantedSize < configADJUSTED_HEAP_SIZE ) )
// 		{
// 			/* Blocks are stored in byte order - traverse the list from the start
// 			(smallest) block until one of adequate size is found. */
// 			pxPreviousBlock = &xStart;
// 			pxBlock = xStart.pxNextFreeBlock;
// 			while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock != NULL ) )
// 			{
// 				pxPreviousBlock = pxBlock;
// 				ncPrintDec(pxBlock->xBlockSize);
// 				ncPrint(" - ");
// 				pxBlock = pxBlock->pxNextFreeBlock;
// 			}

// 			/* If we found the end marker then a block of adequate size was not found. */
// 			if( pxBlock != &xEnd )
// 			{
// 				/* Return the memory space - jumping over the BlockLink_t structure
// 				at its start. */
// 				pvReturn = ( void * ) ( ( ( uint8_t * ) pxPreviousBlock->pxNextFreeBlock ) + heapSTRUCT_SIZE );

// 				/* This block is being returned for use so must be taken out of the
// 				list of free blocks. */
// 				pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

// 				/* If the block is larger than required it can be split into two. */
// 				if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
// 				{
// 					/* This block is to be split into two.  Create a new block
// 					following the number of bytes requested. The void cast is
// 					used to prevent byte alignment warnings from the compiler. */
// 					pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + xWantedSize );

// 					/* Calculate the sizes of two blocks split from the single
// 					block. */
// 					pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
// 					pxBlock->xBlockSize = xWantedSize;

// 					/* Insert the new block into the list of free blocks. */
// 					prvInsertBlockIntoFreeList( ( pxNewBlockLink ) );
// 				}

// 			}
// 				// tengo 16 de info y de alineamiento 8!
// 				ncPrint("Dentro: ");
// 				ncPrintDec(xFreeBytesRemaining);
// 				ncPrint(" y ");
// 				xFreeBytesRemaining -= pxBlock->xBlockSize;
// 				ncPrintDec(xFreeBytesRemaining);
// 				ncNewline();
// 		}

// 		// traceMALLOC( pvReturn, xWantedSize );
// 	}
// 	// ( void ) xTaskResumeAll();

// 	// #if( configUSE_MALLOC_FAILED_HOOK == 1 )
// 	// {
// 	// 	if( pvReturn == NULL )
// 	// 	{
// 	// 		extern void vApplicationMallocFailedHook( void );
// 	// 		vApplicationMallocFailedHook();
// 	// 	}
// 	// }
// 	// #endif

// 	return pvReturn;
// }
// /*-----------------------------------------------------------*/

// void vPortFree( void *pv )
// {
// uint8_t *puc = ( uint8_t * ) pv;
// BlockLink_t *pxLink;

// 	if( pv != NULL )
// 	{
// 		/* The memory being freed will have an BlockLink_t structure immediately
// 		before it. */
// 		puc -= heapSTRUCT_SIZE;

// 		/* This unexpected casting is to keep some compilers from issuing
// 		byte alignment warnings. */
// 		pxLink = ( void * ) puc;

// 		// vTaskSuspendAll();
// 		{
// 			/* Add this block to the list of free blocks. */
// 			prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
// 			xFreeBytesRemaining += pxLink->xBlockSize;
// 			// traceFREE( pv, pxLink->xBlockSize );
// 		}
// 		// ( void ) xTaskResumeAll();
// 	}
// }
// /*-----------------------------------------------------------*/

// size_t xPortGetFreeHeapSize( void )
// {
// 	return xFreeBytesRemaining;
// }
// /*-----------------------------------------------------------*/

// void vPortInitialiseBlocks( void )
// {
// 	/* This just exists to keep the linker quiet. */
// }
// /*-----------------------------------------------------------*/

// static void prvHeapInit( void )
// {
// BlockLink_t *pxFirstFreeBlock;
// uint8_t *pucAlignedHeap;

// 	/* Ensure the heap starts on a correctly aligned boundary. */
// 	pucAlignedHeap = ( uint8_t * ) ( ( ( portPOINTER_SIZE_TYPE ) &ucHeap[ portBYTE_ALIGNMENT ] ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );

// 	/* xStart is used to hold a pointer to the first item in the list of free
// 	blocks.  The void cast is used to prevent compiler warnings. */
// 	xStart.pxNextFreeBlock = ( void * ) pucAlignedHeap;
// 	xStart.xBlockSize = ( size_t ) 0;

// 	/* xEnd is used to mark the end of the list of free blocks. */
// 	xEnd.xBlockSize = configADJUSTED_HEAP_SIZE;
// 	xEnd.pxNextFreeBlock = NULL;

// 	/* To start with there is a single free block that is sized to take up the
// 	entire heap space. */
// 	pxFirstFreeBlock = ( void * ) pucAlignedHeap;
// 	pxFirstFreeBlock->xBlockSize = configADJUSTED_HEAP_SIZE;
// 	pxFirstFreeBlock->pxNextFreeBlock = &xEnd;
// }
// /*-----------------------------------------------------------*/

// // #endif /* #ifdef configHEAP_ALLOCATION_SCHEME */
// // #endif /* #if(configHEAP_ALLOCATION_SCHEME == HEAP_ALLOCATION_TYPE1) */

// // char testOne() {
// //     void * alloc1 = pvPortMalloc(100);
// //     void * alloc2 = pvPortMalloc(200);
// //     void * alloc3 = pvPortMalloc(300);

// //     memset(alloc1, 1, 100);
// //     memset(alloc2, 2, 200);
// //     memset(alloc3, 3, 300);

// //     for (int i = 0; i < 600; i++) {
// //         if (i < 100) {
// //             assert(*((char *) alloc1+i) == 1);
// //         } else if (i < 300) {
// //             assert(*((char *) alloc2+i-100) == 2);
// //         } else if (i < 600) {
// //             assert(*((char *) alloc3+i-300) == 3);
// //         }
// //     }

// //     return EXIT_SUCCESS;
// // }

// // static unsigned long int next = 1;
// // int rand(void) // RAND_MAX assumed to be 32767
// // {
// //     next = next * 1103515245 + 12345;
// //     return (unsigned int)(next/65536) % 32768;
// // }

// /*
// char testTwo() {
//     void * ptr;
//     while (ptr != NULL){
//         ptr = memMalloc((rand() % 2000) + 1);
//         if (!((char *) memoryManager->nextAddress >= memoryManager->initialAddress)) {
//     			printStringLen(13, "allocRand1 -- ERROR", 31); 
// 				new_line();
// 				return EXIT_FAILURE;
// 		}
//         if (!((char *) memoryManager->nextAddress <= memoryManager->initialAddress + MANAGED_MEMORY_SIZE)) {
//     			printStringLen(13, "allocRand2 -- ERROR", 31); 
// 				new_line();
// 				return EXIT_FAILURE;
//         }
// 		// if (!((char *) memoryManager->lastAddress >= memoryManager->initialAddress)) {
//         // }
//         // if (!((char *) memoryManager->lastAddress <= memoryManager->initialAddress + MANAGED_MEMORY_SIZE)) {
//         // }
//     }

//     return EXIT_SUCCESS;

// }
// */

// // char mem[1024];

// // int main() {
// //     // initMemoryManager(mem);
// // 	if (testOne() == EXIT_FAILURE)
// // 		return EXIT_FAILURE;
// // 	// if (testTwo() == EXIT_FAILURE)
// // 		// return EXIT_FAILURE;
// // }

// #endif
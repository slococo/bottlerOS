#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include <stdlib.h>

// typedef struct MemoryManagerCDT * MemoryManagerADT;

// MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory);
// void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

// char initMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory);
void initMemoryManager(void * managedMemory);
void * memMalloc(unsigned nbytes);
void memFree(void *ap);
// void * memMalloc(const size_t memoryToAllocate);
void * pvPortMalloc(size_t xWantedSize);
void vPortFree( void *pv );
size_t xPortGetFreeHeapSize( void );

// SACAR DPS
char testOne();
char testTwo();

#endif
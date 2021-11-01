#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include <stdlib.h>

void initMemoryManager(void * managedMemory);
void * memMalloc(unsigned nbytes);
void memFree(void *ap);
// void * memMalloc(const size_t memoryToAllocate);
void * pvPortMalloc(size_t xWantedSize);
void vPortFree( void *pv );
size_t xPortGetFreeHeapSize( void );

#endif
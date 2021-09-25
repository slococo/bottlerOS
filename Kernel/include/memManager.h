#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include <stdlib.h>

typedef struct MemoryManagerCDT * MemoryManagerADT;

// MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory);
// void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

char initMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory);
void * memMalloc(const size_t memoryToAllocate);

// SACAR DPS
char testOne();
char testTwo();

#endif
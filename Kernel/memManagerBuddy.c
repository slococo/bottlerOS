#ifdef BUDDY

#include "memManager.h"

char initMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
    return 1;
}

void * memMalloc(const size_t memoryToAllocate) {
    return NULL;
}

#endif
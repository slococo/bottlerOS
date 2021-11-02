#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include <stdlib.h>
#define DUMP_MAX_SIZE 100

void initMemoryManager(void * managedMemory);
void *pvPortMalloc(size_t xWantedSize);
void vPortFree( void *pv );
char *dumpMM();

#endif
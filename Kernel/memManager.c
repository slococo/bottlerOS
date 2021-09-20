#include "include/memManager.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MANAGED_MEMORY_SIZE 20480

char mem[MANAGED_MEMORY_SIZE];
char mem2[MANAGED_MEMORY_SIZE];

typedef struct MemoryManagerCDT {
	char *nextAddress;
    char *lastAddress;
    char *initialAddress;
} MemoryManagerCDT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->nextAddress = managedMemory;
    char * aux = managedMemory;
    memoryManager->lastAddress = aux - 1;
    memoryManager->initialAddress = managedMemory;

	return memoryManager;
}

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate) {
	char *allocation = memoryManager->nextAddress;
    if (memoryToAllocate + memoryManager->nextAddress > memoryManager->initialAddress + MANAGED_MEMORY_SIZE){
        return NULL;
    }

	memoryManager->nextAddress += memoryToAllocate;
    if (!(memoryManager->nextAddress -1 == (memoryManager->lastAddress += memoryToAllocate)))
        return NULL;

	return (void *) allocation;
}

int main() {
	static MemoryManagerADT memoryManager;
    static MemoryManagerADT memoryManager2;

    void * memoryForMemoryManager = (void *) 0x700000;
    // void * memoryForMemoryManager = malloc(sizeof(void *));
	if (memoryForMemoryManager == NULL) {
        return 1;
	}

	void *managedMemory = (void *) 0x700016;// malloc(MANAGED_MEMORY_SIZE);
	if (managedMemory == NULL) {
        return 1;
	}


    void * memoryForMemoryManager2 = (void *) 0x700008;
    // void * memoryForMemoryManager2 = malloc(sizeof(void *));
	memoryManager = createMemoryManager(memoryForMemoryManager, managedMemory);

    void * alloc1 = allocMemory(memoryManager, 100);
    void * alloc2 = allocMemory(memoryManager, 200);
    void * alloc3 = allocMemory(memoryManager, 300);

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

    managedMemory = (void *) (0x700016 + 600);
	memoryManager2 = createMemoryManager(memoryForMemoryManager2, managedMemory);

    void * ptr;
    while (ptr != NULL){
        ptr = allocMemory(memoryManager2, (rand() % 2000) + 1);
        assert ((char *) memoryManager2->nextAddress >= mem2);
        assert ((char *) memoryManager2->nextAddress <= mem2 + MANAGED_MEMORY_SIZE);
		assert ((char *) memoryManager2->lastAddress >= mem2);
        assert ((char *) memoryManager2->lastAddress <= mem2 + MANAGED_MEMORY_SIZE);
    }
}
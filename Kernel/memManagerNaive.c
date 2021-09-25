#ifndef BUDDY

#include "memManager.h"
// #include <stdio.h>
#include <string.h>
#include <assert.h>
#include "video.h"

#define MANAGED_MEMORY_SIZE 1024 * 1024 * 64

// char mem[MANAGED_MEMORY_SIZE];
// char mem2[MANAGED_MEMORY_SIZE];

typedef struct MemoryManagerCDT {
	char *nextAddress;
    // char *lastAddress;
    char *initialAddress;
} MemoryManagerCDT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->nextAddress = managedMemory;
    // char * aux = managedMemory;
    // memoryManager->lastAddress = aux - 1;
    memoryManager->initialAddress = managedMemory;

	return memoryManager;
}

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate) {
	char *allocation = memoryManager->nextAddress;
    if (memoryToAllocate + memoryManager->nextAddress > memoryManager->initialAddress + MANAGED_MEMORY_SIZE){
        return NULL;
    }

	memoryManager->nextAddress += memoryToAllocate;
    // if (!(memoryManager->nextAddress -1 == (memoryManager->lastAddress += memoryToAllocate)))
    //     return NULL;

	return (void *) allocation;
}

// Esto es mejor para independizarnos del puntero a memoryManager! Y poder llamar desde distintos lugares (sin la referencia del memManager) a malloc...

static MemoryManagerADT memoryManager;
char initMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
    return ((memoryManager = createMemoryManager(memoryForMemoryManager, managedMemory)) == NULL) ? EXIT_FAILURE : EXIT_SUCCESS;
}

void * memMalloc(const size_t memoryToAllocate) {
    return allocMemory(memoryManager, memoryToAllocate);
}


// SACAR DESPUÉS! ES SOLO PARA TESTEO...

char testOne() {
    void * alloc1 = memMalloc(100);
    void * alloc2 = memMalloc(200);
    void * alloc3 = memMalloc(300);

    memset(alloc1, 1, 100);
    memset(alloc2, 2, 200);
    memset(alloc3, 3, 300);

    for (int i = 0; i < 600; i++) {
        if (i < 100) {
            if (!(*((char *) alloc1+i) == 1)) {
    			printStringLen(13, "alloc1 -- ERROR", 31); 
    			// printStringLen(13, *((char *) alloc1+i), 1); 
				new_line();
				return EXIT_FAILURE;
			}
        } else if (i < 300) {
            if (!(*((char *) alloc1+i) == 2)) {
    			printStringLen(13, "alloc2 -- ERROR", 31); 
				new_line();
				return EXIT_FAILURE;
			}
        } else if (i < 600) {
            if (!(*((char *) alloc1+i) == 3)) {
    			printStringLen(13, "alloc3 -- ERROR", 31); 
				new_line();
				return EXIT_FAILURE;
			}
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

/*
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
*/

#endif
#include "inforeg.h"
#include <stdint.h>
#include "shell.h"
#include "libc.h"

int _getMem(int);

void getMem(int * buffer, int initialAddress) {
    int * bufferAux = (int *) buffer;
    
    for (int i = 0; i < 8; i++) {
        bufferAux[i] = (int) _getMem(initialAddress + (i * 4));
    }
}

// void printmem(long dir) {
void printmem(int argc, char *argv[]) {
	if (argc != 2) {
		printStringLen("printmem receives a memory position\n", 37);
		sys_exit();
	}

	int buffer[8];
	
	getMem(buffer, atoi(argv[1], strlen(argv[1])));
	
	for (int i = 0; i < 8; i++) {
		char bufferAux[8];
		printStringLen("0x", 2);
		printString(itoa(buffer[i], bufferAux, 16));
		new_line();
	}
    sys_exit();
}
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

void printmem(long dir) {
	int buffer[8];
	
	getMem(buffer, dir);
	
	for (int i = 0; i < 8; i++) {
		char bufferAux[8];
		printStringLen("0x", 2);
		printString(itoa(buffer[i], bufferAux, 16));
		new_line();
	}
}
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

void printmem(char * window, int * offset, long dir) {
	int buffer[8];
	
	getMem(buffer, dir);
	
	for (int i = 0; i < 8; i++) {
		char bufferAux[8];
		addText("0x", window, offset);
		addText(itoa(buffer[i], bufferAux, 16), window, offset);
		substractLine(window, offset);
	}
	printWindow(window);
}
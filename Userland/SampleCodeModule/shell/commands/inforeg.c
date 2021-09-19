#include "inforeg.h"
#include <stdint.h>
#include "libc.h"
#include "shell.h"

uint64_t _getRegs();

static char * regsNames[] = {
	"RAX: ", "RBX: ", "RCX: ", "RDX: ", "RBP: ", "RDI: ", "RSI: ", 
	"R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ",
	"RSP: ", "RIP: "};

void inforeg(char * window, int * offset) {
	uint64_t * regs = (uint64_t *) _getRegs();

	char bufferAux[20];
	
	for (int i = 0; i < 17; i++) {
		if (i % 3 == 0 && i != 0)
			substractLine(window, offset);
		addText(regsNames[i], window, offset);
		addText("0x", window, offset);
		addText(gtoa(regs[i], bufferAux, 16, 20), window, offset);
		if (i != 14 && i % 3 != 2)
			addText(" - ", window, offset);
	}
	printWindow(window);
	substractLine(window, offset);
}
#include "inforeg.h"
#include <stdint.h>
#include "libc.h"
#include "shell.h"

uint64_t _getRegs();

static char * regsNames[] = {
	"RAX: ", "RBX: ", "RCX: ", "RDX: ", "RBP: ", "RDI: ", "RSI: ", 
	"R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ",
	"RSP: ", "RIP: "};

void inforeg(int argc, char *argv[]) {
	uint64_t * regs = (uint64_t *) _getRegs();

	char bufferAux[20];
	
	for (int i = 0; i < 17; i++) {
		if (i % 5 == 0 && i != 0)
			new_line();
		printString(regsNames[i]);
		printStringLen("0x", 2);
		printString(gtoa(regs[i], bufferAux, 16, 20));
		if (i != 14 && i % 5 != 4)
			printStringLen(" - ", 3);
	}
	new_line();
	sys_exit();
}
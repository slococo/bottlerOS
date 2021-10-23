#include "lib.h"
#include "time.h"
#include "naiveConsole.h"
#include "pcb.h"
#include "video.h"
#include "keyboard.h"
#include "interrupts.h"

static void * const sampleCodeAddress = (void *) 0x400000;
typedef int (* fn)();

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6

static void zero_division();
static void invalid_opcode();

static uint64_t ripValue;
static uint64_t rspValue;
static uint64_t * stackFrame;

void exceptionDispatcher(int exception, uint64_t rip, uint64_t rsp, uint64_t * rspStackFrame) {
	ripValue = rip;
	rspValue = rsp;
	stackFrame = rspStackFrame;

	switch (exception) {
		case ZERO_EXCEPTION_ID:
			zero_division();
			break;
		case INVALID_OPCODE_ID:
			invalid_opcode();
			break;
		default:
			return;
	}
}

static char * regsNames[] = {
	"RAX: ", "RBX: ", "RCX: ", "RDX: ", "RBP: ", "RDI: ", "RSI: ", 
	"R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: "};

void printRegs() {
	uint64_t * regs = stackFrame;

	// 8 bytes = 64 bits = 2^64
	// máximo: 18.446.744.073.709.551.616 / 2
	// máximo: 9.223.372.036.854.775.808 entonces de 0 a 18
	// dejo el primero para un -
	
	char buffer[20];

	for (int i = 0; i < 15; i++) {
		if (i % 5 == 0)
			new_line();
		printStringLen(15, regsNames[i], 6);
		printStringLen(15, "0x", 3);
		printStringLen(15, itoa(regs[14 - i], buffer, 16, 20), 20);
		printStringLen(15, " - ", 4);
	}
	new_line();
	
	uint64_t * ripValueA = (uint64_t *) ripValue;
	uint64_t * rspValueA = (uint64_t *) rspValue;

	printStringLen(15, "RIP: ", 6);
	printStringLen(15, itoa(*ripValueA, buffer, 16, 20), 20);
	new_line();
	printStringLen(15, "OLD RSP: ", 10);
	printStringLen(15, itoa(*rspValueA, buffer, 16, 20), 20);
	new_line();

    *ripValueA = (uint64_t) sampleCodeAddress;
    *rspValueA = (uint64_t) getSampleRSP();
}

static void startOver() {
	unsigned char key = 0;
	while (key != '\n') {
		_sti();
		haltcpu();
		key = getKeyFromBuffer();
	}

	clear();
	// cleanProcesses();
	moveToWindowVideo(1);
	//((fn)sampleCodeAddress)();
}

static void genericException(char * string, int len) {
	moveToWindowVideo(-1);
	clear();
	printStringLen(15, string, len);
    printRegs();
	startOver();
}

static void zero_division() {
    genericException("Exception 0: Zero division. Press enter to reboot", 50);
}

static void invalid_opcode() {
	genericException("Exception 1: Invalid opcode. Press enter to reboot", 51);
}

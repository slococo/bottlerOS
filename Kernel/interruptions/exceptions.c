// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "exceptions.h"

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
}

static void startOver() {
	long initialTime = getTimeOfDay();
	while (getTimeOfDay() < initialTime + WAITSECONDS);
	clear();

	exitProcess();
	forceTimer();
}

static void genericException(char * string, int len) {
	clear();
	printStringLen(15, string, len);
    printRegs();
	startOver();
}

static void zero_division() {
    genericException("Exception 0: Zero division. Please wait 5 seconds", 50);
}

static void invalid_opcode() {
	genericException("Exception 1: Invalid opcode. Please wait 5 seconds", 51);
}

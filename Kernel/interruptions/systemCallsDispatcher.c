#include <stdint.h>
#include "systemCalls.h"
#include "memManager.h"

void exitProcess();
// void setFn(uint64_t, uint64_t, uint64_t);
char * processes();
void enqueueProcess(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
char openPipe(int *, char *);
char nice(int, char);

uint64_t systemCallsDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	switch (rdi) {
		case 0:
            return write(rsi, rdx, rcx);
        case 1:
            return read(rsi, rdx, rcx);
        case 2:
            return getTime(rsi, rdx, rcx);
        case 3:
            // createProcess(rsi);
            enqueueProcess(rsi, rdx, rcx, r8, r9);
            break;
        case 4:
            exitProcess();
            break;
        case 5:
            // setFn(rsi, rdx, rcx);
            break;
        case 6:
            return (uint64_t) processes();
        case 7:
            return (uint64_t) getSems();
        case 8:
            return pvPortMalloc((void *) rsi);
        case 9:
            vPortFree((void *) rsi);
            break;
        case 10:
            return (uint64_t) openPipe(rsi, rdx);
        case 11:
            return nice(rsi, rdx);
        default:
            return -1;
	}
    return 1;
}
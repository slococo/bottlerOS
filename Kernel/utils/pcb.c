#include "pcb.h"

static const uint8_t * firstProcessAddress = (uint8_t *) 0x18000000;
static const long stackSize = 0x4000000; // 2^26
static const uint8_t * lastProcessAddress = (uint8_t *) 0x10000001; // 2^29 - 1

int activeProcesses = 0, currentProcess = -1;
uint64_t priority0[MAX_PROCESSES];
uint64_t priority1[MAX_PROCESSES];
uint64_t priority2[MAX_PROCESSES];

void cleanProcesses() {
    activeProcesses = 0;
    currentProcess = -1;
}

void newProcess(void (*fn)) {
    if (firstProcessAddress - activeProcesses * stackSize + stackSize <= lastProcessAddress) return;
    _initialize_stack_frame(fn, firstProcessAddress - activeProcesses * stackSize);
}

void newStack(uint64_t rsp) {
    priority0[activeProcesses++] = rsp;
}

uint64_t preserveStack(uint64_t rsp) {
    if (currentProcess != -1) {
        priority0[currentProcess] = rsp;
    }
    if (++currentProcess >= activeProcesses) currentProcess = 0;
    if (activeProcesses == 0) return 0;
    return priority0[currentProcess];
}

static uint64_t sampleRSP;

void saveSampleRSP(uint64_t rsp) {
    sampleRSP = rsp;
}

uint64_t getSampleRSP() {
    return sampleRSP;
}
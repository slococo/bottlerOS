#ifndef PCB_H
#define PCB_H

#include <stdint.h>
#include <defs.h>

#define MAX_PROCESSES 2

uint64_t loadProcess(uint64_t rsp, void (*fn), uint64_t rbp);
uint64_t preserveStack(uint64_t rsp);
void newProcess(void (*fn));
void newStack(uint64_t rsp);
void cleanProcesses();

void * _initialize_stack_frame(void * rip, const void * rsp);

void saveSampleRSP(uint64_t rsp);
uint64_t getSampleRSP();

#endif
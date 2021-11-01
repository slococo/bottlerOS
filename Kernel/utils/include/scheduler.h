#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "memManager.h"
#include <stdlib.h>
#include "../../include/lib.h"
#include "../../drivers/include/time.h"
#include <stddef.h>
#include "schedulerLib.h"

void forceTimer();
void haltcpu();

#define STACK_SIZE 1024 * 512
#define MAX_PRIORITY 40
#define MIN_PRIORITY 0
#define DEF_PRIORITY 0
#define PROCESS_DATA_MAX_SIZE 100
#define MAX_ATTR_SIZE 6
#define MAX_NAME_SIZE 10
#define IDLE_PID 1

typedef struct processCDT * processADT;

void idle();
void newProcess(processADT process, char * name, char priority, char foreground, uint64_t rsp, uint64_t rbp);
uint64_t nextProcess();
char updateRSP(uint64_t newRsp);
char getProcessData(char * out, processADT proc);
processADT searchProcess(processADT * previous, int pid, processADT first);
uint64_t _initialize_stack_frame(void *, void *, int, char **);

#endif
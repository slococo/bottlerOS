#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "memManager.h"
#include "lib.h"

void forceTimer();
void haltcpu();

#define STACK_SIZE 1024 * 512
#define MAX_PRIORITY 40 // Maximum number for a priority
#define MIN_PRIORITY 0 // Minimum number for a priority (yet maximum level of priority) // me gusta
#define DEF_PRIORITY 0
#define PROCESS_DATA_MAX_SIZE 100
#define MAX_ATTR_SIZE 6
#define MAX_NAME_SIZE 10

typedef struct processCDT * processADT;

uint64_t nextProcess();
void idle();
void initScheduler();
void enqueueProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[]);
void newProcess(processADT process, char * name, char priority, char foreground, uint64_t rsp, uint64_t rbp);
char block(int pid);
char unblock(int pid);
char kill(int pid);
void exitProcess();
char nice(char offset);
void updateRSP(uint64_t newRsp);
int getPid();
char quitCPU();
char getProcessData(char * out, processADT proc);
char * processes();

#endif
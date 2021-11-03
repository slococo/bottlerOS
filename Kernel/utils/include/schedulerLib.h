#ifndef SCHEDULERLIB_H
#define SCHEDULERLIB_H

#include <stdint.h>

void initScheduler();
int enqueueProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[], int * fd);
char block(int pid);
char unblock(int pid);
char kill(int pid);
void exitProcess();
char nice(int pid, char offset);
int getPid();
char quitCPU();
char * processes();
int getFdOut();
int getFdIn();
void checkSleeping();
char unblockFirst(int pid);
void sleep(int secs);
char blockIO();
void unblockIO();
char getState(int pid);
char isForeground();
void wait();

#endif
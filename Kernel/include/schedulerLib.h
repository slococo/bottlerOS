#ifndef SCHEDULERLIB_H
#define SCHEDULERLIB_H

#include <stdint.h>

void initScheduler();
void enqueueProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[], int * fd);
char block(int pid);
char unblock(int pid);
char kill(int pid);
void exitProcess();
char nice(char offset);
int getPid();
char quitCPU();
char * processes();
int getFdOut();
int getFdIn();

#endif
#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

uint64_t write(uint64_t, uint64_t, uint64_t);
uint64_t read(uint64_t, uint64_t, uint64_t);
uint64_t getTime(uint64_t, uint64_t, uint64_t);
void createProcess();

#endif
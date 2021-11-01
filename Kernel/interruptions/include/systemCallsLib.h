#ifndef SYSCALLSLIB_H
#define SYSCALLSLIB_H

#include <stdint.h>

uint64_t write(uint64_t, uint64_t, uint64_t);
uint64_t read(uint64_t, uint64_t, uint64_t);

#endif
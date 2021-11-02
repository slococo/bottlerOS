#ifndef TESTMM_H
#define TESTMM_H

#include "test_util.h"
#include "system.h"
#include <stddef.h>
#include "libc.h"

void test_mm(int argc, char *argv[]);

#define MAX_BLOCKS 128
#define MAX_MEMORY 0.8 * 1024 * 1024 * 512 //Should be around 80% of memory managed by the MM

#endif
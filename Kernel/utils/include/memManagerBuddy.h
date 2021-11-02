#ifndef MMBUDDY
#define MMBUDDY

#include "memManager.h"
#include <stddef.h>
#include <stdint.h>

#define NODE_UNUSED 0
#define NODE_USED 1	
#define NODE_SPLIT 2
#define NODE_FULL 3

#define LEVEL 17

#define SIZE (1<<LEVEL)

#endif
#ifndef SEM_H
#define SEM_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "../../include/lib.h"
#include "scheduler.h"
#include "memManager.h"
#include "semLib.h"

#define MAX_SEM 100
#define SEM_DATA_MAX_SIZE 100
#define MAX_PID 4
#define MIN_SIZE 32

void enter_region(uint32_t * lock);
void leave_region(uint32_t * lock);

#endif
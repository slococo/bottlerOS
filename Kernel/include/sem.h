#ifndef SEM_H
#define SEM_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "lib.h"
#include "scheduler.h"
#include "memManager.h"
#include "semLib.h"

#define MAX_SEM 100
#define SEM_DATA_MAX_SIZE 100

void enter_region(uint32_t * lock);
void leave_region(uint32_t * lock);

#endif
#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "lib.h"
#include "scheduler.h"
#include "memManager.h"
#include "pipeLib.h"

#define MAX_SEM 100
#define SEM_NAME "Pipes"

int exists(pipe_t * pipe);
node_t * searchPipe(node_t ** previous, int fd);

#endif
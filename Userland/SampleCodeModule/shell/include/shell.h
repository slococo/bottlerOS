#ifndef SHELL_H
#define SHELL_H

#include "system.h"
#include "libc.h"
#include "help.h"
#include "time.h"
#include "shell.h"
#include "clear.h"
#include "inforeg.h"
#include "printmem.h"
#include "excDiv.h"
#include "excOP.h"
#include "quadratic.h"
#include "cpu_id.h"
#include "ps.h"
#include "pipes.h"
#include "wc.h"
#include "filter.h"
#include "cat.h"
#include "sems.h"
#include "stddef.h"
#include "nice.h"
#include "phylo.h"
#include "kill.h"
#include "block.h"
#include "unblock.h"
#include "loop.h"
#include "test_prio.h"
#include "test_sync.h"
#include "test_processes.h"
#include "test_mm.h"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define SIZE 100
#define MAX_ARGS 11

#define COLS 80
#define ROWS 25

typedef struct cmd_t {
    char * name;
    void (*func) (int argc, char * argv[]);
    char isBuiltIn;
    char isForeground;
} cmd_t;

int scanfNoPrint(char * buffer);
void processInput(char * input);
void shell(int argc, char *argv[]);
void incorrect_comm(char * buffer);
void incorrect_arg(char * command);

#endif
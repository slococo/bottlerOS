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
#include "wc.h"
#include "filter.h"
#include "cat.h"
#include "semCom.h"
#include "stddef.h"
#include "nice.h"
#include "phylo.h"
#include "kill.h"
#include "block.h"
#include "unblock.h"
#include "loop.h"
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
} cmd_t;

cmd_t commands[] = {
    { "help", help, 1 },
    { "cat", cat, 0 },
    { "time", time, 1 },
    { "block", block, 0 },
    { "unblock", unblock, 0 },
    { "inforeg", inforeg, 1 },
    { "excdiv", excdiv, 1 },
    { "excop", excop, 1 },
    { "filter", filter, 0 },
    { "clear", clear, 1 },
    { "cpufeatures", cpufeatures, 1 },
    { "nice", nice, 0 },
    { "ps", ps, 1 },
    { "kill", kill, 1 },
    { "sem", sem, 1 },
    { "quadratic", quadratic, 0 },
    { "printmem", printmem, 0 },
    { "phylo", phylo, 0 },
    { "wc", wc, 0 },
    { "loop", loop, 0 },
    { NULL, NULL, 0}
};

int scanfNoPrint(char * buffer) {
    char c;
    int i = 0;
    while ((c = getChar()) != '\n' && i < SIZE - 1) {
        if (c != -1) {
            if (c == '\b' && i > 0) {
                buffer[--i] = ' ';
                backspace();
            }
            else if (c != 0 && c != '\b') { 
                buffer[i++] = c;
                putChar(c);
            }
        }
    }
    buffer[i] = '\0';
    return i;
}

void processInput(char * input) {
    int comm_flag0 = 0, comm_flag1 = 0, pipe = -1, end = -1;
    char* tokens[SIZE] = {0};
    tokens[0] = strstrip(input, ' ');
    for (int i = 1; i < MAX_ARGS; i++) {
        tokens[i] = strtok(tokens[i - 1], ' ');
        if (i > 1) {
            if (!strcmp(tokens[i - 1], "|"))
                pipe = i - 1;
        }
        if (tokens[i][0] == 0) {
            end = i;
            break;
        }
    }

    int * fd, * fd1, * fd2;
    fd1 = sys_malloc(2 * sizeof(int));
    fd1[0] = 0;
    fd1[1] = 1;

    if (pipe != -1) {
        fd = sys_malloc(2 * sizeof(int));
        if (sys_openPipe(fd, "pipe"))
            return;
        fd2 = sys_malloc(2 * sizeof(int));

        fd1[0] = 0;
        fd1[1] = fd[1];
        fd2[0] = fd[0];
        fd2[1] = 1;
    }
    char ** argv0 = NULL;
    char ** argv1 = NULL;
    int comm0 = -1;
    int comm1 = -1;

    if (pipe != -1) {
        argv0 = sys_malloc(sizeof(char *) * pipe);
        for (int i = 0; i < pipe; i++)
            argv0[i] = tokens[i];
        argv1 = sys_malloc(sizeof(char *) * (end - pipe - 1));
        for (int i = pipe + 1; i < end; i++)
            argv1[i - pipe - 1] = tokens[i];
    }
    else {
        argv0 = sys_malloc(sizeof(char *) * end);
        for (int i = 0; i < end; i++)
            argv0[i] = tokens[i];
    }
    for (int i = 0; commands[i].name != NULL; i++) {
        if (!strcmp(tokens[0], commands[i].name)) {
            comm_flag0 = 1;
            comm0 = i;
            break;
        }
    }
    if (comm_flag0 && pipe != -1) {
        for (int i = 0; commands[i].name != NULL; i++) {
            if (!strcmp(tokens[pipe + 1], commands[i].name)) {
                comm_flag1 = 1;
                comm1 = i;
                break;
            }
        }
    }

    if (comm_flag0 && (comm_flag1 || pipe == -1)) {
        if (pipe != -1) {
            sys_loadProcess(commands[comm0].func, 1, pipe, argv0, fd1);
            sys_loadProcess(commands[comm1].func, 1, end - pipe - 1, argv1, fd2);
        }
        else sys_loadProcess(commands[comm0].func, 1, end, argv0, fd1);
        sys_wait();
    }

    if (!comm_flag0) {
        if (*tokens[0] != 0)
            incorrect_comm(tokens[0]);
    }
    else if (!comm_flag1 && pipe != -1) {
        if (*tokens[pipe + 1] != 0)
            incorrect_comm(tokens[pipe + 1]);
    }
}

void loader(void (*fn) (int, char **), int argc, char * argv[]) {
    fn(argc, argv);
    sys_exit();
}

void shell(int argc, char *argv[]) {
    printStringLen("$> ", 3);
    char buffer[SIZE] = {0};

    while (1) {
        scanfNoPrint(buffer);
        new_line();
        processInput(buffer);
        printStringLen("$> ", 3);
    }
}

void incorrect_comm(char * buffer) {
    printString(buffer);
    printStringLen(" is not a BottlerShell command\n", 32);
}

void incorrect_arg(char * command) {
    printStringLen("Incorrect arguments for command ", 33);
    printString(command);
    new_line();
}
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "libc.h"
#include "shell.h"

void sem(int argc, char *argv[]) {
    char * output = sys_sem();
    printString(output);
    newline();
    sys_free(output);
    addEOF();
    sys_exit();
}
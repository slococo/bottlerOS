// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "libc.h"
#include "shell.h"

void ps(int argc, char *argv[]) {
    char * output = sys_ps();
    printString(output);
    addEOF();
    newline();
    sys_free(output);
    sys_exit();
}
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pipes.h"

void pipe(int argc, char *argv[]) {
    char * output = sys_pipes();
    printString(output);
    newline();
    sys_free(output);
    addEOF();
    sys_exit();
}
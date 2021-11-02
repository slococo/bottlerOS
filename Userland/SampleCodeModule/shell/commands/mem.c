#include "libc.h"
#include "shell.h"

void mem(int argc, char *argv[]) {
    char * output = sys_dumpMM();
    printString(output);
    newline();
    sys_free(output);
    addEOF();
    sys_exit();
}
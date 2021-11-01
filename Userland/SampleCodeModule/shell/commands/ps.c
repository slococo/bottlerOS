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
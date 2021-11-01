#include "libc.h"
#include "shell.h"

void pipes(int argc, char *argv[]) {
    char * output = sys_pipes();
    printString(output);
    newline();
    sys_free(output);
    addEOF();
    sys_exit();
}
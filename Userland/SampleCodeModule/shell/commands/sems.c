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
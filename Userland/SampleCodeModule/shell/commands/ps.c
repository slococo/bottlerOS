#include "libc.h"
#include "shell.h"

void ps(int argc, char *argv[]) {
    char * output = sys_ps();
    printString(output);
    new_line();
    sys_free(output);
}
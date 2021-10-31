#include "libc.h"
#include "shell.h"

void pipes(int argc, char *argv[]) {
    char * output = sys_pipes();
    printString(output);
    new_line();
    sys_free(output);
    sys_exit();
}
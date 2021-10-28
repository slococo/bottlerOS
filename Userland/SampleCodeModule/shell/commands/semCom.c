#include "libc.h"
#include "shell.h"

void sem(int argc, char *argv[]) {
    char * output = sys_sem();
    printString(output);
    new_line();
    sys_free(output);
    sys_exit();
}
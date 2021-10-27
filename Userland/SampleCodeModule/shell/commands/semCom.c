#include "libc.h"
#include "shell.h"

void sem() {
    char * output = sys_sem();
    printString(output);
    new_line();
    // sys_free(output);
}
#include "pipes.h"

void pipe(int argc, char *argv[]) {
    char * output = sys_pipes();
    printString(output);
    newline();
    sys_free(output);
    addEOF();
    sys_exit();
}
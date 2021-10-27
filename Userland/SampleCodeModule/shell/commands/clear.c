#include "libc.h"
#include "shell.h"
#include "clear.h"

void clear(int argc, char *argv[]) {
    winClear();
    sys_exit();
}
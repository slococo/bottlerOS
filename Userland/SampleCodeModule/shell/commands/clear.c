// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "libc.h"
#include "shell.h"
#include "clear.h"

void clear(int argc, char *argv[]) {
    winClear();
    // sys_exit();
    return;
}
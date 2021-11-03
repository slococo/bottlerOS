// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "libc.h"
#include "shell/include/shell.h"
#include "bottler/include/bottler.h"
#include <stddef.h>
#include "system.h"

int main(int argc, char *argv[]) {
    winClear();

    char *argv1[] = {"bottler"};
    sys_loadProcess(bottler, 0, 1, argv1, NULL);
    sys_wait();

    char *argv2[] = {"shell"};
    sys_loadProcess(shell, 1, 1, argv2, NULL);

    sys_exit();
    return 1;
}

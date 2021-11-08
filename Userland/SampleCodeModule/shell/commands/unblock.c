// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "unblock.h"

void unblock(int argc, char ** argv) {
    if (argc != 2) {
        printStringLen("unblock receives a pid\n", 21);
        sys_exit();
    }
    int pid = atoi(argv[1]);

    sys_unblock(pid);
    sys_exit();
}
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "block.h"

void block(int argc, char ** argv) {
    if (argc != 2) {
        printStringLen("block receives a pid\n", 21);
        // sys_exit();
        return;
    }
    int pid = atoi(argv[1]);

    sys_block(pid);
    // sys_exit();
}
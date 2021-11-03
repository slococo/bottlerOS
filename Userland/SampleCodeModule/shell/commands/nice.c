// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "nice.h"

void nice(int argc, char ** argv) {
    if (argc != 3) {
        printStringLen("nice receives a pid and an offset\n", 35);
        sys_exit();
    }
    int pid = atoi(argv[1], MAX_LEN);
    char offset = (char) atoi(argv[2], MAX_LEN);

    sys_nice(pid, offset);
    sys_exit();
}
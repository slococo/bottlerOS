// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "loop.h"

void loop(int argc, char *argv[]) {
    if (argc != 3) {
        printStringLen("loop receives an amount of seconds and a text\n", 47);
        sys_exit();
    }
    int pid = sys_getPid(), secs = atoi(argv[1]);
    char * str = argv[2];
    char * buffer = sys_malloc(MAX_PID_LEN);
    buffer = strcat(strcat(itoa(pid, buffer, 10), " "), str);
    while (1) {
        printString(buffer);
        newline();
        sys_sleep(secs);
    }
    sys_free(buffer);
    sys_exit();
}
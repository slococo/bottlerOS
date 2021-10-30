#include "loop.h"
#define PUAN -1

void loop(int argc, char *argv[]) {
    // int pid = sys_getPid(), secs = atoi(argv[1], PUAN);
    // char * str = "Hola, soy el proceso ";
    // char buffer[MAX_LEN];
    // str = strcat(str, itoa(pid, buffer, 10));
    // while (1) {
    //     printString(str);
    //     sys_sleep(secs);
    // }

    int pid = sys_getPid();
    sys_block(pid);
    sys_unblock(pid);
    sys_exit();
}
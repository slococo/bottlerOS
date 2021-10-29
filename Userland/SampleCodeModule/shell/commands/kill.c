#include "kill.h"

void kill(int argc, char ** argv) {
    if (argc != 2) {
        printStringLen("kill receives a pid\n", 21);
        sys_exit();
    }
    int pid = atoi(argv[1], MAX_LEN);

    sys_kill(pid);
    sys_exit();
}
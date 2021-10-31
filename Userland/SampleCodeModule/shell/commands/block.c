#include "block.h"

void block(int argc, char ** argv) {
    if (argc != 2) {
        printStringLen("block receives a pid\n", 21);
        // sys_exit();
        return;
    }
    int pid = atoi(argv[1], MAX_LEN);

    sys_block(pid);
    // sys_exit();
}
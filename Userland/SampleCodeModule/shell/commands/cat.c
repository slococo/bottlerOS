#include "cat.h"

#define SIZE 1000

void cat(int argc, char ** argv) {
    char c;
    int i = 0;
    char buffer[SIZE] = {0};
    while ((c = getChar()) != 0 && c != -1) {
        if (i >= SIZE)
            break;
        buffer[i++] = c;
    }

    printStringLen(buffer, i);
    sys_exit();
}
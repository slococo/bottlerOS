#include "wc.h"

#define SIZE 1000

void wc(int argc, char ** argv) {
    char c;
    int i = 0;
    char buffer[SIZE] = {0};
    while ((c = getChar()) != 0 && c != -1) {
        if (i >= SIZE)
            break;
        if (c == '\n')
            i++;
    }

    printStringLen(gtoa(i, buffer, 10, SIZE), SIZE);
    sys_exit();
}
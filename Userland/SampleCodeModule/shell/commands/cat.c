// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "cat.h"

#define SIZE 1000

void debug2() {
    return;
}

void cat(int argc, char ** argv) {
    char c;
    int i = 0, j = 0, line = 0;
    char buffer[SIZE] = {0};

    while ((c = getChar()) > 0) {
        if (i > SIZE - 1)
            break;
        buffer[i++] = c;
        j++;
        if (c == '\n') {
            printStringLen(buffer + line, j);
            j = 0;
            line = i;
        }
        if (c == '\b') {
            buffer[--i] = 0;
            buffer[--i] = 0;
        }
    }
    buffer[i++] = c;
    j++;

    debug2();
    printStringLen(buffer + line, j);

    newline();
    sys_exit();
}
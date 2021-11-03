// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "wc.h"

#define SIZE 1000

void wc(int argc, char ** argv) {
    char c;
    int i = 0;
    char buffer[SIZE] = {0};
    while ((c = getChar()) > 0) {
        if (i >= SIZE)
            break;
        if (c == '\n')
            i++;
    }
    buffer[i++] = c;

    printStringLen(gtoa(i, buffer, 10, SIZE), SIZE);
    newline();
    sys_exit();
}
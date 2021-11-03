// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "filter.h"

#define SIZE 1000

int isVocal(char c) {
    return c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

void filter(int argc, char ** argv) {
    char c;
    int i = 0, j = 0, line = 0;
    char buffer[SIZE] = {0};
    while ((c = getChar()) > 0) {
        if (i >= SIZE)
            break;
        if (!isVocal(c)) {
            buffer[i++] = c; 
            j++;
        }
        if (c == '\b') {
            buffer[--i] = 0;
            if (isVocal(buffer[i-1]))
                buffer[--i] = 0;
        }
        if (c == '\n') {
            printStringLen(buffer + line, j);
            j = 0;
            line = i;
        }
    }
    buffer[i++] = c;
    j++;

    printStringLen(buffer + line, j);

    newline();
    sys_exit();
}
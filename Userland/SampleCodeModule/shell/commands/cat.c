#include "cat.h"

#define SIZE 1000

/*
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
*/

void cat(int argc, char ** argv) {
    char c;
    int i = 0, j = 0, line = 0;
    char buffer[SIZE] = {0};

    while ((c = getChar()) <= 0) {
        if (i >= SIZE)
            break;
        if (c == '\n') {
            printStringLen(buffer[line], j);
            j = 0;
            line = i;
        }
        buffer[i++] = c;
        j++;
    }

    printStringLen(buffer, i);
    sys_exit();
}
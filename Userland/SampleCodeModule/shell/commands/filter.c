#include "filter.h"

#define SIZE 1000

int isVocal(char c) {
    return c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

void filter(int argc, char ** argv) {
    char c;
    int i = 0;
    char buffer[SIZE] = {0};
    while ((c = getChar()) != 0 && c != -1) {
        if (i >= SIZE)
            break;
        if (isVocal(c))
            buffer[i++] = c;
    }

    printStringLen(buffer, i);
    sys_exit();
}
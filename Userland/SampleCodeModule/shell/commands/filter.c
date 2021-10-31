#include "filter.h"

#define SIZE 1000

int isVocal(char c) {
    return c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

void debug8()
{
    return;
}

void debug9()
{
    return;
}

void filter(int argc, char ** argv) {
    char c;
    int i = 0;
    char buffer[SIZE] = {0};
    debug9();
    while ((c = getChar()) != 0 && c != -1) {
        debug8();
        if (i >= SIZE)
            break;
        if (isVocal(c))
            buffer[i++] = c;
    }

    printStringLen(buffer, i);
    new_line();
    sys_exit();
}
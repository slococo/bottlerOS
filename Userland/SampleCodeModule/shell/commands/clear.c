#include "libc.h"
#include "shell.h"
#include "clear.h"

void clear(char * window, int * offset) {
    clearWindow(window, offset);
}
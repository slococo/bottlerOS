#include "libc.h"
#include "shell.h"

void ps(char * window, int * offset) {
    char * output = sys_ps();
    addText(output, window, offset);
    printWindow(window);
    substractLine(window, offset);
    sys_free(output);
}
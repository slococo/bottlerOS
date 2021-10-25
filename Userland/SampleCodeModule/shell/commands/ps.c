#include "libc.h"
#include "shell.h"

char * sys_ps();

void ps(char * window, int * offset) {
    char * output = sys_ps();
    addText(output, window, offset);
    printWindow(window);
    substractLine(window, offset);
}
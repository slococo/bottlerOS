#include "libc.h"
#include "shell.h"

void sem(char * window, int * offset) {
    char * output = sys_sem();
    addText(output, window, offset);
    printWindow(window);
    substractLine(window, offset);
    sys_free(output);
}
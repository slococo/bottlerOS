#include "libc.h"
#include "shell.h"

static char * info[] = {"clear: Clear the shell window", "cpufeatures: Show the features the cpu supports", "excdiv: Throw exception 0 (zero division)", "excop: Throw exception 6 (opcode error)", "help: Print information about commands", "inforeg: Print values of registers","printmem: Prints 32 bytes of memory, starting at a given direction", "quadratic: Receives 3 floats, quadratic coefficients, prints roots if real", "time: Prints date and time in UTC" };
static const int len = 9;

void help(int argc, char *argv[]) {
    for (int i = 0; i < len; i++) {
        printString(info[i]);
        newline();
    }
    addEOF();
    sys_exit();
}
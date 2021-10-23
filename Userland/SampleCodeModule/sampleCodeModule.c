/* sampleCodeModule.c */
#include "libc.h"
#include "shell/include/shell.h"

void sys_loadProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[]);
// void sys_switchContext();

int main() {
    winClear();

    char * argv[] = {"BottlerSh"};
    sys_loadProcess(shell, 1, 1, argv);
    // sys_loadProcess(shell);

    // sys_switchContext();

	return 1; 
}

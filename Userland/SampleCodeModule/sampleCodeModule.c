/* sampleCodeModule.c */
#include "libc.h"
#include "shell/include/shell.h"

void sys_loadProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[]);
void sys_exit();
// void sys_switchContext();

int main(int argc, char *argv[]) {
    winClear();

    char * argv2[] = {"BottlerSh"};
    sys_loadProcess(shell, 1, 1, argv2);
    // sys_loadProcess(shell);

    // sys_switchContext();
    sys_exit();
	return 1; 
}

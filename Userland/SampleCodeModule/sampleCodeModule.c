/* sampleCodeModule.c */
#include "libc.h"
#include "shell/include/shell.h"
#include "bottler/include/bottler.h"
#include <stddef.h>

int sys_loadProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[], int * fd);
void sys_exit();
// void sys_switchContext();

int main(int argc, char *argv[]) {
    winClear();

    char * argv1[] = {"bottler"};
    sys_loadProcess(bottler, 1, 1, argv1, NULL);
    sys_sleep(4);
    // winClear();

    char * argv2[] = {"shell"};
    sys_loadProcess(shell, 1, 1, argv2, NULL);
    // sys_loadProcess(shell);

    // sys_switchContext();
    sys_exit();
	return 1;
}

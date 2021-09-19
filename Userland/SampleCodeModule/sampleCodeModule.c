/* sampleCodeModule.c */
#include "libc.h"
#include "shell/include/shell.h"

void sys_loadProcess(void (*fn));
void sys_switchContext();

int main() {
    winClear();

    sys_loadProcess(shell);
    sys_loadProcess(shell);

    sys_switchContext();

	return 1; 
}

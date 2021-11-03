// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "libc.h"
#include "shell.h"

#define PAGE_SIZE 10

static char * info[] = {"block: Block a process given the pid", "cat: Prints out the text it receives", "clear: Clears the window", "cpufeatures: Show the features the cpu supports", "excdiv: Throw exception 0 (zero division)", "excop: Throw exception 6 (opcode error)", "filter: Prints out the text it receives ignoring vocals", "help: Print information about commands", "inforeg: Print values of registers", "kill: Kills a process given the pid", "loop: Prints it's pid and a given message every given amount of seconds", "nice: Changes the priority of a given process (min: -20, max: 19)", "phylo: Shows a solution for the philosopher problem", "pipes: Prints the dump of the used pipes and their state", "printmem: Prints 32 bytes of memory, starting at a given direction", "ps: Prints information of the running processes", "quadratic: Receives 3 floats, quadratic coefficients, prints roots if real", "sems: Prints the dump of used semaphores", "test_mm: Runs memory manager test", "test_no_sync: Runs synchronization test without semaphores", "test_prio: Runs process priority test", "test_processes: Runs process creation test", "test_sync: Runs synchronization test with semaphores", "time: Prints date and time in UTC", "unblock: Unblocks a process given it's pid", "wc: Prints out amount of lines in the received text", 0 };

int getCurrentPages() {
    int i = 0;
    for (; info[i] != 0; i++);
    return ((i + 1) / PAGE_SIZE) + 1;
}

void help(int argc, char *argv[]) {
    if (argc != 2) {
        printStringLen("help receives a number of page between 1 and ", 46);
        char buffer[10] = {0};
        printString(itoa(getCurrentPages(), buffer, 10));
        newline();
        sys_exit();
    }

    int page = atoi(argv[1], 10);
    int pages = getCurrentPages();

    if (page < 1 || page > pages) {
        if (pages > 1) {
            printStringLen("page number must be between 1 and ", 34);
            char buffer[10] = {0};
            printString(itoa(pages, buffer, 10));
        }
        else
            printStringLen("help only has one page (page = 1)\n", 35);
        newline();
    }

    for (int i = (page - 1) * PAGE_SIZE; info[i] != 0 && i < page * PAGE_SIZE; i++) {
        printString(info[i]);
        newline();
    }
    addEOF();
    sys_exit();
}
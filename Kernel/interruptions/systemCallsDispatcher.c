// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "systemCallsDispatcher.h"

uint64_t systemCallsDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
    switch (rdi) {
        case 0:
            return write(rsi, rdx, rcx);
        case 1:
            return read(rsi, rdx, rcx);
        case 2:
            return getTime(rsi);
        case 3:
            return enqueueProcess((void (*)(int, char **)) rsi, rdx, rcx, (char **) r8, (int *) r9);
        case 4:
            exitProcess();
            break;
        case 5:
            return (uint64_t) dumpMM();
        case 6:
            return (uint64_t) processes();
        case 7:
            return (uint64_t) getSems();
        case 8:
            return (uint64_t) pvPortMalloc((size_t) rsi);
        case 9:
            vPortFree((void *) rsi);
            break;
        case 10:
            return (uint64_t) openPipe((int *) rsi, (char *) rdx);
        case 11:
            return nice(rsi, rdx);
        case 12:
            sleep(rsi);
            break;
        case 13:
            semWait((sem_t *) rsi);
            break;
        case 14:
            semPost((sem_t *) rsi);
            break;
        case 15:
            return (uint64_t) semOpen((char *) rsi, rdx);
        case 16:
            kill(rsi);
            break;
        case 17:
            return getPid();
        case 18:
            return semClose((sem_t *) rsi);
        case 19:
            return block(rsi);
        case 20:
            return unblock(rsi);
        case 21:
            wait();
            break;
        case 22:
            return (uint64_t) pipes();
        case 23:
            return quitCPU();
        case 24:
            closePipe(rsi);
            break;
        default:
            return -1;
    }
    return 1;
}
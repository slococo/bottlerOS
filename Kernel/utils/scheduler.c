#include "scheduler.h"

enum states {
    READY = 0, DEAD, BLOCKED, BBCHILDREN, WAITING, BLOCKEDIO
};

typedef struct processCDT {
    struct processCDT *next;
    char *name;
    int pid;
    int ppid;
    uint64_t rsp;
    uint64_t rbp;
    char priority;
    char executions;
    char foreground;
    enum states state;
    int *fd;
    int children;
    char backWait;
} processCDT;

typedef struct sleepCDT {
    int pid;
    long time;
    long secs;
    struct sleepCDT *next;
} sleepCDT;

processCDT *firstBlockedIteration = NULL;

processCDT *firstProcess = NULL;
processCDT *lastProcess = NULL;
sleepCDT *firstSleep = NULL;

static processCDT *currentProcess = NULL;
static int pids = IDLE_PID;
static char update = 1;
static char idleFlag = 2;

void removeProcess(processCDT *del, processCDT **first, processCDT **last) {
    processCDT *prev = NULL;
    del = searchProcess(&prev, del->pid, *first);

    if (prev == NULL) {
        *first = del->next;
        if (*last == del)
            *last = NULL;
    } else {
        prev->next = del->next;
        if (*last == del)
            *last = prev;
    }
}

uint64_t nextProcess(uint64_t currentRSP) {
    if (currentProcess != NULL)
        currentProcess->rsp = currentRSP;
    processCDT *prev = currentProcess;
    if (currentProcess != NULL && currentProcess->state == READY &&
        currentProcess->executions == MAX_PRIORITY - currentProcess->priority) {
        currentProcess->executions = 0;
        currentProcess = currentProcess->next;
    }
    while (currentProcess == NULL || currentProcess->state == BLOCKED || currentProcess->state == DEAD ||
           currentProcess->state == WAITING || currentProcess->state == BLOCKEDIO) {
        if (currentProcess == NULL) {
            currentProcess = firstProcess;
        } else if (currentProcess == firstBlockedIteration) {
            idleFlag = 1;
            unblock(IDLE_PID);
            prev = currentProcess;
            currentProcess = currentProcess->next;
        } else if (currentProcess->state == DEAD) {
            processCDT *del = currentProcess;
            currentProcess = currentProcess->next;
            removeProcess(del, &firstProcess, &lastProcess);
            vPortFree((void *) del);
        } else if (currentProcess->state == BLOCKED || currentProcess->state == WAITING ||
                   currentProcess->state == BLOCKEDIO) {
            if (firstBlockedIteration == NULL)
                firstBlockedIteration = currentProcess;
            prev = currentProcess;
            currentProcess = currentProcess->next;
        }
    }
    if (currentProcess->pid != IDLE_PID && idleFlag) {
        idleFlag = 0;
        block(IDLE_PID);
    }
    firstBlockedIteration = NULL;
    currentProcess->executions++;
    return currentProcess->rsp;
}

void idle() {
    while (1) {
        haltcpu();
    }
}

void initScheduler() {
    char *argv[] = {"idle"};
    nice(enqueueProcess(idle, 0, 1, argv, NULL), 19);
}

int enqueueProcess(void (*fn)(int, char **), char foreground, int argc, char *argv[], int *fd) {
    if (fd == NULL) {
        int *aux = pvPortMalloc(2 * sizeof(int));
        aux[0] = 0;
        aux[1] = 1;
        fd = aux;
    }
    if (!idleFlag)
        block(IDLE_PID);

    if (currentProcess != NULL) {
        currentProcess->children++;
        if (currentProcess->foreground && foreground) {
            currentProcess->foreground = 0;
            currentProcess->backWait = 1;
        }
    }

    processADT process = pvPortMalloc(sizeof(processCDT));
    uint64_t *auxi = pvPortMalloc(STACK_SIZE);
    uint64_t *rbp = STACK_SIZE + auxi;
    uint64_t *rsp = rbp - 20;

    char priority = (foreground == 1) ? DEF_PRIORITY : MAX_PRIORITY / 2;

    char *aux = pvPortMalloc(10);
    int j;
    for (j = 0; j < MAX_NAME_SIZE - 1 && argv[0][j] != 0; j++) {
        aux[j] = argv[0][j];
    }
    aux[j] = '\0';

    process->name = aux;
    process->pid = pids++;
    process->ppid = currentProcess->pid;
    process->priority = priority;
    process->rsp = (uint64_t) rsp;
    process->rbp = (uint64_t) rbp;
    process->executions = 0;
    process->foreground = foreground;
    process->state = READY;
    process->fd = fd;
    process->next = NULL;
    process->children = 0;
    process->backWait = 0;

    process->rsp = _initialize_stack_frame(fn, rbp, argc, argv);

    if (firstProcess == NULL)
        firstProcess = process;
    else
        lastProcess->next = process;
    lastProcess = process;

    return process->pid;
}

void sleep(int secs) {
    if (currentProcess == NULL)
        return;

    sleepCDT *proc = pvPortMalloc(sizeof(sleepCDT));
    proc->pid = currentProcess->pid;
    proc->secs = secs;
    proc->time = getTimeOfDay();
    proc->next = firstSleep;
    firstSleep = proc;

    block(currentProcess->pid);
}

void wakeUp(sleepCDT *wake, sleepCDT *prev) {
    if (wake == firstSleep)
        firstSleep = wake->next;
    else {
        prev->next = wake->next;
    }
    unblock(wake->pid);
    vPortFree(wake);
}

void checkSleeping() {
    sleepCDT *prev = NULL;
    sleepCDT *aux = firstSleep;
    while (aux != NULL) {
        if (getTimeOfDay() >= aux->time + aux->secs) {
            wakeUp(aux, prev);
            aux = prev->next;
        } else {
            prev = aux;
            aux = aux->next;
        }
    }
}

processADT searchProcess(processADT *previous, int pid, processADT first) {
    processADT curr = first;
    *previous = NULL;
    while (curr != NULL) {
        if (curr->pid == pid) {
            break;
        }
        *previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        *previous = NULL;
        return NULL;
    }
    return curr;
}

void unblockIO() {
    processCDT *aux = firstProcess;
    while (aux != NULL) {
        if (aux->state == BLOCKEDIO)
            aux->state = READY;

        aux = aux->next;
    }
}

char blockIO() {
    if (currentProcess == NULL || currentProcess->state == DEAD)
        return EXIT_FAILURE;
    
    currentProcess->state = BLOCKEDIO;
    currentProcess->executions = 0;
    forceTimer();

    return EXIT_SUCCESS;
}

char block(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstProcess);
    if (del == NULL || del->state == DEAD)
        return EXIT_FAILURE;
    else {
        del->state = BLOCKED;
    }

    processADT parent = searchProcess(&prev, del->ppid, firstProcess);
    if (del->foreground) {
        if (parent->backWait) {
            parent->backWait = 0;
            parent->foreground = 1;
        }
    }
    if (pid == currentProcess->pid) {
        currentProcess->executions = 0;
        forceTimer();
    }

    return EXIT_SUCCESS;
}

char unblock(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstProcess);
    if (del == NULL || del->state == DEAD) {
        return EXIT_FAILURE;
    }
    
    del->state = READY;

    processADT parent = searchProcess(&prev, del->ppid, firstProcess);
    if (del->foreground) {
        if (parent->foreground) {
            parent->backWait = 1;
            parent->foreground = 0;
        }
    }
    return EXIT_SUCCESS;
}

char unblockFirst(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstProcess);
    if (del == NULL || del->state == DEAD) {
        return EXIT_FAILURE;
    }

    removeProcess(del, &firstProcess, &lastProcess);
    if (currentProcess != NULL) {
        currentProcess->executions = MAX_PRIORITY - currentProcess->priority;
        del->next = currentProcess->next;
        currentProcess->next = del;
        if (currentProcess == lastProcess)
            lastProcess = del;
    }
    else {
        if (firstProcess == NULL) {
            del->next = NULL;
            firstProcess = del;
            lastProcess = del;
        }
        else {
            del->next = firstProcess->next;
            firstProcess = del;
        }
    }

    del->state = READY;
    
    processADT parent = searchProcess(&prev, del->ppid, firstProcess);
    if (del->foreground) {
        if (parent->foreground) {
            parent->backWait = 1;
            parent->foreground = 0;
        }
    }
    forceTimer();
    return EXIT_SUCCESS;
}

char kill(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstProcess);
    if (del == NULL) {
        return EXIT_FAILURE;
    }
    processADT parent = searchProcess(&prev, del->ppid, firstProcess);

    if (parent != NULL) {
        parent->children--;
        if (!parent->children && parent->state == WAITING) {
            parent->state = READY;
        }
        if (parent->backWait && del->foreground) {
            parent->backWait = 0;
            parent->foreground = 1;
        }
    }

    vPortFree(del->fd);
    vPortFree(del->name);
    void *auxi = del->rbp - STACK_SIZE;
    vPortFree((void *) auxi);
    del->state = DEAD;

    if (pid == currentProcess->pid) {
        forceTimer();
    }

    return EXIT_SUCCESS;
}

int getFdOut() {
    if (currentProcess == NULL)
        return EXIT_FAILURE;
    return currentProcess->fd[1];
}

int getFdIn() {
    if (currentProcess == NULL)
        return EXIT_FAILURE;
    return currentProcess->fd[0];
}

void exitProcess() {
    kill(currentProcess->pid);
}

char nice(int pid, char offset) {
    if (offset >= 20 || offset < -20)
        return EXIT_FAILURE;

    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstProcess);
    if (del == NULL) {
        return EXIT_FAILURE;
    } else {
        del->priority = offset + 20;
    }
    return EXIT_SUCCESS;
}

void wait() {
    if (currentProcess != NULL && currentProcess->children != 0) {
        currentProcess->state = WAITING;
        forceTimer();
    }
}

char updateRSP(uint64_t newRsp) {
    if (currentProcess == NULL) {
        return -1;
    }
    if (update)
        currentProcess->rsp = newRsp;
    return update;
}

int getPid() {
    if (currentProcess == NULL)
        return -1;
    return currentProcess->pid;
}

char getState(int pid){
    processCDT * aux = firstProcess;
    while (aux != NULL) {
        if (aux->pid == pid){
            break;
        }
        aux = aux -> next;
    }
    return aux == NULL ? -1 : aux->state;
}

char quitCPU() {
    int pid = getPid();
    if (pid == -1)
        return EXIT_FAILURE;
    // return block(pid);
    currentProcess->executions = MAX_PRIORITY - currentProcess->priority;
    forceTimer();
    return EXIT_SUCCESS;
}

char isForeground() {
    if (currentProcess == NULL)
        return -1;
    return currentProcess->foreground;
}

void getGenProcessData(char **out, char *written, char toAdd, char *in, char isLast) {
    char copied = strcpy(*out, in);
    *out += copied;
    *out += addSpaces(*out, toAdd - copied);
    *written += toAdd;
    if (!isLast) {
        *out += addSpaces(*out, 2);
        *written += 2;
    }
}

char getProcessData(char *out, processCDT *proc) {
    if (proc == NULL)
        return EXIT_FAILURE;

    char written = 0;

    char flag = 0;
    for (int j = 0; j < MAX_NAME_SIZE; j++) {
        if (!flag && proc->name[j] == 0)
            flag = 1;
        else if (flag)
            out += addSpaces(out, 1);
        else
            *out++ = proc->name[j];
    }
    written += MAX_NAME_SIZE;

    out += addSpaces(out, 2);
    written += 2;

    char buffer[10];
    getGenProcessData(&out, &written, MAX_ATTR_SIZE, itoa(proc->pid, buffer, 10, 10), 0);
    getGenProcessData(&out, &written, MAX_ATTR_SIZE, itoa(proc->priority, buffer, 10, 2), 0);
    getGenProcessData(&out, &written, MAX_NAME_SIZE, itoa(proc->rsp, buffer, 16, 10), 0);
    getGenProcessData(&out, &written, MAX_NAME_SIZE, itoa(proc->rbp, buffer, 16, 10), 0);
    getGenProcessData(&out, &written, MAX_ATTR_SIZE, (proc->foreground == 1) ? "F" : "B", 0);
    getGenProcessData(&out, &written, MAX_ATTR_SIZE,
                      proc->state == BLOCKED ? "Block" : proc->state == DEAD ? "Killed" : "Ready", 1);

    return written;
}

char *processes() {
    char *ans = pvPortMalloc(pids * PROCESS_DATA_MAX_SIZE);
    char *ret = ans;

    char *info = "name       pid     prio    rsp         rbp         fore    state\n";
    ans += strcpy(ans, info);

    processCDT *aux = firstProcess;
    while (aux != NULL) {
        char writtenChars = getProcessData(ans, aux);
        if (writtenChars == EXIT_FAILURE)
            return NULL;
        ans += writtenChars - 1;
        *ans++ = '\n';
        aux = aux->next;
    }
    *--ans = 0;

    return ret;
}
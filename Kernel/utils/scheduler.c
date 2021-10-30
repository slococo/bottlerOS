#include "scheduler.h"
#define IDLE_PID 1

// void _initialize_stack_frame(void *, void *, int, char**, void **, void **);
// void _initialize_stack_frame(void *, void *, int, char**, void *, void *);
uint64_t * _initialize_stack_frame(void *, void *, int, char**);

enum states {READY = 0, DEAD, BLOCKED};

typedef struct processCDT {
    struct processCDT * next;
    char * name;
    int pid;
    int ppid;
    uint64_t rsp;
    uint64_t rbp;
    char priority;
    char executions;
    char foreground;
    enum states state;
    int * fd;
} processCDT;

typedef struct sleepCDT {
    int pid;
    long time;
    long secs;
    struct sleepCDT * next;
} sleepCDT;

processCDT * firstBlockedIteration = NULL;

processCDT * firstProcess = NULL;
processCDT * lastProcess = NULL;
// processCDT * firstBlocked = NULL;
// processCDT * lastBlocked = NULL;
sleepCDT * firstSleep = NULL;

static processCDT * currentProcess = NULL;
static int pids = IDLE_PID;
static char update = 1;
static char idleFlag = 2;

void debug() {
    return;
}

void removeProcess(processCDT * del, processCDT * prev, processCDT ** first, processCDT ** last) {
    if (prev == NULL) {
        *first = del->next;
        if (*last == del)
            *last = NULL;
    }
    else {
        prev->next = del->next;
        if (*last == del)
            *last = prev;
    }
}

uint64_t nextProcess(uint64_t currentRSP) {
    if (currentProcess != NULL /*&& currentProcess->state != BLOCKED*/)
        currentProcess->rsp = currentRSP;
    processCDT * prev = currentProcess;
    if (currentProcess != NULL)
        currentProcess = currentProcess->next;    
    while (currentProcess == NULL || currentProcess->state == BLOCKED || currentProcess->state == DEAD) {
        if (currentProcess == NULL) {
            currentProcess = firstProcess;
        }
        else if (currentProcess == firstBlockedIteration) {
            idleFlag = 2;
            unblock(IDLE_PID);
            prev = currentProcess;
            currentProcess = currentProcess->next;
        }
        else if (currentProcess->state == DEAD) {
            debug();
            processCDT * del = currentProcess;
            currentProcess = currentProcess->next;
            removeProcess(del, prev, &firstProcess, &lastProcess);
            vPortFree((void *) del);
        }
        else if (currentProcess->state == BLOCKED) {
            if (firstBlockedIteration == NULL)
                firstBlockedIteration = currentProcess;
            prev = currentProcess;
            currentProcess = currentProcess->next;
        }
        else if (currentProcess->state == READY && currentProcess->executions == MAX_PRIORITY - currentProcess->priority + 1) {
            currentProcess->executions = 0;
            prev = currentProcess;
            currentProcess = currentProcess->next;
        }
    }
    if (currentProcess->pid != IDLE_PID)
        block(IDLE_PID);
    firstBlockedIteration = NULL;
    currentProcess->executions++;
    return currentProcess->rsp;
}

/*
uint64_t nextProcess(uint64_t currentRSP) {
    if (currentProcess == firstBlockedIteration) {
        idleFlag = 2;
        unblock(IDLE_PID);
    }
    update = 1;
    if (currentProcess == NULL) {
        currentProcess = firstProcess;
        return currentProcess->rsp;
    }
    else if (currentProcess->state == DEAD) {
        processCDT * del = currentProcess;
        if (currentProcess->next != NULL) {
            currentProcess = currentProcess->next;
        }
        else currentProcess = firstProcess;
        processCDT * prev = NULL;
        removeProcess(del, prev, &firstProcess, &lastProcess);
        vPortFree((void *) del);
        return currentProcess->rsp;
    }
    currentProcess->rsp = currentRSP;
    if (currentProcess->state == BLOCKED) {
        if (firstBlockedIteration == NULL)
            firstBlockedIteration = currentProcess;
        processCDT * del = currentProcess;
        if (currentProcess->next != NULL) {
            currentProcess = currentProcess->next;
        }
        else if (firstProcess == NULL) {
            idleFlag = 2;
            unblock(IDLE_PID);
        }
        else currentProcess = firstProcess;
        return currentProcess->rsp;
    }
    firstBlockedIteration = NULL;
    if (currentProcess->executions < MAX_PRIORITY - currentProcess->priority + 1) {
        currentProcess->executions++;
        return currentProcess->rsp;
    }
    currentProcess->executions = 0;
    if (currentProcess->next != NULL)
        currentProcess = currentProcess->next;
    else {
        currentProcess = firstProcess;
    }
    return currentProcess->rsp;
}
*/

void idle() {
    while (1) {
        haltcpu();
    }
}

void initScheduler() {
    char * argv[] = {"idle"};
    nice(enqueueProcess(idle, 0, 1, argv, NULL), 19);
}

int enqueueProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[], int * fd) {
    if (fd == NULL) {
        int * aux = pvPortMalloc(2 * sizeof(int));
        aux[0] = 0;
        aux[1] = 1;
        fd = aux;
    }
    if (!idleFlag)
        block(IDLE_PID);

    processADT process = pvPortMalloc(sizeof(processCDT));
    uint64_t * auxi = pvPortMalloc(STACK_SIZE);
    uint64_t * rbp = STACK_SIZE + auxi;
    uint64_t * rsp = rbp - 20; // 20 //22
    
    char priority = (foreground == 1) ? DEF_PRIORITY : MAX_PRIORITY/2;

    // char aux[MAX_NAME_SIZE];
    char * aux = pvPortMalloc(10);
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

    process->rsp = _initialize_stack_frame(fn, rbp, argc, argv);
    // _initialize_stack_frame(fn, rbp, argc, argv);

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

    sleepCDT * proc = pvPortMalloc(sizeof(sleepCDT));
    proc->pid = currentProcess->pid;
    proc->secs = secs;
    proc->time = getTimeOfDay();
    proc->next = firstSleep;
    firstSleep = proc;

    block(currentProcess->pid);
}

void wakeUp(sleepCDT * wake, sleepCDT * prev) {
    if (wake == firstSleep)
        firstSleep = wake->next;
    else {
        prev->next = wake->next;
    }
    unblock(wake->pid);
    vPortFree(wake);
}

void checkSleeping() {
    sleepCDT * prev = NULL;
    sleepCDT * aux = firstSleep;
    while(aux != NULL) {
        if (getTimeOfDay() >= aux->time + aux->secs) {
            wakeUp(aux, prev);
            aux = prev->next;
        }
        else {
            prev = aux;
            aux = aux->next;
        }
    }
}

// void * getSSEaddress() {
// //     return currentProcess->sseBytes;
//     return currentProcess->bytes->s.sseBytes;
// }

// void * getFPUaddress() {
//     // return currentProcess->fpuBytes;
//     return currentProcess->bytes->s.fpuBytes;
// }

void newProcess(processADT process, char * name, char priority, char foreground, uint64_t rsp, uint64_t rbp) {
}

processADT searchProcess(processADT * previous, int pid, processADT first) {
    processADT curr = first;
    * previous = NULL;
    while (curr != NULL) {
        if (curr->pid == pid) {
            break;
        }
        * previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        * previous = NULL;
        return NULL;
    }
    return curr;
}

char block(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstProcess);
    if (del == NULL)
        return EXIT_FAILURE;
    else {
        // removeProcess(del, prev, &firstReady, &lastReady);
        del->state = BLOCKED;
        //blockProcess(del, prev);
//        if (prev != NULL) {
//            prev->next = del->next;
//            if (lastReady == del)
//                lastReady = prev;
//        }
//        else {
//            firstReady = del->next;
//            if (del == lastReady)
//                lastReady = NULL;
//        }
    }

    // processCDT * next = del->next;
    // del->next = NULL;

    // del->state = BLOCKED;
    // if (lastBlocked != NULL)
    //     lastBlocked->next = del;
    // else
    //     firstBlocked = del;
    // lastBlocked = del;

    // processCDT * auxCurr = pvPortMalloc(sizeof(processCDT));
    // auxCurr->next = next;
    // auxCurr->pid = pid;
    // currentProcess->state = BLOCKED;

    if (pid == currentProcess->pid) {
        // update = 0;
        // currentProcess = next;
        
        forceTimer();
    }

    return EXIT_SUCCESS; 
}

// void debug() {
//     return;
// }

char unblock(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstProcess);
    if (del == NULL) {
        // debug();
        return EXIT_FAILURE;
    }
    else {
        // removeProcess(del, prev, &firstBlocked, &lastBlocked);
        del->state = READY;
//        if (prev != NULL) {
//            prev->next = del->next;
//            if (del == lastBlocked)
//                lastBlocked = prev;
//        }
//        else {
//            firstBlocked = del->next;
//            if (lastBlocked == del)
//                lastBlocked = NULL;
//        }
    }

    // del->next = NULL;
    // if (lastReady != NULL)
    //     lastReady->next = del;
    // else
    //     firstReady = del;
    // lastReady = del;
// 
    // del->state = READY;
    // if (firstReady != NULL && firstReady->pid == IDLE_PID && lastReady->pid != IDLE_PID)
    if (idleFlag && !(--idleFlag))
        block(IDLE_PID);
    
    return EXIT_SUCCESS;
}

/*
char unblockFirst(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstBlocked);
    if (del == NULL)
        return EXIT_FAILURE;
    else {
        // removeProcess(del, prev, &firstBlocked, &lastBlocked);
        
//        if (prev != NULL) {
//            prev->next = del->next;
//            if (lastBlocked == del)
//                lastBlocked = prev;
//        }
//        else {
//            firstBlocked = del->next;
//            if (lastBlocked == del)
//                lastBlocked = NULL;
//        }
    }
    
    if (currentProcess != NULL) {
        del->next = currentProcess->next;
        currentProcess->next = del;
        if (lastReady == currentProcess)
            lastReady = del;
    }
    else {
        if (firstReady != NULL)
            del->next = firstReady->next;
        else del->next = NULL;
        firstReady = del;
        if (lastReady == NULL)
            lastReady = del;
    }

    del->state = READY;

    // if (firstReady != NULL && lastReady->pid == IDLE_PID && firstReady->pid != IDLE_PID)
    if (idleFlag && !(--idleFlag))
        block(IDLE_PID);
    
    return EXIT_SUCCESS;
}
*/

char kill(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstProcess);
    if (del == NULL) {
    //    del = searchProcess(&prev, pid, firstBlocked);
    //    if (del == NULL)
            return EXIT_FAILURE;
    //     else {
    //         removeProcess(del, prev, &firstBlocked, &lastBlocked);
    //         // if (prev != NULL) {
    //         //     prev->next = del->next;
    //         //     if (del == lastBlocked)
    //         //         lastBlocked = prev;
    //         // }
    //         // else
    //         //     firstBlocked = del->next;
    //     }
    }
    // else {
        // removeProcess(del, prev, &firstReady, &lastReady);
        // if (prev != NULL) {
        //     prev->next = del->next;
        //     if (del == lastReady)
        //         lastReady = prev;
        // }
        // else
        //     firstReady = del->next;
    // }

    // processCDT * next = del->next;

    vPortFree(del->fd);
    vPortFree((void *) del->rbp - STACK_SIZE);
    currentProcess->state = DEAD;

    if (pid == currentProcess->pid) {
        // if (pid < 9)
            // update = 0;
        // currentProcess = next;
        
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
    //    del = searchProcess(&prev, pid, firstBlocked);
    //    if (del == NULL)
            return EXIT_FAILURE;
    //     else 
    //         del->priority = offset + 20;
    }
    else {
        del->priority = offset + 20;
    }
    return EXIT_SUCCESS;
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
        return EXIT_FAILURE;
    return currentProcess->pid;
}

char quitCPU() {
    int pid = getPid();
    if (pid == EXIT_FAILURE)
        return EXIT_FAILURE;
    // return block(pid);
    forceTimer();
    return EXIT_SUCCESS;
}

void getGenProcessData(char ** out, char * written, char toAdd, char * in, char isLast) {
    char copied = strcpy(*out, in);
    *out += copied;
    *out += addSpaces(*out, toAdd - copied);
    *written += toAdd;
    if (!isLast) {
        *out += addSpaces(*out, 2);
        *written += 2;
    }
}

char getProcessData(char * out, processCDT * proc) {
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
    getGenProcessData(&out, &written, MAX_ATTR_SIZE, proc->state == BLOCKED ? "Block" : proc->state == DEAD ? "Killed" : "Ready", 1);
    
    return written;
}

char * processes(){
    char * ans = pvPortMalloc(pids * PROCESS_DATA_MAX_SIZE);
    char * ret = ans;
    
    char * info = "name       pid     prio    rsp         rbp         fore      state\n";
    ans += strcpy(ans, info);
    // ans += 56;

    processCDT * aux = firstProcess;
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
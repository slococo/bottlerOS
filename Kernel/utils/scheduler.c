#include "scheduler.h"
#define IDLE_PID 1

// void _initialize_stack_frame(void *, void *, int, char**, void **, void **);
void _initialize_stack_frame(void *, void *, int, char**);

enum states {READY = 0, BLOCKED};

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
    void * sseBytes;
    void * fpuBytes;
} processCDT;

processCDT * firstReady = NULL;
processCDT * lastReady = NULL;
processCDT * firstBlocked = NULL;
processCDT * lastBlocked = NULL;

static processCDT * currentProcess = NULL;
static int pids = IDLE_PID;
static char update = 1;

uint64_t nextProcess() {
    update = 1;
    if (currentProcess == NULL) {
        if (firstReady == NULL)
            unblock(IDLE_PID);

        currentProcess = firstReady;
        return firstReady->rsp;
    }
    if (currentProcess->executions < MAX_PRIORITY - currentProcess->priority + 1) {
        currentProcess->executions++;
        return currentProcess->rsp;
    }
    currentProcess->executions = 0;
    if (currentProcess->next != NULL)
        currentProcess = currentProcess->next;
    else {
        currentProcess = firstReady;
    }
    return currentProcess->rsp;
}

void idle() {
    while (1) {
        haltcpu();
    }
}

void initScheduler() {
    char * argv[] = {"Idle"};
    enqueueProcess(idle, 0, 1, argv);
}

// void setFn(void (*fn) (int, char **), int argc, char *argv[]) {
//     currentProcess->rsp = (currentProcess->rbp) - 20;
    // _initialize_stack_frame(fn, currentProcess->rbp, argc, argv);
// }

void enqueueProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[]) {
// void enqueueProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[], int * fd[2]) {
    if (firstReady != NULL && firstReady->pid == IDLE_PID)
        block(IDLE_PID);

    processADT process = pvPortMalloc(sizeof(processCDT));
    uint64_t * auxi = pvPortMalloc(STACK_SIZE);
    uint64_t * rbp = STACK_SIZE + auxi;
    uint64_t * rsp = rbp - 20; //22
    
    char priority = (foreground == 1) ? DEF_PRIORITY : MAX_PRIORITY/2;

    // newProcess(process, argv[0], priority, foreground, (uint64_t) rsp, (uint64_t) rbp);
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
    // process->sseBytes = pvPortMalloc(64);
    // process->fpuBytes = pvPortMalloc(14);

    // _initialize_stack_frame(fn, rbp, argc, argv, &(process->fpuBytes), &(process->sseBytes));
    _initialize_stack_frame(fn, rbp, argc, argv);

    if (firstReady == NULL)
        firstReady = process;
    else 
        lastReady->next = process;
    lastReady = process;
        
    // ncClear();
    // ncPrint(argv[0]);
    // // proc->name = argv[0];
    // ncPrint(process->name);
    // ncPrintDec(process->pid);
    // ncPrintHex(process->rsp);
    // ncPrintHex(process->rbp);
    // wait(3);
    return;
}

void * getSSEaddress() {
    return currentProcess->sseBytes;
}

void * getFPUaddress() {
    return currentProcess->fpuBytes;
}

void newProcess(processADT process, char * name, char priority, char foreground, uint64_t rsp, uint64_t rbp) {
    // char aux[MAX_NAME_SIZE];
    // int j;
    // for (j = 0; j < MAX_NAME_SIZE - 1 && name[j] != 0; j++) {
    //     aux[j] = name[j];
    // }
    // aux[j] = '\0';

    // process->name = aux;
    // process->pid = pids++;
    // process->ppid = currentProcess->pid;
    // process->priority = priority;
    // process->rsp = rsp;
    // process->rbp = rbp;
    // process->executions = 0;
    // process->foreground = foreground;
    // process->state = READY;
}

// void loader(int argc, char * argv[], void (*fn) (int, char **)) {
//     fn(argc, argv);
//     exit();
// }

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
    if (curr == first) {
        * previous = NULL;
        return curr;
    }
    return curr;
}

char block(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstReady);
    if (del == NULL)
        return EXIT_FAILURE;
    else {
        if (prev != NULL)
            prev->next = del->next;
        else
            firstReady = del->next;
    }

    processCDT * next = del->next;
    del->next = NULL;

    if (lastBlocked != NULL)
        lastBlocked->next = del;
    else
        firstBlocked = del;
    lastBlocked = del;

    if (pid == currentProcess->pid) {
        update = 0;
        currentProcess = next;
        forceTimer();
    }

    return EXIT_SUCCESS; 
}

char unblock(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstBlocked);
    if (del == NULL)
        return EXIT_FAILURE;
    else {
        if (prev != NULL)
            prev->next = del->next;
        else
            firstBlocked = del->next;
        del->next = NULL;
        if (lastReady != NULL)
            lastReady->next = del;
        else
            firstReady = del;
        lastReady = del;
    }
    
    return EXIT_SUCCESS;
}

char kill(int pid) {
    processADT prev = NULL;
    processADT del = searchProcess(&prev, pid, firstReady);
    if (del == NULL) {
        del = searchProcess(&prev, pid, firstBlocked);
        if (del == NULL)
            return EXIT_FAILURE;
        else {
            if (prev != NULL)
                prev->next = del->next;
            else
                firstBlocked = del->next;
        }
    }
    else {
        if (prev != NULL)
            prev->next = del->next;
        else
            firstReady = del->next;
    }

    processCDT * next = del->next;

    vPortFree((void *) del->rbp - STACK_SIZE);
    vPortFree((void *) del);

    if (pid == currentProcess->pid) {
        update = 0;
        currentProcess = next;
        forceTimer();
    }

    return EXIT_SUCCESS;
}

int getFdOut() {
    if (currentProcess == NULL)
        return NULL;
    return currentProcess->fd[1];
}

int getFdIn() {
    if (currentProcess == NULL)
        return NULL;
    return currentProcess->fd[0];
}

void exitProcess() {
    kill(currentProcess->pid);
}

char nice(char offset) {
    if (currentProcess == NULL)
        return EXIT_FAILURE;
    currentProcess->priority += offset;
    return EXIT_SUCCESS;
}

void updateRSP(uint64_t newRsp) {
    if (currentProcess == NULL) {
        return;
    }
    if (update)
        currentProcess->rsp = newRsp;
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

char getGenProcessData(char ** out, char * written, char toAdd, char * in, char isLast) {
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
    // char copied = strcpy(out, itoa(proc->pid, buffer, 10, 10));
    // out += copied;
    // out += addSpaces(out, MAX_ATTR_SIZE - copied);
    // written += MAX_ATTR_SIZE;
    // out += addSpaces(out, 2);
    // written += 2;

    getGenProcessData(&out, &written, MAX_ATTR_SIZE, itoa(proc->priority, buffer, 10, 2), 0);
    // copied = strcpy(out, itoa(proc->priority, buffer, 10, 2));
    // out += copied;
    // out += addSpaces(out, MAX_ATTR_SIZE - copied);
    // written += MAX_ATTR_SIZE;
    // out += addSpaces(out, 2);
    // written += 2;

    getGenProcessData(&out, &written, MAX_NAME_SIZE, itoa(proc->rsp, buffer, 16, 10), 0);
    // copied = strcpy(out, itoa(proc->rsp, buffer, 16, 10));
    // out += copied;
    // out += addSpaces(out, MAX_NAME_SIZE - copied);
    // written += MAX_NAME_SIZE;
    // out += addSpaces(out, 2);
    // written += 2;

    getGenProcessData(&out, &written, MAX_NAME_SIZE, itoa(proc->rbp, buffer, 16, 10), 0);
    // copied = strcpy(out, itoa(proc->rbp, buffer, 16, 10));
    // out += copied;
    // out += addSpaces(out, MAX_NAME_SIZE - copied);
    // written += MAX_NAME_SIZE;
    // out += addSpaces(out, 2);
    // written += 2;

    getGenProcessData(&out, &written, MAX_ATTR_SIZE, (proc->foreground == 1) ? "F" : "B", 1);
    // *out++ = (proc->foreground == 1) ? 'F' : 'B';
    // out += addSpaces(out, MAX_ATTR_SIZE - 1);
    // written += MAX_ATTR_SIZE;

    // out += addSpaces(out, 2);
    // written += 2;
    
    return written;
}

char * processes(){
    char * ans = pvPortMalloc(pids * PROCESS_DATA_MAX_SIZE);
    char * ret = ans;
    
    char * info = "name       pid     prio    rsp         rbp         fore\n";
    ans += strcpy(ans, info);
    // ans += 56;

    processCDT * aux = firstReady;
    while (aux != NULL) {
        char writtenChars = getProcessData(ans, aux);
        if (writtenChars == EXIT_FAILURE)
            return NULL;
        ans += writtenChars - 1;
        *ans++ = '\n';

        if (aux == lastBlocked)
            aux = NULL;
        else if (aux == lastReady)
            aux = firstBlocked;
        else
            aux = aux->next;
    }
    *--ans = 0;

    return ret;
}

/*
● Crear un proceso. DEBERÁ soportar el pasaje de parámetros. LISTO
● Obtener el ID del proceso que llama. LISTO
● Listar todos los procesos: nombre, ID, prioridad, stack y base pointer, foreground y
cualquier otra variable que consideren necesaria. LISTO
● Matar un proceso arbitrario. LISTO
● Modificar la prioridad de un proceso arbitrario.  LISTO
● Bloquear y desbloquear un proceso arbitrario. LISTO
● Renunciar al CPU. LISTO
*/
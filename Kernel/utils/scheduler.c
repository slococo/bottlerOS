#include "scheduler.h"
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
} processCDT;

processCDT * firstReady = NULL;
processCDT * lastReady = NULL;
processCDT * firstBlocked = NULL;
processCDT * lastBlocked = NULL;

int readyLen = 0;
int blockedLen = 0;

processCDT * currentProcess = NULL;
int pids = 0;

uint64_t nextProcess() {
    if (currentProcess == NULL)
        return firstReady->rsp;
    if (currentProcess->executions < MAX_PRIORITY - currentProcess->priority + 1) {
        currentProcess->executions++;
        return currentProcess->rsp;
    }
    currentProcess->executions = 0;
    currentProcess = currentProcess->next;
    return currentProcess->rsp;
}

void idle() {
    while (1) {
        haltcpu();
    }
}

void initScheduler() {
    char * argv[] = {"Dummy"};
    enqueueProcess(idle, 0, 1, argv);
}

void enqueueProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[]) {
    processADT proc = pvPortMalloc(sizeof(processCDT));
    uint64_t * rbp = pvPortMalloc(STACK_SIZE);
    uint64_t * rsp = rbp;
    
    char priority = (foreground == 1) ? DEF_PRIORITY : MAX_PRIORITY/2;

    newProcess(proc, argv[0], priority, foreground, (uint64_t)rsp, (uint64_t)rbp);

    _initialize_stack_frame(fn, rbp, argc, argv);

    if (firstReady == NULL)
        firstReady = proc;
    else
        lastReady->next = proc;
        
    return;
}

void newProcess(processADT process, char * name, char priority, char foreground, uint64_t rsp, uint64_t rbp) {
    char aux[MAX_NAME_SIZE];
    for (int j = 0; j < MAX_NAME_SIZE && name[j] != 0; j++) {
        aux[j] = name[j];
    }

    process->name = aux;
    process->pid = pids++;
    process->ppid = currentProcess->pid;
    process->priority = priority;
    process->rsp = rsp;
    process->rbp = rbp;
    process->executions = 0;
    process->foreground = foreground;
    process->state = READY;
}

// void loader(int argc, char * argv[], void (*fn) (int, char **)) {
//     fn(argc, argv);
//     exit();
// }

processCDT * search(processCDT ** previous, int pid, processCDT * first) {
    processCDT * curr = first;
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
    processCDT * prev = NULL;
    processCDT * del = search(&prev, pid, firstReady);
    if (del == NULL)
        return EXIT_FAILURE;
    else {
        if (prev != NULL)
            prev->next = del->next;
        else
            firstReady = del->next;
    }

    lastBlocked->next = del;
    del->next = NULL;
    lastBlocked = del;

    forceTimer();

    return EXIT_SUCCESS; 
}

char unblock(int pid) {
    processCDT * prev = NULL;
    processCDT * del = search(&prev, pid, firstBlocked);
    if (del == NULL)
        return EXIT_FAILURE;
    else {
        if (prev != NULL)
            prev->next = del->next;
        else
            firstBlocked = del->next;
    }

    if (pid == del->pid)
        forceTimer();
    
    return EXIT_SUCCESS;
}

char kill(int pid) {
    processCDT * prev = NULL;
    processCDT * del = search(&prev, pid, firstReady);
    if (del == NULL) {
        del = search(&prev, pid, firstBlocked);
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

    if (pid == del->pid)
        forceTimer();
    
    vPortFree((void *) del->rsp);
    vPortFree((void *) del);

    return EXIT_SUCCESS;
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
    if (currentProcess == NULL)
        return;
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

char addSpaces(char * str, char qty) {
    char aux = qty;
    while (qty-- > 0) {
        *str++ = ' ';
    }
    return aux;
}

char getProcessData(char * out, processCDT * proc) {
    if (proc == NULL)
        return EXIT_FAILURE;

    char written = 0;

    char flag = 0;
    for (int j = 0; j < MAX_ATTR_SIZE; j++) {
        if (proc->name[j] != 0)
            flag = 1;
        if (flag){
            out += addSpaces(out, 1);
        } else
            *out++ = proc->name[j];
    }
    written += MAX_ATTR_SIZE;

    out += addSpaces(out, 2);
    written += 2;
    
    char buffer[10];
    written += strcpy(out, itoa(proc->pid, buffer, 10, 10));
    out += addSpaces(out, 2);

    buffer[0] = 0;
    written += strcpy(out, itoa(proc->priority, buffer, 10, 2));
    out += addSpaces(out, 2);

    buffer[0] = 0;
    written += strcpy(out, itoa(proc->rsp, buffer, 16, 10));
    out += addSpaces(out, 2);

    buffer[0] = 0;
    written += strcpy(out, itoa(proc->rbp, buffer, 16, 10));
    out += addSpaces(out, 2);

    buffer[0] = 0;
    written += strcpy(out, (proc->foreground == 1) ? "F" : "B");
    // out += addSpaces(out, 2);
    
    return written;
}

char * processes(){
    char * ans = pvPortMalloc(pids * PROCESS_DATA_MAX_SIZE);
    
    int lastPid = getPid();
    if (lastPid == EXIT_FAILURE)
        return NULL;
    processCDT * aux = firstReady;
    while (aux != NULL){
        char writtenChars = getProcessData(ans, aux);
        if (writtenChars == EXIT_FAILURE)
            return NULL;
        ans += writtenChars;
        *ans++ = '\n';

        if (aux == lastBlocked)
            aux = NULL;
        else if (aux == lastReady)
            aux = firstBlocked;
        else
            aux = aux->next;
    }
    *ans = 0;
    return ans;
}

/*
● Crear un proceso. DEBERÁ soportar el pasaje de parámetros. LISTO
● Obtener el ID del proceso que llama. LISTO
● Listar todos los procesos: nombre, ID, prioridad, stack y base pointer, foreground y
cualquier otra variable que consideren necesaria.
● Matar un proceso arbitrario. LISTO
● Modificar la prioridad de un proceso arbitrario.  LISTO
● Bloquear y desbloquear un proceso arbitrario. LISTO
● Renunciar al CPU. LISTO
*/
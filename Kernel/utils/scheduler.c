#include "scheduler.h"
#include "queue.h"
#include <stdint.h>
#include <stddef.h>
#include "memManager.h"

void forceTimer();

#define STACK_SIZE 1024

enum states {READY = 0, BLOCKED};

typedef struct processCDT {
    char * name;
    int pid;
    int ppid;
    uint64_t rsp;
    uint64_t rbp;
    char priority;
    char foreground;
    enum states state;
} processCDT;

typedef struct nodeT {
    processADT * process;
    struct nodeT * next;
    struct nodeT * previous;
} nodeT;

typedef struct listT {
    nodeT * first;
    nodeT * last;
    int length;
} listT; 

listT * ready = {NULL, NULL, 0};
listT * blocked = {NULL, NULL, 0};
nodeT * current = NULL;
int pids = 0;

processADT * next() {
    if (ready->first == NULL)
        return NULL;
    current = current->next;
    if (current == NULL)
        current = ready->first;
    return current->process;
}

// void init() {
//     new(idle, )
// }

void idle() {
    while (1) {
        haltcpu();
    }    
}

void launchProcess(void (*fn) (int, char **), char priority, char foreground, int argc, char *argv[]) {
    processADT proc = pvPortMalloc(sizeof(processCDT));
    nodeT * node = pvPortMalloc(sizeof(nodeT));
    uint64_t * rbp = pvPortMalloc(STACK_SIZE);
    uint64_t * rsp = rbp;

    newProcess(proc, argv[0], priority, foreground);

    _initialize_stack_frame(fn, rbp, argc, argv);

    if (current != NULL && foreground) {
        block((*current->process)->pid);
        return;
    }

    return;
}

void newProcess(processADT process, char * name, char priority, char foreground) {


    process->name = name;
    process->pid = pids++;
    process->ppid = (*current->process)->pid;
    process->foreground = foreground;
    process->priority = priority;
    process->state = READY;
    process->rbp = STACK_SIZE;
}

void loader(int argc, char * argv[], void (*fn) (int, char **)) {
    fn(argc, argv);
    exit();
}

void exit() {
    kill((*current->process)->pid);
}

char block(int pid) {
    nodeT * prev = NULL;
    nodeT * del = search(&prev, pid, ready);
    if (del == NULL)
        return -1;
    else {
        if (prev != NULL)
            prev->next = del->next;
        else
            ready->first = del->next;
    }
    
    if (pid == (*del->process)->pid)
        forceTimer();

    return 0; 
}

char unblock(int pid) {
    nodeT * prev = NULL;
    nodeT * del = search(&prev, pid, blocked);
    if (del == NULL)
        return -1;
    else {
        if (prev != NULL)
            prev->next = del->next;
        else
            blocked->first = del->next;
    }

    if (pid == (*del->process)->pid)
        forceTimer();
    
    return 0;
}

char kill(int pid) {
    nodeT * prev = NULL;
    nodeT * del = search(&prev, pid, ready);
    if (del == NULL) {
        del = search(&prev, pid, blocked);
        if (del == NULL)
            return -1;
        else {
            if (prev != NULL)
                prev->next = del->next;
            else
                blocked->first = del->next;
        }
    }
    else {
        if (prev != NULL)
            prev->next = del->next;
        else
            ready->first = del->next;
    }

    if (pid == (*del->process)->pid)
        forceTimer();
    
    vPortFree((*del->process)->rsp);
    vPortFree(del);

    return 0;
}

nodeT * search(nodeT ** previous, int pid, listT * list) {
    nodeT * curr = list->first;
    * previous = NULL;
    while (curr != NULL) {
        if ((*curr->process)->pid == pid) {
            break;
        }
        * previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        * previous = NULL;
        return NULL;
    }
    if (curr = list->first) {
        * previous = NULL;
        return curr;
    }
    return curr;
}

/*
● Crear un proceso. DEBERÁ soportar el pasaje de parámetros.
● Obtener el ID del proceso que llama.
● Listar todos los procesos: nombre, ID, prioridad, stack y base pointer, foreground y
cualquier otra variable que consideren necesaria.
● Matar un proceso arbitrario.
● Modificar la prioridad de un proceso arbitrario.
● Bloquear y desbloquear un proceso arbitrario.
● Renunciar al CPU
*/
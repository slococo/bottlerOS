#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "libc.h"
#include "scheduler.h"

#define MAX_SEM 100
#define MAX_NAME 100

void enter_region(uint32_t * lock);
void leave_region(uint32_t * lock);
static uint32_t semLock;

typedef struct sem {
    unsigned int value;
    char name[MAX_NAME];
    // cola FIFO
    pid_t * entering;
    pid_t * last;
} sem_t;

typedef struct pid_t {
    int pid;
    struct pid_t * next;
} pid_t;

// Podemos manejarnos con indices. 
static sem_t semaphores[MAX_SEM];
/*
int semOpen(char * name, unsigned int value) {}

// int semClose(char * name) {}
int semClose(char semaphore) {}

int semInit(char semaphore, unsigned int value) {}

int semWait(char semaphore) {}

int semPost(char semaphore) {}
*/

// o pasando la estructura de sem. esta es la q hace posix
sem_t * semOpen(char * name, unsigned int value) {
    enter_region(&semLock);
    
    sem_t * sem = pvPortMalloc(sizeof(sem_t));
    strcpy(sem->name, name);
    sem->value = value;

    leave_region(&semLock);
}

int semClose(sem_t * sem) {
    if (sem == NULL)
        return EXIT_FAILURE;

    enter_region(&semLock);

    pid_t * pid = sem->entering;
    while (pid != NULL) {
        pid_t * aux = pid;
        pid = pid->next;
        vPortFree(aux);
    }
    vPortFree(sem);
    
    leave_region(&semLock);
}

int semWait(sem_t * sem) {
    enter_region(&semLock);

    if (sem->value > 0) {
        sem->value--;
    } 
    else {
        leave_region(&semLock);

        pid_t * curr = pvPortMalloc(sizeof(pid_t));
        curr->pid = getPid();
        curr->next = NULL;
        sem->last->next = curr;
        sem->last = curr;
        block(curr->pid);

        enter_region(&semLock);
        sem->value--;
    }

    leave_region(&semLock);
}

int semPost(sem_t * sem) {
    enter_region(&semLock);

    sem->value++;

    pid_t * aux = sem->entering;
    sem->entering = sem->entering->next;
    unblock(sem->entering->pid);
    pvPortFree(aux);

    leave_region(&semLock);
}
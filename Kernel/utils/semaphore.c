#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "lib.h"
#include "scheduler.h"
#include "memManager.h"

#define MAX_SEM 100
#define MAX_NAME 100
#define SEM_DATA_MAX_SIZE 100

void enter_region(uint32_t * lock);
void leave_region(uint32_t * lock);
static uint32_t semLock;

typedef struct pid_t {
    int pid;
    struct pid_t * next;
} pid_t;

typedef struct sem_t {
    unsigned int value;
    char name[MAX_NAME];
    pid_t * entering;
    pid_t * last;
} sem_t;

typedef struct node_t {
    sem_t * sem;
    struct node_t * next;
} node_t;

static sem_t semaphores[MAX_SEM];
node_t * first = NULL;
static char counter = 0;

sem_t * semOpen(char * name, unsigned int value) {
    enter_region(&semLock);
    
    sem_t * sem = pvPortMalloc(sizeof(sem_t));
    node_t * node = pvPortMalloc(sizeof(node_t));
    node->sem = sem;
    node->next = first;    
    first = node;
    strcpy(sem->name, name);
    sem->value = value;
    counter++;

    leave_region(&semLock);
}

int semClose(sem_t * sem) {
    if (sem == NULL)
        return EXIT_FAILURE;

    enter_region(&semLock);

    node_t * del = NULL;

    if (first == sem) {
        del = first;
        first = first->next;
    }
    else {
        node_t * aux = first;
        while (aux != NULL) {
            if (aux->next != NULL)
                if (aux->next->sem == sem) {
                    del = aux->next;
                    aux->next = aux->next->next;
                    break;
                }
            aux = aux->next;
        }
    }

    if (del == NULL) {
        leave_region(&semLock);
        return EXIT_FAILURE;
    }

    pid_t * pid = sem->entering;
    while (pid != NULL) {
        pid_t * aux = pid;
        pid = pid->next;
        vPortFree(aux);
    }
    vPortFree(sem);
    vPortFree(del);
    
    counter--;
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
    vPortFree(aux);

    leave_region(&semLock);
}

char getSemaphoresData(char * out, node_t * node) {
    if (node == NULL)
        return EXIT_FAILURE;

    char written = 0;

    char flag = 0;
    for (int j = 0; j < MAX_NAME_SIZE; j++) {
        if (!flag && node->sem->name[j] == 0)
            flag = 1;
        else if (flag)
            out += addSpaces(out, 1);
        else
            *out++ = node->sem->name[j];
    }
    written += MAX_NAME_SIZE;

    out += addSpaces(out, 2);
    written += 2;
    
    char buffer[10];
    char copied = strcpy(out, itoa(node->sem->value, buffer, 10, 2));
    out += copied;
    out += addSpaces(out, MAX_ATTR_SIZE - copied);
    written += MAX_ATTR_SIZE;
    out += addSpaces(out, 2);
    written += 2;

    pid_t *aux_pid = node->sem->entering;
        
    while (aux_pid != NULL) {
        copied = strcpy(out, itoa(aux_pid->pid, buffer, 10, 10));
        aux_pid = aux_pid->next;
        copied += addSpaces(out, 1);
        out += copied;
        written += copied;
    }
    return written;
}

char * getSems() {
    char * ans = pvPortMalloc(counter * SEM_DATA_MAX_SIZE);
    char * ret = ans;
    
    char * info = "name       value   pidsWaiting\n";
    ans += strcpy(ans, info);
    // ans += 56;

    node_t * aux = first;
    while (aux != NULL) {
        char writtenChars = getSemaphoresData(ans, aux);
        if (writtenChars == EXIT_FAILURE)
            return NULL;
        ans += writtenChars - 1;
        *ans++ = '\n';

        aux = aux->next;

    }
    *--ans = 0;

    return ret;
}

#include <stdint.h>

#define MAX_SEM 100
#define MAX_NAME 100

void enter_region(uint32_t * lock);
void leave_region(uint32_t * lock);
static uint32_t semLock;

typedef struct sem {
    unsigned int value;
    char name[MAX_NAME];
    // cola FIFO
} sem_t;

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
    leave_region(&semLock);
}

int semClose(sem_t * semaphore) {
    enter_region(&semLock);
    leave_region(&semLock);
}

int semWait(sem_t * semaphore) {
    enter_region(&semLock);

    if (semaphore->value > 0) {
        semaphore->value--;
    } 
    else {
        // leave_region(&semLock);
        // enter_region(&semLock);
        // semaphore->value--;

        // mandarlo a noni. es decir bloquearlo y solo despertarlo cuando alguien hace post
        // block(con el pid)
        // 
    }

    leave_region(&semLock);
}

int semPost(sem_t * semaphore) {
    enter_region(&semLock);
    leave_region(&semLock);
}
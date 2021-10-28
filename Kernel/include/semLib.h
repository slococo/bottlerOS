#ifndef SEMLIB_H
#define SEMLIB_H

#define MAX_NAME 100

typedef struct pid_t {
    int pid;
    struct pid_t * next;
} pid_t;

typedef struct sem_t {
    unsigned int value;
    // char name[MAX_NAME];
    char * name;
    pid_t * entering;
    pid_t * last;
} sem_t;

sem_t * semOpen(char * name, unsigned int value);
char semClose(sem_t * sem);
void semWait(sem_t * sem);
void semPost(sem_t * sem);

#endif
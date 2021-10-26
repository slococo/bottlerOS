#ifndef SEM_H
#define SEM_H

sem_t * semOpen(char * name, unsigned int value);
int semClose(sem_t * sem);
int semWait(sem_t * sem);
int semPost(sem_t * sem);

#endif
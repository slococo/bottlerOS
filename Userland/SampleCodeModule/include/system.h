#ifndef SYSTEM_H
#define SYSTEM_H

typedef struct pid_t {
    int pid;
    struct pid_t * next;
} pid_t;

typedef struct sem_t {
    unsigned int value;
    char * name;
    pid_t * entering;
    pid_t * last;
} sem_t;

int sys_loadProcess(void (*fn) (int, char **), char foreground, int argc, char *argv[], int * fd);
int sys_time(char);
void sys_exit();
void sys_write(int, char *, int);
char sys_read(int, char *, int);
char * sys_ps();
char * sys_sem();
void * sys_malloc(int);
void * sys_free(void *);
void * sys_openPipe(int *, char *);
char sys_nice(int, char);
void sys_sleep(int);
void sys_semWait(sem_t *);
void sys_semPost(sem_t *);
sem_t * sys_semOpen(char *, unsigned int);
int sys_getPid();
char sys_semClose(void *);
char sys_kill(int pid);
char sys_block(int pid);
char sys_unblock(int pid);
void sys_quitCPU();
void sys_wait();

#endif
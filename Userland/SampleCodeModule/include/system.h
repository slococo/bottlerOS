#ifndef SYSTEM_H
#define SYSTEM_H

// void sys_switchContext();
// void sys_loadProcess();
int sys_time(char);
void sys_write(int, char *, int);
char sys_read(int, char *, int);
char * sys_ps();
char * sys_sem();
void * sys_malloc(int);
void * sys_free(void *);

#endif
#ifndef SYSTEM_H
#define SYSTEM_H

void sys_switchContext();
void sys_loadProcess();
int sys_time(char);
void sys_write(int, char *, int);
char sys_read(int, char *, int);

#endif
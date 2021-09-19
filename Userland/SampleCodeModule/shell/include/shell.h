#ifndef SHELL
#define SHELL

#include "system.h"

void shell(void);
void printWindow(char * window);
void addText(char * buffer, char * window, int * offset);
void incorrect_comm(char * buffer, char * window, int * offset);
void incorrect_arg(char * command, char * window, int * offset);
void clearWindow(char * window, int * offset);
void substractLine(char * window, int * offset);

#endif
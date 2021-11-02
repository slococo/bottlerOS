#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

void swap(char * x, char * y);
char * reverse(char * buffer, int i, int j);
int abs(int value);
char * itoa(int value, char * buffer, int base, int length);
void strlen(const char *str, int *len);
int strcpy(char * strDest, const char * strSrc);
char addSpaces(char * str, char qty);
int strcmp(const char *s1, const char *s2);

#endif
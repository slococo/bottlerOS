#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

void moveToWindowVideo(char window);
int printStringLen(int color, const char * string, int maxLen);
void new_line();
void backspace();
void clear();
void increment();
void changeWindow();
void scroll();

#endif
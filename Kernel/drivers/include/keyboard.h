#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "../../utils/include/schedulerLib.h"

#define SIZE 1

void keyboard_handler();
unsigned char getKeyFromBuffer();
unsigned char getCharInterrupt();

#endif

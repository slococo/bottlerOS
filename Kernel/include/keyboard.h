#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// static unsigned char kbdsp[250];
// static unsigned char kbdus[250];
// static unsigned char kbdsp_sh[250];
// static unsigned char kbdus_sh[250];

void keyboard_handler();
unsigned char getKeyFromBuffer();
unsigned char getCharInterrupt();

#endif

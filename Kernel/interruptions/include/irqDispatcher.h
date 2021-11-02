#ifndef IRQDISP_H
#define IRQDISP_H

#include <stdint.h>

void keyboard_handler();
void timer_handler();

static void int_20();
static void int_21();

#endif
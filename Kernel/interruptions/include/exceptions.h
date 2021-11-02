#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "lib.h"
#include "time.h"
#include "naiveConsole.h"
#include "video.h"
#include "keyboard.h"
#include "interrupts.h"

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6
#define WAITSECONDS 5

void exitProcess();
long getTimeOfDay();
void forceTimer();

#endif
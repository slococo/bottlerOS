#include "time.h"
#include "libc.h"
#include "shell.h"
#include "system.h"

int getSeconds() {
    return sys_time(SECONDS);
}

int getMinutes() {
    return sys_time(MINUTES);
}

int getHours() {
    return sys_time(HOURS);
}

int getDay() {
    return sys_time(DAY);
}

int getMonth() {
    return sys_time(MONTH);
}

int getYear() {
    return sys_time(YEAR);
}

void printTime(int number, char * window, int * offset) {
    char buffer[3] = {0};
    buffer[0] = number / 10 + '0';
    buffer[1] = number % 10 + '0';
    buffer[2] = '\0';
    addText(buffer, window, offset);
}

void time(char * window, int * offset) {
    printTime(getDay(), window, offset);
    addText("/", window, offset);
    printTime(getMonth(), window, offset);
    addText("/", window, offset);
    printTime(getYear(), window, offset);
    addText(" ", window, offset);
    printTime(getHours(), window, offset);
    addText(":", window, offset);
    printTime(getMinutes(), window, offset);
    addText(":", window, offset);
    printTime(getSeconds(), window, offset);
    printWindow(window);
    substractLine(window, offset);
}
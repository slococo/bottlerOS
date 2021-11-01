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

void printTime(int number) {
    char buffer[3] = {0};
    buffer[0] = number / 10 + '0';
    buffer[1] = number % 10 + '0';
    buffer[2] = '\0';
    printString(buffer);
}

void time(int argc, char *argv[]) {
    printTime(getDay());
    putChar('/');
    printTime(getMonth());
    putChar('/');
    printTime(getYear());
    putChar(' ');
    printTime(getHours());
    putChar(':');
    printTime(getMinutes());
    putChar(':');
    printTime(getSeconds());
    addEOF();
    newline();
    sys_exit();
}
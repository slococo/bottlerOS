#include "time.h"

static unsigned long ticks = 0;

void timer_handler() {
    ticks++;
}

int ticks_elapsed() {
    return ticks;
}

int seconds_elapsed() {
    return ticks / 18;
}

int getTime(char option) {
    switch (option) {
        case SECONDS:
            return getTimeGen(SECONDS);
        case MINUTES:
            return getTimeGen(MINUTES);
        case HOURS:
            return getTimeGen(HOURS);
        case DAY:
            return getTimeGen(DAY);
        case MONTH:
            return getTimeGen(MONTH);
        case YEAR:
            return getTimeGen(YEAR);
        default:
            return -1;
    }
}

static long getTimeFrom2000() {
    long years = 00 * 31557600;
    long months = 1 * 2629800;
    long days = 1 * 86400;

    return years + months + days;
}

long getTimeOfDay() {
    long years = getTimeGen(YEAR) * 31557600;
    long months = getTimeGen(MONTH) * 2629800;
    long days = getTimeGen(DAY) * 86400;
    long hours = getTimeGen(HOURS) * 3600;
    long minutes = getTimeGen(MINUTES) * 60;
    long seconds = getTimeGen(SECONDS);

    long total = years + months + days + hours + minutes + seconds;

    return total - getTimeFrom2000();
}
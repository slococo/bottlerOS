#ifndef TIME_H
#define TIME_H

void timer_handler();
int ticks_elapsed();
int seconds_elapsed();

#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY 7
#define MONTH 8
#define YEAR 9

int getTimeGen(char option);
int getTime(char option);
long getTimeOfDay();

#endif

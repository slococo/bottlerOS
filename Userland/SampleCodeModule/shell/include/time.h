#ifndef TIME
#define TIME

int getSeconds();
int getMinutes();
int getHours();
int getDays();
int getMonths();
int getYears();
void time(int argc, char *argv[]);

#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY 7
#define MONTH 8
#define YEAR 9

#define UTC -3

#endif
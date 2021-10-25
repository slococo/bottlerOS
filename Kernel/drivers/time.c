#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	// return ticks / 18;
	return ticks / 18;
}

int getTime(char option) {
	switch(option) {
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
		default: return -1;
	}
}

// TODO
void wait(long seconds) {
	// int initialSeconds = getTime(SECONDS);
	// while (initialSeconds - seconds != initialSeconds) {

	// }
	// while (seconds_elapsed() < seconds);
}
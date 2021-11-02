#ifndef TESTSYNC_H
#define TESTSYNC_H

#include <stdint.h>
#include <stddef.h>
#include "system.h"
#include "libc.h"

void test_sync(int argc, char *argv[]);
void test_no_sync(int argc, char ** argv);

#endif
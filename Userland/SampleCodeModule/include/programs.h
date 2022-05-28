
#ifndef TPE_PROGRAMS_H
#define TPE_PROGRAMS_H

#include <stdint.h>
#include <libc.h>
#include <math.h>

#define COUNT_REGS (18)

typedef uint8_t (* time_func) ();

void inforeg();

void printmem();

void tiempo();

void primos();

void fibonacci();

void help();

void zero_division_exc();

void invalid_opcode_exc();

#endif //TPE_PROGRAMS_H

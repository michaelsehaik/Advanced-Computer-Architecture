#pragma once

#include <stdbool.h>

// simulate the clock
typedef struct Clock {
	int cycle;
} Clock;

typedef struct LinkRegister {
	bool flag; // if flag is true then address is on watch
	int address;
} LinkRegister;

typedef struct DQ_FF {
	int Q;	// note: Q is the first 4 bytes.
	int D;  // note: D is the next 4 bytes.
} DQ_FF;


/**
* move all D values to Q values in register (array of numOfFF flip flops)
*/
void register_DtoQ(DQ_FF* FFs, int numOfFF);
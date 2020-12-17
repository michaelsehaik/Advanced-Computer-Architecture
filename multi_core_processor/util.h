#pragma once

#include <stdbool.h>

typedef struct Clock {
	int cycle;
} Clock;

typedef struct RegisterDMA {
	bool flag;
	int address;
} RegisterDMA;

typedef struct DQ_FF {
	int D;
	int Q;
} DQ_FF;


/**
* move all D values to Q values in register (array of numOfFF flip flops)
*/
void register_DtoQ(DQ_FF* FFs, int numOfFF);
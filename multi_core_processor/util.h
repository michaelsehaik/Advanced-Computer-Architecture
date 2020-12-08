#pragma once

#include <stdbool.h>

typedef struct Clock {
	int cycle;
} Clock;

typedef struct RegisterDMA {
	bool flag;
	int address;
} RegisterDMA;
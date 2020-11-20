#pragma once

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct Clock {
	int cycle;
} Clock;

#define IMEM_FILE_BASE				1
#define MEMIN_FILE					5
#define MEMOUT_FILE					6
#define REGS_FILE_BASE				7
#define CORE_TRACE_FILE_BASE		11
#define BUSTRACE_FILE				15
#define DSRAM_FILE_BASE				16
#define TSRAM_FILE_BASE				20
#define STATS_FILE_BASE				24

static char* default_args[] = {  "sim.exe",
								"imem0.txt", "imem1.txt", "imem2.txt", "imem3.txt",
								"memin.txt", "memout.txt",
								"regout0.txt", "regout1.txt", "regout2.txt", "regout3.txt",
								"core0trace.txt", "core1trace.txt", "core2trace.txt", "core3trace.txt",
								"bustrace.txt",
								"dsram0.txt", "dsram1.txt", "dsram2.txt", "dsram3.txt",
								"tsram0.txt", "tsram1.txt", "tsram2.txt", "tsram3.txt",
								"stats0.txt", "stats1.txt", "stats2.txt", "stats3.txt",
							    0 };

int loadArrayFromFile(FILE* file, int valueArray[], int size);
void arrayToFile(char* filepath, int valueArray[], int size, bool newline);
//int* allocateArrayFromFile(char *filepath);

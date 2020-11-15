#pragma once

#include <string.h>
#include <stdio.h>

#define IMEM_FILE_BASE				1
#define MEMIN_FILE					5
#define MEMOUT_FILE					6
#define REGOUT_FILE					7
#define CORE_TRACE_FILE_BASE		8
#define BUSTRACE_FILE				12
#define DSRAM_FILE_BASE				13
#define TSRAM_FILE_BASE				17
#define STATS_FILE_BASE				21

static const char* const default_args[] = { "sim.exe", "imem0.txt", "imem1.txt", "imem2.txt", "imem3.txt", "memin.txt", "memout.txt",
											"regout.txt", "core0trace.txt", "core1trace.txt", "core2trace.txt", "core3trace.txt", "bustrace.txt",
											"dsram0.txt", "dsram1.txt", "dsram2.txt", "dsram3.txt", "tsram0.txt", "tsram1.txt", "tsram2.txt",
											"tsram3.txt", "stats0.txt", "stats1.txt", "stats2.txt", "stats3.txt", 0 };

void loadArrayFromFile(FILE* file, int valueArray[], int size);
//int* allocateArrayFromFile(char *filepath);

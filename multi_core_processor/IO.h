#pragma once

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define IMEM_FILE_BASE				1
#define MEMIN_FILE					5
#define MEMOUT_FILE					6
#define REGS_FILE_BASE				7
#define CORE_TRACE_FILE_BASE		11
#define BUSTRACE_FILE				15
#define DSRAM_FILE_BASE				16
#define TSRAM_FILE_BASE				20
#define STATS_FILE_BASE				24
#define NUM_OF_FILENAMES			28

static char* default_args[] = { "sim.exe",
								"imem0.txt", "imem1.txt", "imem2.txt", "imem3.txt",
								"memin.txt", "memout.txt",
								"regout0.txt", "regout1.txt", "regout2.txt", "regout3.txt",
								"core0trace.txt", "core1trace.txt", "core2trace.txt", "core3trace.txt",
								"bustrace.txt",
								"dsram0.txt", "dsram1.txt", "dsram2.txt", "dsram3.txt",
								"tsram0.txt", "tsram1.txt", "tsram2.txt", "tsram3.txt",
								"stats0.txt", "stats1.txt", "stats2.txt", "stats3.txt",
								0 };

void checkFiles(char **filepaths);
int loadArrayFromFile(FILE* file, int valuesArray[], int size);
void printArray(FILE *outputFile, int valueArray[], int size, bool newline);
void createFileFromArray(char* filepath, int valueArray[], int size, bool newline);
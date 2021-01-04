#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>

#include "IO.h"

/**
* check if @parma filename exists
*/
void fileExists(char *filename) {
	struct stat buffer;
	if (stat(filename, &buffer) != 0) {
		printf("Input file %s does not exist. Aborting...", filename);
		exit(1);
	}
}

/**
* check if all relevant input files exist
*/
void checkFiles(char **filepaths) {
	int fileIdx = 1;
	while (filepaths[fileIdx] != NULL) {
		if (fileIdx != MEMOUT_FILE && !(fileIdx >= REGS_FILE_BASE && fileIdx <= BUSTRACE_FILE) && fileIdx < STATS_FILE_BASE) {
			fileExists(filepaths[fileIdx]);
		}
		fileIdx++;
	}
	assert(fileIdx == NUM_OF_FILENAMES);
}

/**
* load values from the given file to the give array
*/
int loadArrayFromFile(FILE* file, int valuesArray[], int size) {
	int numOfLines = 0;
	while (fscanf(file, "%x\n", &valuesArray[numOfLines++]) > 0 && numOfLines < size);
	fseek(file, 0, SEEK_SET); // move pointer back to begginnig of file
	return numOfLines - 1;
}

/**
* write array to file
*/
void printArray(FILE *outputFile, int* valueArray, int size, bool newline, bool isFF) {
	for (int i = 0; i < size; i++) {
		int idx = isFF ? i * 2 : i;
		fprintf(outputFile, "%08X", valueArray[idx]);
		if (newline) fprintf(outputFile, "\n");
		else fprintf(outputFile, " ");
	}
}

/**
* create new file base on the data in the given array
*/
void createFileFromArray(char* filepath, int* valueArray, int size, bool newline, bool isFF) {
	FILE *outputFile = NULL;
	fopen_s(&outputFile, filepath, "w");
	printArray(outputFile, valueArray, size, newline, isFF);
	fclose(outputFile);
}

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>

#include "IO.h"

void fileExists(char *filename) {
	struct stat buffer;
	if (stat(filename, &buffer) != 0) {
		printf("Input file %s does not exist. Aborting...", filename);
		exit(1);
	}
}

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

int loadArrayFromFile(FILE* file, int valuesArray[], int size) {
	int numOfLines = 0;
	while (fscanf(file, "%x\n", &valuesArray[numOfLines++]) != 0 && numOfLines < size);
	fseek(file, 0, SEEK_SET); // move pointer back to begginnig of file
	return numOfLines;
}

void printArray(FILE *outputFile, int valueArray[], int size, bool newline) {
	for (int i = 0; i < size; i++) {
		fprintf(outputFile, "%08X", valueArray[i]);
		if (newline) fprintf(outputFile, "\n");
		else fprintf(outputFile, " ");
	}
}

void createFileFromArray(char* filepath, int valueArray[], int size, bool newline) {
	FILE *outputFile = NULL;
	fopen_s(&outputFile, filepath, "w");
	printArray(outputFile, valueArray, size, newline);
	fclose(outputFile);
}

/*
int getNumOfLines(FILE* file) {
	int count = 0;
	for (char ch = getc(file); ch != EOF; ch = getc(file))
		if (ch == '\n') count++;

	fseek(file, 0, SEEK_SET); // move pointer back to begginnig of file
	return count;
}

int* allocateArrayFromFile(char *filepath) {
	FILE* file;
	int numOfLines;
	int* valuesArray;

	file = fopen(filepath, "r");
	numOfLines = getNumOfLines(file);

	valuesArray = (int *)calloc(numOfLines, sizeof(int));
	loadArrayFromFile(file, valuesArray, numOfLines);

	fclose(file);

	return valuesArray;
}
*/
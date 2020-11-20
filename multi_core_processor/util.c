#include <string.h>
#include <stdio.h>

#include "util.h"

int loadArrayFromFile(FILE* file, int valuesArray[], int size) {
	int numOfLines = 0;
	while (fscanf(file, "%x\n", &valuesArray[numOfLines++]) != 0 && numOfLines < size);
	fseek(file, 0, SEEK_SET); // move pointer back to begginnig of file
	return numOfLines;
}

void arrayToFile(char* filepath, int valueArray[], int size, bool newline) {
	FILE *outputFile = NULL;
	fopen_s(&outputFile, filepath, "w");
	for (int i = 0; i < size; i++) {
		fprintf(outputFile, "%08X", valueArray[i]);
		if (newline) fprintf(outputFile, "\n");
	}
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
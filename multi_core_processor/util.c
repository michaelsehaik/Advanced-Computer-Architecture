#include <string.h>
#include <stdio.h>

void loadArrayFromFile(FILE* file, int valuesArray[], int size) {
	int i = 0;
	while (fscanf_s(file, "%x\n", &valuesArray[i++]) != 0 && i < size);
	fseek(file, 0, SEEK_SET); // move pointer back to begginnig of file
}

int getNumOfLines(FILE* file) {
	int count = 0;
	for (char ch = getc(file); ch != EOF; ch = getc(file))
		if (ch == '\n') count++;

	fseek(file, 0, SEEK_SET); // move pointer back to begginnig of file
	return count;
}
/*
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
#pragma once

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct Clock {
	int cycle;
} Clock;

int loadArrayFromFile(FILE* file, int valueArray[], int size);
void arrayToFile(char* filepath, int valueArray[], int size, bool newline);
//int* allocateArrayFromFile(char *filepath);

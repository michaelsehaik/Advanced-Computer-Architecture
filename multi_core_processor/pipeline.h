#pragma once

enum PipelineRegisters { // indexes to buffer array
	FE_DE,
	DE_EXE,
	EXE_MEM,
	MEM_WB,
};

struct Buffer {
	int rt;
	int rs;
	int rd;
	int PC;
	int simm;
	int aluRes;
	int memOut;
};

typedef struct Pipeline {
	struct Buffer D[4];
	struct Buffer Q[4];
	bool stall[4];
} Pipeline;

/*
	1. buffer struct:
		- has all important fields (some of them are not relevant for all stages)
		- boolean stall (we dont always stall all stages)
	2. struct pipeline which has:
		- 2 arrays of 4 buffers: D array and Q array
	3. update function sets Q = D

	has status struct to keep track over:
		1. number of cycles a pipeline stall was inserted in decode stage
		2. number of cycles a pipeline stall was inserted in mem stage
*/
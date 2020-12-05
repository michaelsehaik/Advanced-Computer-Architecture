#pragma once

#include <stdbool.h>


enum PiplineStages {
	FETCH,
	DECODE,
	EXEC,
	MEM,
	WB,
	NUM_OF_STAGES
};


typedef struct DQ_FF {
	int D;
	int Q;
}DQ_FF;

struct FetchDecoderReg {
	DQ_FF instruction;
	DQ_FF PC;
};

struct DecoderExecuteReg {
	DQ_FF A_val;
	DQ_FF B_val;
	DQ_FF rd;
	DQ_FF opcode;
	DQ_FF PC;
	DQ_FF bubble;
};

struct ExecuteMemoryReg {
	DQ_FF aluRes;
	DQ_FF rd;
	DQ_FF opcode;
	DQ_FF PC;
	DQ_FF bubble;
};

struct MemoryWriteBackReg{
	DQ_FF aluRes;
	DQ_FF memValue;
	DQ_FF rd;
	DQ_FF opcode;
	DQ_FF PC;
	DQ_FF bubble;
};

typedef struct Pipeline {
	struct FetchDecoderReg IF_ID;
	struct DecoderExecuteReg ID_EX;
	struct ExecuteMemoryReg EX_MEM;
	struct MemoryWriteBackReg MEM_WB;
	bool stall[NUM_OF_STAGES];
	bool halt[NUM_OF_STAGES];
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

void pipeline__init(Pipeline *pipeline);

/**
* update pipeline registers, for each register, move D values to Q values
*/
void pipeline__update(Pipeline* pipeline);
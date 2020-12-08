#pragma once

#include <stdbool.h>


typedef enum PiplineStage {
	FETCH,
	DECODE,
	EXEC,
	MEM,
	WB,
	NUM_OF_STAGES
} PiplineStage;


typedef struct DQ_FF {
	int D;
	int Q;
}DQ_FF;

struct FetchDecoderReg {
	DQ_FF instruction;
	DQ_FF PC;
	DQ_FF valid;
};

struct DecoderExecuteReg {
	DQ_FF A_val;
	DQ_FF B_val;
	DQ_FF rd;
	DQ_FF opcode;
	DQ_FF PC;
	DQ_FF valid;
};

struct ExecuteMemoryReg {
	DQ_FF aluRes;
	DQ_FF rd;
	DQ_FF opcode;
	DQ_FF PC;
	DQ_FF valid;
};

struct MemoryWriteBackReg{
	DQ_FF aluRes;
	DQ_FF memValue;
	DQ_FF rd;
	DQ_FF memOpSuccess;
	DQ_FF opcode;
	DQ_FF PC;
	DQ_FF valid;
};

typedef struct Pipeline {
	struct FetchDecoderReg IF_ID;
	struct DecoderExecuteReg ID_EX;
	struct ExecuteMemoryReg EX_MEM;
	struct MemoryWriteBackReg MEM_WB;
	bool memStall;
	bool decodeStall;
	int decodeStallCount;
	int memStallCount;
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
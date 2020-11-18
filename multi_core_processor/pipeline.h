#pragma once

#include <stdbool.h>

enum PipelineRegisters { // indexes to buffer array
	FE_DE,
	DE_EXE,
	EXE_MEM,
	MEM_WB,
	NUM_OF_PIPELINE_REGS
};

struct Buffer {
	int rt;
	int rs;
	int rd;
	int PC;
	int simm;
	int aluRes;
	int memOut;
	bool valid;
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
};

struct ExecuteMemoryReg {
	DQ_FF aluRes;
	DQ_FF rd;
};

struct MemoryWriteBackReg{
	DQ_FF rd;
};

typedef struct Pipeline {
	struct FetchDecoderReg IF_ID;
	struct DecoderExecuteReg ID_EX;
	struct ExecuteMemoryReg EX_MEM;
	struct MemoryWriteBackReg MEM_WB;
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

void pipeline__init(Pipeline *pipeline);

/**
* update pipeline registers, for each register, move D values to Q values
*/
void pipeline__update(Pipeline* pipeline);
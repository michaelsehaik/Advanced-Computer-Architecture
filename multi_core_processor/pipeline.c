#include "pipeline.h"

void pipeline__init(Pipeline *pipeline) {
	memset(pipeline, 0, sizeof(Pipeline));
}

void pipeline__setStall(Pipeline *pipeline, PiplineStage stage, bool value) {
	while (stage >= 0) {
		pipeline->stall[stage] = value;
		stage--;
	}
}

void FF_DtoQ(DQ_FF* FF) {
	FF->Q = FF->D;
}

/**
* move all D values to Q values in register (array of numOfFF flip flops)
*/
void register_DtoQ(DQ_FF* FFs, int numOfFF) {
	for (int i = 0; i < numOfFF; i++) {
		FF_DtoQ(&FFs[i]);
	}
}

void pipeline__update(Pipeline* pipeline) {
	if(!(pipeline->stall[DECODE] || pipeline->stall[MEM]))
		register_DtoQ((DQ_FF*)(&pipeline->IF_ID), sizeof(struct FetchDecoderReg)/sizeof(DQ_FF));
	if (!(pipeline->stall[MEM]))
		register_DtoQ((DQ_FF*)(&pipeline->ID_EX), sizeof(struct DecoderExecuteReg) / sizeof(DQ_FF));
	if (!pipeline->stall[MEM])
		register_DtoQ((DQ_FF*)(&pipeline->EX_MEM), sizeof(struct ExecuteMemoryReg) / sizeof(DQ_FF));
	if (!pipeline->stall[MEM])
		register_DtoQ((DQ_FF*)(&pipeline->MEM_WB), sizeof(struct MemoryWriteBackReg) / sizeof(DQ_FF));
}
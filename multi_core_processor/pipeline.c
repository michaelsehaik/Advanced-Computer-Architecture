#include "pipeline.h"

void pipeline__init(Pipeline *pipeline) {
	//for (int i = 0; i < NUM_OF_PIPELINE_REGS; i++) {
	//	pipeline->D[i].valid = false;
	//	pipeline->Q[i].valid = false;
	//}
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
	register_DtoQ((DQ_FF*)(&pipeline->IF_ID), sizeof(struct FetchDecoderReg)/sizeof(DQ_FF));
	register_DtoQ((DQ_FF*)(&pipeline->ID_EX), sizeof(struct DecoderExecuteReg) / sizeof(DQ_FF));
	register_DtoQ((DQ_FF*)(&pipeline->EX_MEM), sizeof(struct ExecuteMemoryReg) / sizeof(DQ_FF));
	register_DtoQ((DQ_FF*)(&pipeline->MEM_WB), sizeof(struct MemoryWriteBackReg) / sizeof(DQ_FF));
}
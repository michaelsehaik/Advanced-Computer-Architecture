#include "core.h"

int signExtension(int imm) {
	int signBit = imm & 0x800;
	if (signBit == 0) {
		return imm;
	}
	return imm | 0xFFFFF000;
}

int calcAluRes(int A, int B, OpCode opcode) {
	unsigned int x;
	switch (opcode)
	{
	case ADD:
		return A + B;
	case SUB:
		return A - B;
	case AND:
		return A & B;
	case OR:
		return A | B;
	case XOR:
		return A ^ B;
	case MUL:
		return A * B;
	case SLL:
		return A << B;
	case SRA:
		return A >> B;
	case SRL:
		// TODO : check this function, if doesn't work, Lami is guilty
		x = (unsigned int)A;
		return x >> B;
	case LW:
	case SW:
	case LL:
	case SC:
		return A + B;
	default:
		return 0;
	}
}

int calcNeedToJump(Core* core, int R_rs, int R_rt, OpCode opcode) {
	switch (opcode)
	{
	case BEQ:
		return R_rs == R_rt;
	case BNE:
		return R_rs != R_rt;
	case BLT:
		return R_rs < R_rt;
	case BGT:
		return R_rs > R_rt;
	case BLE:
		return R_rs <= R_rt;
	case BGE:
		return R_rs >= R_rt;
	case JAL:
		core->registers[15].D = core->PC.Q;
		return 1;
	default:
		return 0;
	}
}

CACHE_OPERATION_NAME getCacheOpName(OpCode opcode) {
	switch (opcode)
	{
	case LW:
		return LOAD_WORD;
	case LL:
		return LOAD_LINKED;
	case SW:
		return STORE_WORD;
	case SC:
		return STORE_CONDITIONAL;
	}
}

bool checkWriteRdForOpcode(OpCode opcode) {

	if (opcode == ADD || opcode == SUB || opcode == AND || opcode == OR || opcode == XOR || opcode == MUL ||
		opcode == SLL || opcode == SRA || opcode == SRL || opcode == LW || opcode == LL) {
		return true;
	}
	return false;
}

bool checkStoreOpcode(OpCode opcode) {

	if (opcode == SW || opcode == SC) {
		return true;
	}
	return false;
}

bool checkDecodeStall(Core* core, OpCode opcode, int rd, int rs, int rt) { 
	bool stall = false;
	
	int rd_MEM_WB = core->pipeline.MEM_WB.rd.Q;
	int rd_EX_MEM = core->pipeline.EX_MEM.rd.Q;
	int rd_ID_EX = core->pipeline.ID_EX.rd.Q;

	if (checkWriteRdForOpcode(core->pipeline.MEM_WB.opcode.Q) && core->pipeline.MEM_WB.valid.Q) { // check if need to write to rd in WB stage
		if (rd_MEM_WB == rs || rd_MEM_WB == rt) stall = true; // read register that should be written before
		if (checkStoreOpcode(opcode) && rd == rd_MEM_WB) stall = true; // store register that should be written before
	}
	if (checkWriteRdForOpcode(core->pipeline.EX_MEM.opcode.Q) && core->pipeline.EX_MEM.valid.Q) {
		if (rd_EX_MEM == rs || rd_EX_MEM == rt) stall = true; // read register that should be written before
		if (checkStoreOpcode(opcode) && rd == rd_EX_MEM) stall = true; // store register that should be written before
	}
	if (checkWriteRdForOpcode(core->pipeline.ID_EX.opcode.Q) && core->pipeline.ID_EX.valid.Q) {
		if (rd_ID_EX == rs || rd_ID_EX == rt) stall = true; // read register that should be written before
		if (checkStoreOpcode(opcode) && rd == rd_ID_EX) stall = true; // store register that should be written before
	}
		
	//printf("core %d: decode stall=%d\n", core->cache.origID, stall);

	core->pipeline.decodeStall = stall;
	return stall;
}

void doFetchStage(Core* core) {
	if (core->halt) {
		core->pipeline.IF_ID.valid.D = false;
		return;
	}
	if (core->pipeline.decodeStall || core->pipeline.memStall) {
		return;
	}
	
	core->instructionCount++;
	core->pipeline.IF_ID.valid.D = true;
	core->pipeline.IF_ID.instruction.D = core->Imem[core->PC.Q];
	core->pipeline.IF_ID.PC.D = core->PC.Q;
	if (core->PC.Q == core->PC.D) { // PC hasn't changed on decode stage
		core->PC.D++;
	}
}

void doDecodeStage(Core *core) {
	if (core->pipeline.memStall) {
		return;
	}
	if (!core->pipeline.IF_ID.valid.Q) {
		core->pipeline.ID_EX.valid.D = false;
		return;
	}
	core->pipeline.ID_EX.valid.D = true;

	core->pipeline.ID_EX.PC.D = core->pipeline.IF_ID.PC.Q;
	int Instruction = core->pipeline.IF_ID.instruction.Q;
	OpCode opcode = (Instruction >> 24) & 0xFF;
	int rd = (Instruction >> 20) & 0xF;
	int rs = (Instruction >> 16) & 0xF;
	int rt = (Instruction >> 12) & 0xF;
	int imm = (Instruction >> 0) & 0xFFF;
	core->pipeline.ID_EX.opcode.D = opcode;

	if (opcode == HALT) {
		core->halt = true;
		return; 
	}

	if (checkDecodeStall(core, opcode, rd, rs, rt)) {
		core->pipeline.ID_EX.valid.D = false;
		core->pipeline.decodeStallCount++;
		return; 
	}

	core->registers[IMM_REG].D = signExtension(imm);
	core->registers[IMM_REG].Q = core->registers[IMM_REG].D; // special case (it's not a register it is a wire..)
	core->pipeline.ID_EX.A_val.D = core->registers[rs].Q;
	core->pipeline.ID_EX.B_val.D = core->registers[rt].Q;
	core->pipeline.ID_EX.rd.D = rd;
	int needToJump = calcNeedToJump(core, core->registers[rs].Q, core->registers[rt].Q, opcode); //Set according to branch commands comparison
	if (needToJump) {
		int jumpDestination = core->registers[rd].Q & 0x03FF;
		//printf("core %d: jumpDestination: %d\n", core->cache.origID, jumpDestination);
		core->PC.D = jumpDestination;
	}
}

void doExecuteStage(Core *core) {
	if (core->pipeline.memStall) {
		return;
	}
	if (!core->pipeline.ID_EX.valid.Q) {
		core->pipeline.EX_MEM.valid.D = false;
		return;
	}
	core->pipeline.EX_MEM.valid.D = true;

	OpCode opcode = core->pipeline.ID_EX.opcode.Q;
	core->pipeline.EX_MEM.PC.D = core->pipeline.ID_EX.PC.Q;
	core->pipeline.EX_MEM.opcode.D = core->pipeline.ID_EX.opcode.Q;
	int aluRes = calcAluRes(core->pipeline.ID_EX.A_val.Q, core->pipeline.ID_EX.B_val.Q, opcode); // Call to ALU function base or opcode
	//printf("aluRes=%d\n", aluRes);
	core->pipeline.EX_MEM.aluRes.D = aluRes;
	core->pipeline.EX_MEM.rd.D = core->pipeline.ID_EX.rd.Q;	
}

void memoryManage(Core* core) {
	OpCode opcode = core->pipeline.EX_MEM.opcode.Q;
	int address = core->pipeline.EX_MEM.aluRes.Q;
	int data = core->registers[core->pipeline.EX_MEM.rd.Q].Q;

	if (core->cache.state == IDLE_S) {
		if (opcode == SC && !(address == core->linkRegister.address && core->linkRegister.flag)) {
			core->registers[core->pipeline.EX_MEM.rd.Q].D = 0; // failure
			return;
		}
		if (cache__setNewOperation(&core->cache, address, data, getCacheOpName(opcode))) {
			core->pipeline.memStall = true;
			core->pipeline.MEM_WB.valid.D = false;
		}
		else {
			core->pipeline.MEM_WB.valid.D = true;
			core->pipeline.MEM_WB.memValue.D = core->cache.curOperation.data; // data operation completed
		}
	}
	else if (core->cache.state == DATA_READY_S) {
		//printf("core %d: data is ready %d\n", core->cache.origID, core->cache.curOperation.data);
		core->pipeline.MEM_WB.memValue.D = core->cache.curOperation.data;
		core->pipeline.memStall = false;
		core->pipeline.MEM_WB.valid.D = true;
	}
}

void doMemStage(Core *core) {
	if (!core->pipeline.EX_MEM.valid.Q && core->cache.state == IDLE_S) {
		core->pipeline.MEM_WB.valid.D = false;
		return;
	}

	OpCode opcode = core->pipeline.EX_MEM.opcode.Q;
	core->pipeline.MEM_WB.opcode.D = core->pipeline.EX_MEM.opcode.Q;
	core->pipeline.MEM_WB.PC.D = core->pipeline.EX_MEM.PC.Q;
	core->pipeline.MEM_WB.aluRes.D = core->pipeline.EX_MEM.aluRes.Q;
	core->pipeline.MEM_WB.rd.D = core->pipeline.EX_MEM.rd.Q;

	if (opcode == LW || opcode == SW || opcode == LL || opcode == SC) {
		memoryManage(core);
	} 
	else {
		core->pipeline.MEM_WB.valid.D = true;
	}
	if (core->pipeline.memStall) {
		core->pipeline.memStallCount++;
	}
}

void doWriteBackStage(Core *core) {
	if (!core->pipeline.MEM_WB.valid.Q) {
		return;
	}

	OpCode opcode = core->pipeline.MEM_WB.opcode.Q;
	int rd = core->pipeline.MEM_WB.rd.Q;
	if (rd == 0 || rd == 1) { // Can't write to reg0 and reg1
		return;
	}
	if (opcode == LW || opcode == LL) {
		core->registers[rd].D = core->pipeline.MEM_WB.memValue.Q;
	}
	else if (opcode >= ADD && opcode <= SRL) {
		core->registers[rd].D = core->pipeline.MEM_WB.aluRes.Q;
	}
}

void executeStep(Core *core) {
	doWriteBackStage(core); 
	doMemStage(core);		
	doExecuteStage(core);
	doDecodeStage(core);	
	doFetchStage(core);
}

/*
* Print PC to trace file, print --- if stage stalled or inactive (for the first lines if some stage hasn't start to work yet)
*/
void printPC(Core *core, bool valid, int value) {
	if (!valid) fprintf(core->traceFile, "--- ");
	else fprintf(core->traceFile, "%03X ", value);
}

void updateCoreTrace(Core *core) {
	fprintf(core->traceFile, "%d ", core->clock->cycle);
	printPC(core, !core->halt, core->PC.Q);
	printPC(core, core->pipeline.IF_ID.valid.Q, core->pipeline.IF_ID.PC.Q);
	printPC(core, core->pipeline.ID_EX.valid.Q, core->pipeline.ID_EX.PC.Q);
	printPC(core, core->pipeline.EX_MEM.valid.Q, core->pipeline.EX_MEM.PC.Q);
	printPC(core, core->pipeline.MEM_WB.valid.Q, core->pipeline.MEM_WB.PC.Q);
	printArray(core->traceFile, &core->registers[2], REG_FILE_SIZE-2, false, true); 
	fprintf(core->traceFile, "\n");
}

void core__update(Core *core) {
	if (!core->pipelineIsEmpty) {
		updateCoreTrace(core);	
		executeStep(core);
		register_DtoQ((DQ_FF*)(&core->registers), REG_FILE_SIZE);
		pipeline__update(&core->pipeline);
		core->PC.Q = core->PC.D;
		if (core->halt && !core->pipeline.MEM_WB.valid.Q && core->cache.state == IDLE_S) {
			core->pipelineIsEmpty = true;
			core__terminate(core);
		}
	}
}

void core__init(Core *core,
				MSI_BUS* bus,
				OriginatorID origID,
				char *ImemFilepath,
				char *traceFilepath,
				char *dsramFilepath,
				char *tsramFilepath,
				char *statsFilepath,
				char *regoutFilepath,
				Clock *clock) { 

	pipeline__init(&core->pipeline);
	core->linkRegister.flag = false;
	cache__init(&core->cache, bus, origID, dsramFilepath, tsramFilepath, &core->linkRegister);
	memset(core->registers, 0, REG_FILE_SIZE * sizeof(DQ_FF));
	memset(core->Imem, 0, IMEM_SIZE * sizeof(int));
	core->PC.Q = 0;
	core->PC.D = 0;
	core->waitForCache = false;
	core->waitForWB = false;
	core->clock = clock;
	core->instructionCount = 0;
	core->halt = false;
	core->pipelineIsEmpty = false;
	core->regoutFilepath = regoutFilepath;
	core->statsFilepath = statsFilepath;

	fopen_s(&core->traceFile, traceFilepath, "w");
	FILE *ImemFile = NULL;
	fopen_s(&ImemFile, ImemFilepath, "r");
	loadArrayFromFile(ImemFile, core->Imem, IMEM_SIZE);
	fclose(ImemFile);
}

void printStatsFile(Core *core) { // FIXME: not counting clock cycles, and some other stats
	FILE *statsFile = NULL;
	fopen_s(&statsFile, core->statsFilepath, "w");
	fprintf(statsFile, "cycles %d\n", core->clock->cycle + 1);
	fprintf(statsFile, "instructions %d\n", core->instructionCount);
	fprintf(statsFile, "read_hit %d\n", core->cache.readHitCount);
	fprintf(statsFile, "write_hit %d\n", core->cache.writeHitCount);
	fprintf(statsFile, "read_miss %d\n", core->cache.readMissCount);
	fprintf(statsFile, "write_miss %d\n", core->cache.writeMissCount);
	fprintf(statsFile, "decode_stall %d\n", core->pipeline.decodeStallCount);
	fprintf(statsFile, "mem_stall %d\n", core->pipeline.memStallCount);
	fclose(statsFile);
}

void core__terminate(Core *core) {
	cache__terminate(&core->cache);
	fclose(core->traceFile);
	createFileFromArray(core->regoutFilepath, &core->registers[2], REG_FILE_SIZE-2, true, true); 
	printStatsFile(core);
}
#include "core.h"

int signExtension(int imm) {
	int signBit = imm & 0x800;
	if (signBit == 0) {
		return imm;
	}
	return imm | 0xFFFFF000;
}

void doFetchStage(Core* core) {
	if (core->pipeline.stall[DECODE]) {
		core->pipeline.stall[FETCH] = true;
		return;
	}
	if (!core->halt) {
		core->pipeline.stall[FETCH] = false;
		core->pipeline.IF_ID.instruction.D = core->Imem[core->PC.Q];
		core->pipeline.IF_ID.PC.D = core->PC.Q;
		core->PC.D++;
	}
}

bool doDecodeStage(Core *core) {
	if (core->pipeline.stall[EXEC]) {
		core->pipeline.stall[DECODE] = true;
		return false; // TODO: check this value
	}	
	int Instruction = core->pipeline.IF_ID.instruction.Q;
	OpCode opcode = (Instruction >> 24) & 0xFF;
	int rd = (Instruction >> 20) & 0xF;
	int rs = (Instruction >> 16) & 0xF;
	int rt = (Instruction >> 12) & 0xF;
	int imm = (Instruction >> 0) & 0xFFF;
	if (opcode == HALT || core->halt) {
		core->halt = true;
		return false; // TODO: check this value
	}
	core->pipeline.stall[DECODE] = checkDecodeStall(core, opcode, rd, rs, rt);
	if (core->pipeline.stall[DECODE]) {
		return false; // TODO: check this value
	}
	core->registers[IMM_REG] = signExtension(imm);
	core->pipeline.ID_EX.A_val.D = core->registers[rs];
	core->pipeline.ID_EX.B_val.D = core->registers[rt];
	core->pipeline.ID_EX.rd.D = rd;
	core->pipeline.ID_EX.opcode.D = opcode;
	int needToJump = calcNeedToJump(core, core->registers[rs], core->registers[rt], opcode); //Set according to branch commands comparison
	if (needToJump) {
		int jumpDestination = core->registers[rd] & 0x03FF;
		core->PC.D = jumpDestination;
	}
	return true; // should we stall?
}

void doExecuteStage(Core *core) {
	if (core->pipeline.stall[MEM]) {
		core->pipeline.stall[EXEC] = true;
		return;
	} 
	core->pipeline.stall[EXEC] = false;
	int aluRes = calcAluRes(core->pipeline.ID_EX.A_val.Q, core->pipeline.ID_EX.B_val.Q, core->pipeline.ID_EX.opcode.Q); // Call to ALU function base or opcode
	core->pipeline.EX_MEM.aluRes.D = aluRes;
	core->pipeline.EX_MEM.rd.D = core->pipeline.ID_EX.rd.Q;
	core->pipeline.EX_MEM.opcode.D = core->pipeline.ID_EX.opcode.Q;
}

bool doMemStage(Core *core) {
	// FIXME : if stall return; (need to check with cache somehow)
	// see if we are waiting for bus
	// call sendRequest(...) method of bus - this does not mean request is granted because of priorities!
	int memValue = memoryManage(core);
	core->pipeline.MEM_WB.aluRes.D = core->pipeline.EX_MEM.aluRes.Q;
	core->pipeline.MEM_WB.memValue.D = memValue;
	core->pipeline.MEM_WB.rd.D = core->pipeline.EX_MEM.rd.Q;
	core->pipeline.MEM_WB.opcode.D = core->pipeline.EX_MEM.opcode.Q;
	return false; // sould we stall?
}

void doWriteBackStage(Core *core) {
	int rd = core->pipeline.MEM_WB.rd.Q;
	if (rd == 0 || rd == 1) { // Can't write to reg0 and reg1
		return;
	}
	OpCode opcode = core->pipeline.MEM_WB.opcode.Q;
	switch (opcode)
	{
	case LW:
	case LL: // check load linked
		core->registers[rd] = core->pipeline.MEM_WB.memValue.Q;
	case ADD:
	case SUB:
	case AND:
	case OR:
	case XOR:
	case MUL:
	case SLL:
	case SRA:
	case SRL:
		core->registers[rd] = core->pipeline.MEM_WB.aluRes.Q;
	default:
		break;
	}
}

bool executeStep(Core *core) {
	doWriteBackStage(core);
	doMemStage(core);		// do we need to stall pipeline?
	doExecuteStage(core);
	doDecodeStage(core);	// do we need to stall D+F?
	doFetchStage(core);
	return core->halt;
}

void printPC(Core *core, bool stall, int value) {
	if (stall) fprintf(core->traceFile, "--- ");
	else fprintf(core->traceFile, "%03X ", value);
}

void updateCoreTrace(Core *core) {
	fprintf(core->traceFile, "%d ", core->clock->cycle);
	printPC(core, core->pipeline.stall[FETCH], core->PC.Q);
	printPC(core, core->pipeline.stall[DECODE], core->pipeline.IF_ID.PC.Q);
	printPC(core, core->pipeline.stall[EXEC], core->pipeline.ID_EX.PC.Q);
	printPC(core, core->pipeline.stall[MEM], core->pipeline.EX_MEM.PC.Q);
	printPC(core, core->pipeline.stall[WB], core->pipeline.MEM_WB.PC.Q);
	printArray(core->traceFile, &core->registers[2], REG_FILE_SIZE-2, false); //TODO change registers[2] to R2 enum
	fprintf(core->traceFile, "\n");
}

bool core__update(Core *core) {
	bool halt;
	updateCoreTrace(core);
	cache__update(&core->cache);
	halt = executeStep(core);
	core->PC.Q = core->PC.D;
	return halt;
}

void core__init(Core *core,
				MSI_BUS* bus,
				char *ImemFilepath,
				char *traceFilepath,
				char *dsramFilepath,
				char *tsramFilepath,
				char *statsFilepath,
				char *regoutFilepath,
				Clock *clock) { 

	pipeline__init(&core->pipeline);
	cache__init(&core->cache, bus, dsramFilepath, tsramFilepath);
	memset(core->registers, 0, REG_FILE_SIZE * sizeof(int));
	memset(core->Imem, 0, IMEM_SIZE * sizeof(int));
	core->PC.Q = 0;
	core->waitForCache = false;
	core->waitForWB = false;
	core->regoutFilepath = regoutFilepath;
	core->statsFilepath = statsFilepath;
	core->clock = clock;
	core->instructionCount = 0;
	core->decodeStallCount = 0;
	core->memStallCount = 0;

	fopen_s(&core->traceFile, traceFilepath, "w");
	FILE *ImemFile = NULL;
	fopen_s(&ImemFile, ImemFilepath, "r");
	loadArrayFromFile(ImemFile, core->Imem, IMEM_SIZE);
	fclose(ImemFile);
}

void printStatsFile(Core *core) {
	FILE *statsFile = NULL;
	fopen_s(&statsFile, core->statsFilepath, "w");
	fprintf(statsFile, "cycles %d\n", core->clock->cycle);
	fprintf(statsFile, "instructions %d\n", core->instructionCount);
	fprintf(statsFile, "read_hit %d\n", core->cache.readHitCount);
	fprintf(statsFile, "write_hit %d\n", core->cache.writeHitCount);
	fprintf(statsFile, "read_miss %d\n", core->cache.readMissCount);
	fprintf(statsFile, "write_miss %d\n", core->cache.writeMissCount);
	fprintf(statsFile, "decode_stall %d\n", core->decodeStallCount);
	fprintf(statsFile, "mem_stall %d\n", core->memStallCount);
	fclose(statsFile);
}

void core__terminate(Core *core) {
	cache__terminate(&core->cache);
	fclose(core->traceFile);
	createFileFromArray(core->regoutFilepath, &core->registers[2], REG_FILE_SIZE-2, true); //TODO change registers[2] to R2 enum
	printStatsFile(core);
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
		core->registers[15] = core->PC.Q;
		return 1;
	default:
		return 0;
	}
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

int memoryManage(Core* core) {
	OpCode opcode = core->pipeline.EX_MEM.opcode.Q;
	switch (opcode)
	{
	case LW:
		// Check if cache has the data, if so return it, otherwise get data from RAM 
	case SW:
		// FIXME: MSI
	case LL:
		// FIXME: MSI
	case SC:
		// FIXME: MSI
	default:
		return 0;
	}
}

bool checkWriteRdForOpcode(OpCode opcode) {

	if (opcode == ADD || opcode == SUB || opcode == AND || opcode == OR || opcode == XOR || opcode == MUL ||
		opcode == SLL || opcode == SRA || opcode == SRL || opcode == LW || opcode == LL) {
		return true;
	}
	return false;
}

bool checkDecodeStall(Core* core, OpCode opcode, int rd, int rs, int rt) {
	int rd_MEM_WB = core->pipeline.MEM_WB.rd.Q;
	int rd_EX_MEM = core->pipeline.EX_MEM.rd.Q;
	int rd_ID_EX = core->pipeline.ID_EX.rd.Q;
	if(checkWriteRdForOpcode(core->pipeline.MEM_WB.opcode.Q)) // check if need to write to rd in WB stage
		if (rd_MEM_WB == rs || rd_MEM_WB == rt) return true; // read register that should be writen before
	if (checkWriteRdForOpcode(core->pipeline.EX_MEM.opcode.Q))
		if (rd_EX_MEM == rs || rd_EX_MEM == rt) return true; // read register that should be writen before
	if (checkWriteRdForOpcode(core->pipeline.ID_EX.opcode.Q))
		if (rd_ID_EX == rs || rd_ID_EX == rt) return true; // read register that should be writen before
	
	return false;
}
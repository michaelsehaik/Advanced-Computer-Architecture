#include "core.h"
#include "util.h"

int signExtension(int imm) {
	int signBit = imm & 0x800;
	if (signBit == 0) {
		return imm;
	}
	return imm | 0xFFFFF000;
}

void doFetchStage(Core *core) {
	// FIXME : if stall return;
	core->pipeline.IF_ID.instruction.D = core->Imem[core->PC];
	core->pipeline.IF_ID.PC.D = core->PC;
	core->PC++;
}

bool doDecodeStage(Core *core) {
	// FIXME : if stall return;
	// decode instruction, set ^^ values
	int Instruction = core->pipeline.IF_ID.instruction.Q;
	int opcode = (Instruction >> 24) & 0xFF;
	int rd = (Instruction >> 20) & 0xF;
	int rs = (Instruction >> 16) & 0xF;
	int rt = (Instruction >> 12) & 0xF;
	int imm = (Instruction >> 0) & 0xFFF;
	core->registers[IMM_REG] = signExtension(imm);
	core->pipeline.ID_EX.A_val.D = core->registers[rs];
	core->pipeline.ID_EX.B_val.D = core->registers[rt];
	core->pipeline.ID_EX.rd.D = rd;
	core->pipeline.ID_EX.opcode.D = opcode;
	int needToJump = 1; //Set according to branch commands comparison
	int jumpDestination = core->registers[rd] & 0x03FF;
	if (needToJump) {
		core->PC = jumpDestination;
	}
	return false; // should we stall?
}

void doExecuteStage(Core *core) {
	// FIXME : if stall return;
	int aluRes = 0;// Call to ALU function base or opcode
	core->pipeline.EX_MEM.aluRes.D = aluRes;
	core->pipeline.EX_MEM.rd.D = core->pipeline.ID_EX.rd.Q;
	core->pipeline.EX_MEM.opcode.D = core->pipeline.ID_EX.opcode.Q;
}

bool doMemStage(Core *core) {
	// FIXME : if stall return; (need to check with sram somehow)
	//if (core->pipeline.EX_MEM.opcode.Q == lw) {
	//	core->pipeline.MEM_WB.memValue.D = 0; // = lw()
	//} else if(core->pipeline.EX_MEM.opcode.Q == sw){
	//	call sw()
	//}
	core->pipeline.MEM_WB.aluRes.D = core->pipeline.EX_MEM.aluRes.Q;
	core->pipeline.MEM_WB.rd.D = core->pipeline.EX_MEM.rd.Q;
	core->pipeline.MEM_WB.opcode.D = core->pipeline.EX_MEM.opcode.Q;
	// see if we are waiting for bus
	// update pipeline regs if needed
	// call sendRequest(...) method of bus - this does not mean request is granted because of priorities!
	return false; // sould we stall?
}

void doWriteBackStage(Core *core) {
	// FIXME : if stall return;
}

bool executeStep(Core *core) {
	bool halt = false;
	
	doFetchStage(core);
	doDecodeStage(core);	// do we need to stall D+F?
	doExecuteStage(core);
	doMemStage(core);		// do we need to stall pipeline?
	doWriteBackStage(core);
	return halt;
}

void printPC(Core *core, bool stall, int value) {
	if (stall) fprintf(core->traceFile, "--- ");
	else fprintf(core->traceFile, "%03X ", value);
}

void updateCoreTrace(Core *core) {
	fprintf(core->traceFile, "%d ", core->clock->cycle);
	printPC(core, core->pipeline.stall[FETCH], core->PC);
	printPC(core, core->pipeline.stall[DECODE], core->pipeline.IF_ID.PC.Q);
	printPC(core, core->pipeline.stall[EXEC], core->pipeline.ID_EX.PC.Q);
	printPC(core, core->pipeline.stall[MEM], core->pipeline.EX_MEM.PC.Q);
	printPC(core, core->pipeline.stall[WB], core->pipeline.MEM_WB.PC.Q);
	arrayToFile(core->regoutFilepath, core->registers, REG_FILE_SIZE, false);
	fprintf(core->traceFile, "\n");
}

bool core__update(Core *core) {
	bool halt;
	updateCoreTrace(core);
	sram__update(&core->SRAM);
	halt = executeStep(core);

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
	sram__init(&core->SRAM, bus, dsramFilepath, tsramFilepath);
	memset(core->registers, 0, REG_FILE_SIZE * sizeof(int));
	memset(core->Imem, 0, IMEM_SIZE * sizeof(int));
	core->PC = 0;
	core->waitForSRAM = false;
	core->waitForWB = false;
	core->regoutFilepath = regoutFilepath;
	core->statsFilepath = statsFilepath;
	core->clock = clock;
	core->instructionCount = 0;
	core->decodeStallCount = 0;
	core->memStallCount = 0;

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
	fprintf(statsFile, "read_hit %d\n", core->SRAM.readHitCount);
	fprintf(statsFile, "write_hit %d\n", core->SRAM.writeHitCount);
	fprintf(statsFile, "read_miss %d\n", core->SRAM.readMissCount);
	fprintf(statsFile, "write_miss %d\n", core->SRAM.writeMissCount);
	fprintf(statsFile, "decode_stall %d\n", core->decodeStallCount);
	fprintf(statsFile, "mem_stall %d\n", core->memStallCount);
	fclose(statsFile);
}

void core__terminate(Core *core) {
	sram__terminate(&core->SRAM);
	fclose(core->traceFile);
	arrayToFile(core->regoutFilepath, core->registers, REG_FILE_SIZE, true);
	printStatsFile(core);
}
#include "core.h"
#include "util.h"

void initCore(Core *core) {

}

int signExtension(int imm) {
	int signBit = imm & 0x800;
	if (signBit == 0) {
		return imm;
	}
	return imm | 0xFFFFF000;
}

void doFetchStage(Core *core) {
	core->pipeline.IF_ID.instruction.D = core->Imem[core->PC];
	core->pipeline.IF_ID.PC.D = core->PC;
	core->PC++;
}

bool doDecodeStage(Core *core) {
	int rd = 0, rs = 0, rt = 0, imm = 0;
	OpCodes opcode;
	// decode instruction, set ^^ values
	int Instruction = core->pipeline.IF_ID.instruction.Q;
	opcode = (Instruction >> 24) & 0xFF;
	rd = (Instruction >> 20) & 0xF;
	rs = (Instruction >> 16) & 0xF;
	rt = (Instruction >> 12) & 0xF;
	imm = (Instruction >> 0) & 0xFFF;
	core->registers[IMM_REG] = signExtension(imm);
	core->pipeline.ID_EX.A_val.D = core->registers[rs];
	core->pipeline.ID_EX.B_val.D = core->registers[rt];
	core->pipeline.ID_EX.rd.D = rd;
	core->pipeline.ID_EX.opcode.D = opcode;
	int needToJump = 0; //Set according to branch commands comparison
	needToJump = calcNeedToJump(core);
	int jumpDestination = core->registers[rd] & 0x03FF;
	if (needToJump) {
		//set PC
	}

	return false; // should we stall?
}

void doExecuteStage(Core *core) {

}

bool doMemStage(Core *core) {
	// see if we are waiting for bus
	// update pipeline regs if needed
	// call sendRequest(...) method of bus - this does not mean request is granted because of priorities!
	return false; // sould we stall?
}

void doWriteBackStage(Core *core) {

}

bool executeStep(Core *core) {
	bool halt = false;
	// if not stalled
		doFetchStage(core);
	doDecodeStage(core);	// do we need to stall D+F?
	doExecuteStage(core);
	doMemStage(core);		// do we need to stall pipeline?
	doWriteBackStage(core);
	return halt;
}

void updateCoreTrace(Core *core) {
	// TODO
}

bool core__update(Core *core) {
	bool halt;
	sram__update(&core->SRAM);
	halt = executeStep(core);
	updateCoreTrace(core);

	return halt;
}

void core__init(Core *core, MSI_BUS* bus, char *imemFilepath, char *traceFilepath, char *dsramFilepath, char *tsramFilepath, char *statsFilepath) {
	pipeline__init(&core->pipeline);
	sram__init(&core->SRAM, bus, dsramFilepath, tsramFilepath);
	memset(core->registers, 0, REG_FILE_SIZE);
	core->PC = 0;
	core->waitForSRAM;
	core->waitForWB;

	FILE *ImemFile = NULL;
	fopen_s(&ImemFile, imemFilepath, "r");
	memset(core->Imem, 0, IMEM_SIZE*sizeof(int));
	loadArrayFromFile(ImemFile, core->Imem, IMEM_SIZE);
	fclose(ImemFile);
}

void core__terminate(Core *core) {
	sram__terminate(&core->SRAM);
	fclose(core->traceFile);
	fclose(core->statsFile);
}

void add(Core *core)
{
}

void sub(Core *core)
{
}

void and(Core *core)
{
}

void or (Core *core)
{
}

void xor(Core *core)
{
}

void mul(Core *core)
{
}

void sll(Core *core)
{
}

void sra(Core *core)
{
}

void srl(Core *core)
{
}

void beq(Core *core)
{
}

void bne(Core *core)
{
}

void blt(Core *core)
{
}

void bgt(Core *core)
{
}

void ble(Core *core)
{
}

void bge(Core *core)
{
}

void jal(Core *core)
{
}

void lw(Core *core)
{
}

void sw(Core *core)
{
}

void ll(Core *core)
{
}

void sc(Core *core)
{
}

void halt(Core *core)
{
}

#include <stdio.h>

typedef enum Opcode {
	ADD, SUB,	AND, OR, XOR, MUL, SLL,	SRA, SRL, BEQ, BNE,	BLT, BGT, BLE, BGE,	JAL, LW, SW, LL, SC, HALT
} Opcode;

typedef enum Register {
	R0,	R1,	R2,	R3,	R4,	R5,	R6,	R7,	R8,	R9,	R10, R11, R12, R13, R14, R15
} Register;

FILE* outputFile;

void addInstruction(Opcode opcode, Register rd, Register rs, Register rt, short imm) {
	fprintf(outputFile, "%02X%X%X%X%03X\n", opcode, rd, rs, rt, imm & 0xFFF);
}

void testProgram1() {
	char* filename = "test_program_1.txt";
	fopen_s(&outputFile, filename, "w");

	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(SUB, R5, R4, R3, 0xF);
	addInstruction(AND, R8, R7, R6, 0x10);
	addInstruction(OR, R11, R10, R9, 0xFF);
	addInstruction(XOR, R14, R13, R12, 0x100);
	addInstruction(MUL, R2, R1, R15, 0xFFF);
	addInstruction(SLL, R2, R1, R2, 0xFEF);
	addInstruction(SRA, R2, R1, R2, 0xBA9);
	addInstruction(SRL, R2, R1, R2, 0x123);
	addInstruction(BEQ, R2, R1, R2, 0x0);
	addInstruction(BNE, R2, R1, R2, 0x111);
	addInstruction(BLT, R2, R1, R2, 0xA);
	addInstruction(BGT, R2, R1, R2, 0xB);
	addInstruction(BLE, R2, R1, R2, 0xC);
	addInstruction(BGE, R2, R1, R2, 0xD);
	addInstruction(JAL, R2, R1, R2, 0xE);
	addInstruction(LW, R2, R1, R2, 0xF);
	addInstruction(SW, R2, R1, R2, 0x10);
	addInstruction(LL, R2, R1, R2, 0x12);
	addInstruction(SC, R2, R1, R2, 0x13);
	addInstruction(HALT, R0, R0, R0, 0x0);

	fclose(outputFile);
}

int main(int argc, char **argv) {
	testProgram1();
	return 0;
}
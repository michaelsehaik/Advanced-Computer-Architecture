#include <stdio.h>

typedef enum Opcode {
	ADD, SUB, AND, OR, XOR, MUL, SLL, SRA, SRL, BEQ, BNE, BLT, BGT, BLE, BGE, JAL, LW, SW, LL, SC, HALT
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

void testLoad() {
	char* filename = "test_load.txt";
	fopen_s(&outputFile, filename, "w");

	addInstruction(LW, R2, R1, R0, 0x0);	// load from 0
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(HALT, R0, R0, R0, 0x0);	// halt

	fclose(outputFile);
}

void testLoadFromCore() {
	char* filename = "test_load_from_core.txt";
	fopen_s(&outputFile, filename, "w");

	addInstruction(LL, R2, R1, R0, 0x0);	// load from 0
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(HALT, R0, R0, R0, 0x0);	// halt

	fclose(outputFile);
}

void testLoadStore() {
	char* filename = "test_load_store.txt";
	fopen_s(&outputFile, filename, "w");

	addInstruction(LW, R2, R1, R0, 0x0);	// load from 0 (bus)
	addInstruction(LW, R2, R1, R0, 0x0);	// load from 0
	addInstruction(LW, R2, R1, R0, 0x0);	// load from 0
	addInstruction(LW, R2, R1, R0, 0x0);	// load from 0
	addInstruction(LW, R2, R1, R0, 0x0);	// load from 0
	addInstruction(ADD, R2, R2, R2, 0x0);	// R2 = R2 + R2
	addInstruction(SW, R2, R1, R0, 0x0);	// store MEM[0]=R2 (bus)
	addInstruction(LW, R2, R1, R0, 0x0);	// load from 0
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(ADD, R2, R1, R0, 0x0);
	addInstruction(HALT, R0, R0, R0, 0x0);	// halt

	fclose(outputFile);
}

void MM88(char* filename, int offsetR14, int offsetR13 , int MemOffset, int lineOffset, int addToPrevCalc) {

	addInstruction(ADD, R8, R1, R0, 0x7 + offsetR13);
	addInstruction(ADD, R9, R1, R0, 0x70 + offsetR14);

	addInstruction(ADD, R14, R1, R0, 0x0 + offsetR14);
	addInstruction(ADD, R13, R1, R0, 0x0 + offsetR13);

	addInstruction(LW, R2, R1, R14, 0x00);
	addInstruction(LW, R3, R1, R14, 0x01);
	addInstruction(LW, R6, R1, R13, 0x100);
	addInstruction(LW, R7, R1, R13, 0x110);
	addInstruction(MUL, R2, R2, R6, 0x00);
	addInstruction(MUL, R3, R3, R7, 0x00);


	addInstruction(LW, R6, R1, R14, 0x02);
	addInstruction(LW, R7, R1, R14, 0x03);
	addInstruction(ADD, R5, R2, R3, 0x0); //change location
	
	addInstruction(LW, R2, R1, R13, 0x120);
	addInstruction(LW, R3, R1, R13, 0x130);
	addInstruction(ADD, R10, R5, R0, 0x0);//change location? init R10 = R2 + 0 
	addInstruction(MUL, R6, R6, R2, 0x00);
	addInstruction(MUL, R7, R7, R3, 0x00);


	addInstruction(LW, R2, R1, R14, 0x00 + 0x04);
	addInstruction(LW, R3, R1, R14, 0x01 + 0x04);
	addInstruction(ADD, R5, R6, R7, 0x0); //change location
	if (addToPrevCalc) {
		addInstruction(LW, R4, R11, R14, 0x0);//change location?
	}
	addInstruction(LW, R6, R1, R13, 0x100 + 0x40);
	addInstruction(LW, R7, R1, R13, 0x110 + 0x40);
	addInstruction(ADD, R10, R5, R10, 0x0);//change location?
	addInstruction(MUL, R2, R2, R6, 0x00);
	addInstruction(MUL, R3, R3, R7, 0x00);


	addInstruction(LW, R6, R1, R14, 0x02 + 0x04);
	addInstruction(LW, R7, R1, R14, 0x03 + 0x04);
	addInstruction(ADD, R5, R2, R3, 0x0); //change location

	if (addToPrevCalc) {
		addInstruction(ADD, R10, R10, R4, 0x0);//change location?
	}
	
	addInstruction(LW, R2, R1, R13, 0x120 + 0x40);
	addInstruction(LW, R3, R1, R13, 0x130 + 0x40);
	addInstruction(ADD, R10, R5, R10, 0x0);//change location?
	addInstruction(MUL, R6, R6, R2, 0x00);
	addInstruction(MUL, R7, R7, R3, 0x00);
	addInstruction(ADD, R11, R13, R1, 0x200 - MemOffset);//change location? init R11 = 0x200 + R13 (200 for 3'rd matrix location.
	addInstruction(ADD, R5, R6, R7, 0x0); //change location
	addInstruction(ADD, R10, R5, R10, 0x0);//change location?




	addInstruction(SW, R10, R11, R14, 0x10);
	

	addInstruction(BLT, R1, R13, R8, 4 + lineOffset);
	addInstruction(ADD, R13, R13, R1, 0x1);

	
	addInstruction(BLT, R1, R14, R9, 3 + lineOffset);
	addInstruction(ADD, R14, R14, R1, 0x10);
}

/*
void MM88old(char* filename, int offsetR14, int offsetR13, int MemOffset, int lineOffset, int addToPrevCalc) {

	addInstruction(ADD, R8, R1, R0, 0x7 + offsetR13);
	addInstruction(ADD, R9, R1, R0, 0x70 + offsetR14);

	addInstruction(ADD, R14, R1, R0, 0x0 + offsetR14);
	addInstruction(ADD, R13, R1, R0, 0x0 + offsetR13);

	addInstruction(LW, R2, R1, R14, 0x00);
	addInstruction(LW, R3, R1, R14, 0x01);
	addInstruction(LW, R6, R1, R13, 0x100);
	addInstruction(LW, R7, R1, R13, 0x110);
	addInstruction(MUL, R2, R2, R6, 0x00);
	addInstruction(MUL, R3, R3, R7, 0x00);
	addInstruction(ADD, R2, R2, R3, 0x0); //change location
	addInstruction(ADD, R10, R2, R0, 0x0);//change location? init R10 = R2 + 0 

	addInstruction(LW, R6, R1, R14, 0x02);
	addInstruction(LW, R7, R1, R14, 0x03);
	addInstruction(LW, R2, R1, R13, 0x120);
	addInstruction(LW, R3, R1, R13, 0x130);
	addInstruction(MUL, R6, R6, R2, 0x00);
	addInstruction(MUL, R7, R7, R3, 0x00);
	addInstruction(ADD, R6, R6, R7, 0x0); //change location
	addInstruction(ADD, R10, R6, R10, 0x0);//change location?

	addInstruction(LW, R2, R1, R14, 0x00 + 0x04);
	addInstruction(LW, R3, R1, R14, 0x01 + 0x04);
	addInstruction(LW, R6, R1, R13, 0x100 + 0x40);
	addInstruction(LW, R7, R1, R13, 0x110 + 0x40);
	addInstruction(MUL, R2, R2, R6, 0x00);
	addInstruction(MUL, R3, R3, R7, 0x00);
	addInstruction(ADD, R2, R2, R3, 0x0); //change location
	addInstruction(ADD, R10, R2, R10, 0x0);//change location?

	addInstruction(LW, R6, R1, R14, 0x02 + 0x04);
	addInstruction(LW, R7, R1, R14, 0x03 + 0x04);
	addInstruction(LW, R2, R1, R13, 0x120 + 0x40);
	addInstruction(LW, R3, R1, R13, 0x130 + 0x40);
	addInstruction(MUL, R6, R6, R2, 0x00);
	addInstruction(MUL, R7, R7, R3, 0x00);
	addInstruction(ADD, R6, R6, R7, 0x0); //change location
	addInstruction(ADD, R10, R6, R10, 0x0);//change location?


	addInstruction(ADD, R11, R13, R1, 0x200 - MemOffset);//change location? init R11 = 0x200 + R13 (200 for 3'rd matrix location.
	if (addToPrevCalc) {
		addInstruction(LW, R4, R11, R14, 0x0);//change location?
		addInstruction(ADD, R10, R10, R4, 0x0);//change location?
	}
	addInstruction(SW, R10, R11, R14, 0x10);


	addInstruction(BLT, R1, R13, R8, 4 + lineOffset);
	addInstruction(ADD, R13, R13, R1, 0x1);


	addInstruction(BLT, R1, R14, R9, 3 + lineOffset);
	addInstruction(ADD, R14, R14, R1, 0x10);
}
*/

void MM1Core() { //Matrix Multiplication 1 Core
	char* filename = "MM1Core.txt";
	fopen_s(&outputFile, filename, "w");

	MM88(filename, 0         , 0         , 0   , 0 * 42, 0);
	MM88(filename, 0         , 0x8       , 0   , 1 * 42, 0);
	MM88(filename, 0x80      , 0         , 0   , 2 * 42, 0);
	MM88(filename, 0x80      , 0x8       , 0   , 3 * 42, 0);
	MM88(filename, 0x8       , 0x80      , 0x88, 4 * 42, 1);
	MM88(filename, 0x8       , 0x80 + 0x8, 0x88, 5 * 42 + 2, 1);
	MM88(filename, 0x8 + 0x80, 0x80      , 0x88, 6 * 42 + 4, 1);
	MM88(filename, 0x8 + 0x80, 0x80 + 0x8, 0x88, 7 * 42 + 6, 1);

	for (int i = 0; i < 256; i++)
	{
		addInstruction(LW, R2, R1, R0, 0x100 + i);
	}
	

	addInstruction(HALT, R0, R0, R0, 0x0);
	fclose(outputFile);
}

void MM4Cores() { //Matrix Multiplication 1 Core

	char* filename0 = "MM4Core0.txt";
	fopen_s(&outputFile, filename0, "w");
	MM88(filename0, 0, 0, 0, 0, 0);
	MM88(filename0, 0x8, 0x80, 0x88, 42, 1);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			addInstruction(LW, R2, R1, R0, 0x100 + 16 * i + j);
		}
	}
	addInstruction(HALT, R0, R0, R0, 0x0);
	fclose(outputFile);

	char* filename1 = "MM4Core1.txt";
	fopen_s(&outputFile, filename1, "w");
	MM88(filename1, 0, 0x8, 0, 0, 0);
	MM88(filename1, 0x8, 0x80 + 0x8, 0x88, 42, 1);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			addInstruction(LW, R2, R1, R0, 0x100 + 8 + 16*i + j);
		}
	}
	addInstruction(HALT, R0, R0, R0, 0x0);
	fclose(outputFile);

	char* filename2 = "MM4Core2.txt";
	fopen_s(&outputFile, filename2, "w");
	MM88(filename2, 0x80, 0, 0, 0, 0);
	MM88(filename2, 0x8 + 0x80, 0x80, 0x88, 42, 1);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			addInstruction(LW, R2, R1, R0, 0x100 + 128 + 16 * i + j);
		}
	}
	addInstruction(HALT, R0, R0, R0, 0x0);
	fclose(outputFile);

	char* filename3 = "MM4Core3.txt";
	fopen_s(&outputFile, filename3, "w");
	MM88(filename3, 0x80, 0x8, 0, 0, 0);
	MM88(filename3, 0x8 + 0x80, 0x80 + 0x8, 0x88, 42, 1);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			addInstruction(LW, R2, R1, R0, 0x100 +  128 + 8 + 16 * i + j);
		}
	}
	addInstruction(HALT, R0, R0, R0, 0x0);
	fclose(outputFile);

}

void testMM8() { //Matrix Multiplication 1 Core

	char* filename0 = "testMM8.txt";
	fopen_s(&outputFile, filename0, "w");
	MM88(filename0, 0, 0, 0, 0, 0);
	addInstruction(HALT, R0, R0, R0, 0x0);
	fclose(outputFile);
}

/*
void Program1MM1Core() { //Matrix Multiplication 1 Core
	char* filename = "MM1Core.txt";
	fopen_s(&outputFile, filename, "w");
	
	addInstruction(ADD, R12, R1, R0, 0x0);
	addInstruction(ADD, R11, R1, R0, 0x0);
	//addInstruction(ADD, R10, R1, R0, 0x0); //R10  = result. 
loop3:
	addInstruction(ADD, R14, R1, R0, 0x0);
loop2:
	addInstruction(ADD, R13, R1, R0, 0x0);
loop1:
	addInstruction(LW, R2, R1, R14, 0x00);
	addInstruction(LW, R3, R1, R14, 0x01);
	addInstruction(LW, R6, R1, R13, 0x100);
	addInstruction(LW, R7, R1, R13, 0x110);
	addInstruction(MUL, R2, R2, R6, 0x00);
	addInstruction(MUL, R3, R3, R7, 0x00);
	addInstruction(ADD, R2, R2, R3, 0x0); //change location
	addInstruction(ADD, R10, R2, R0, 0x0);//change location? init R10 = R2 + 0 

	addInstruction(LW, R6, R1, R14, 0x02);
	addInstruction(LW, R7, R1, R14, 0x03);
	addInstruction(LW, R2, R1, R13, 0x120);
	addInstruction(LW, R3, R1, R13, 0x130);
	addInstruction(MUL, R6, R6, R2, 0x00);
	addInstruction(MUL, R7, R7, R3, 0x00);
	addInstruction(ADD, R6, R6, R7, 0x0); //change location
	addInstruction(ADD, R10, R6, R10, 0x0);//change location?

	addInstruction(LW, R2, R1, R14, 0x00 + 0x04);
	addInstruction(LW, R3, R1, R14, 0x01 + 0x04);
	addInstruction(LW, R6, R1, R13, 0x100 + 0x40);
	addInstruction(LW, R7, R1, R13, 0x110 + 0x40);
	addInstruction(MUL, R2, R2, R6, 0x00);
	addInstruction(MUL, R3, R3, R7, 0x00);
	addInstruction(ADD, R2, R2, R3, 0x0); //change location
	addInstruction(ADD, R10, R2, R10, 0x0);//change location?

	addInstruction(LW, R6, R1, R14, 0x02 + 0x04);
	addInstruction(LW, R7, R1, R14, 0x03 + 0x04);
	addInstruction(LW, R2, R1, R13, 0x120 + 0x40);
	addInstruction(LW, R3, R1, R13, 0x130 + 0x40);
	addInstruction(MUL, R6, R6, R2, 0x00);
	addInstruction(MUL, R7, R7, R3, 0x00);
	addInstruction(ADD, R6, R6, R7, 0x0); //change location
	addInstruction(ADD, R10, R6, R10, 0x0);//change location?

	addInstruction(ADD, R11, R13, R1, 0x200);//change location? init R11 = 0x200 + R13 (200 for 3'rd matrix location.
	addInstruction(SW, R10, R11, R14, 0x10);

	addInstruction(ADD, R2, R1, R0, 0x8);
	addInstruction(BLT, R1, R13, R2, loop1);
	addInstruction(ADD, R13, R13, R1, 0x1);

	addInstruction(ADD, R2, R1, R0, 0x80);
	addInstruction(BLT, R1, R14, R2, loop2);
	addInstruction(ADD, R14, R14, R1, 0x10);


	addInstruction(HALT, R0, R0, R0, 0x0);
	fclose(outputFile);
}
*/

void LLAndSC() {
	char* filename0 = "llsc.txt";
	fopen_s(&outputFile, filename0, "w");

	// INIT
	addInstruction(ADD, R2, R0, R1, 0x7F); // 127
	addInstruction(LL, R3, R0, R0, 0x0);	// load to R3
	// Label = TRY (2)
	addInstruction(ADD, R3, R3, R1, 0x1);	// R3++
	addInstruction(SC, R3, R0, R0, 0x0);	// store R3
	addInstruction(BEQ, R1, R3, R0, 0x2);	// if R3==0 goto TRY
	addInstruction(LL, R3, R0, R0, 0x0);	// load to R3

	addInstruction(BNE, R1, R2, R0, 0x1);	// if R2!=0 goto TRY
	addInstruction(SUB, R2, R2, R1, 0x1);	// R2--

	addInstruction(LW, R3, R0, R1, 0x100);  // force conflict flush
	addInstruction(HALT, R0, R0, R0, 0x0);
	fclose(outputFile);
}

int main(int argc, char **argv) {
	testProgram1();
	MM1Core();
	MM4Cores();
	testMM8();
	testLoad();
	testLoadFromCore();
	testLoadStore();
	LLAndSC();

	return 0;
}
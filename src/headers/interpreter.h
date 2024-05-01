#pragma once
#include "file.h"
#include "input.h"
#include "output.h"

#define MEM_SIZE 0x1000ul
#define ENTRY_POINT 0x0200ul
#define STACK_START 0x0ul
#define STACK_SIZE (16 * sizeof(unsigned short))
#define NUM_SPRITES_START 0x0100ul
#define NUM_SPRITES_SIZE 5
#define MAX_PROG_SIZE (MEM_SIZE - ENTRY_POINT)

enum InitializeResult { INIT_OK, INIT_SIZE_ERR };
enum ExecuteResult
{
	EXEC_OK,
	EXEC_WAIT_INPUT,
	EXEC_RET_EMPTY_STACK,
	EXEC_INVALID_INSTRUCTION,
	EXEC_STACK_OVERFLOW
};

struct Interpreter {
	unsigned char* memory;
	unsigned char genRegs[16];
	unsigned short addrReg;
	unsigned char delayTimer;
	unsigned char soundTimer;
	unsigned short pc;
	unsigned char sp;
	struct ButtonStates bs;
	struct Screen scr;
};
void Interpreter_free(struct Interpreter* toFree);
unsigned short getSpriteIndex(unsigned char value);
enum InitializeResult initInterpreter(struct Interpreter* interpreter, struct BinRead* readData);
enum ExecuteResult execNextInstruction(struct Interpreter* interpreter, unsigned char** keyReg);

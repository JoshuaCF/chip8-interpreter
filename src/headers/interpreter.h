#pragma once
#include "file.h"
#include "input.h"

#define MEM_SIZE 0x1000ul
#define ENTRY_POINT 0x0200ul
#define STACK_START 0x0ul
#define STACK_SIZE (16 * sizeof(unsigned short))
#define MAX_PROG_SIZE (MEM_SIZE - ENTRY_POINT)

enum InitializeResult { INIT_OK, INIT_SIZE_ERR };
enum ExecuteResult
{
	EXEC_OK,
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
};
void Interpreter_free(struct Interpreter* toFree);
enum InitializeResult initInterpreter(struct Interpreter* interpreter, struct BinRead* readData);
enum ExecuteResult execNextInstruction(struct Interpreter* interpreter);

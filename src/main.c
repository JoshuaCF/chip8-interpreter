#include "headers/file.h"
#include "headers/interpreter.h"

#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"

enum argStatus { OK, TOO_FEW, TOO_MANY };
enum argStatus verifyArgs(int argc)
{
	if(argc > 2) return TOO_MANY;
	if(argc < 2) return TOO_FEW;
	return OK;
}

int main(int argc, char* argv[])
{
	enum argStatus status = verifyArgs(argc);
	switch(status)
	{
		case TOO_FEW:
			printf("Too few arguments passed to program.\n");
			return 1;
			break;
		case TOO_MANY:
			printf("Too many arguments passed to the program.\n");
			return 1;
			break;
		case OK:
			break;
	}

	char* filename = argv[1];
	printf("Running file '%s'...\n", filename);

	struct BinRead read;
	switch(getFileContents(filename, &read))
	{
		case READ_ERR:
			printf("Error when reading file.\n");
			return 1;
			break;
		default:
			break;
	}

	struct Interpreter interpreter;
	switch(initInterpreter(&interpreter, &read))
	{
		case INIT_SIZE_ERR:
			printf("Binary size is %lu bytes, larger than maximum allowed size %lu bytes.\n",
				read.size, MAX_PROG_SIZE);
			return 1;
			break;
		default:
			break;
	}
	BinRead_free(&read);

	while(true)
	{
		enum ExecuteResult res = execNextInstruction(&interpreter);
		switch(res)
		{
			case EXEC_INVALID_INSTRUCTION:
				{
					printf("Invalid instruction found at address %x: %x",
						interpreter.pc, *(unsigned short*)(interpreter.memory + interpreter.pc));
					return 1;
				}
				break;
			case EXEC_STACK_OVERFLOW:
				{
					printf("Stack overflowed at address %x: %x",
						interpreter.pc, *(unsigned short*)(interpreter.memory + interpreter.pc));
					return 1;
				}
				break;
			default:
				break;
		}
	}

	Interpreter_free(&interpreter);
	return 0;
}

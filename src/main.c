#include "headers/file.h"
#include "headers/interpreter.h"
#include "headers/input.h"

#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"

#include "signal.h"
#include "time.h"
#include "unistd.h"
#include <bits/time.h>

void sigCleanup(int sig)
{
	disableRawInput();
	abort();
}

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

	struct BinRead readData;
	switch(getFileContents(filename, &readData))
	{
		case READ_ERR:
			printf("Error when reading file.\n");
			return 1;
			break;
		default:
			break;
	}

	struct Interpreter interpreter;
	switch(initInterpreter(&interpreter, &readData))
	{
		case INIT_SIZE_ERR:
			printf("Binary size is %lu bytes, larger than maximum allowed size %lu bytes.\n",
				readData.size, MAX_PROG_SIZE);
			return 1;
			break;
		default:
			break;
	}
	BinRead_free(&readData);

	// Set up terminal display and exit cleanup
	enableRawInput();
	signal(SIGTERM, sigCleanup);
	signal(SIGSEGV, sigCleanup);
	signal(SIGINT, sigCleanup);
	signal(SIGILL, sigCleanup);
	signal(SIGABRT, sigCleanup);
	signal(SIGFPE, sigCleanup);
	atexit(disableRawInput);

	bool running = true;
	bool waiting = false;
	unsigned char* keyReg;
	struct timespec prevTS;
	struct timespec curTS;
	clock_gettime(CLOCK_REALTIME, &prevTS);
	long timeSinceDecrement = 0;

	while(running)
	{
		clock_gettime(CLOCK_REALTIME, &curTS);
		long oldNS = prevTS.tv_nsec;
		long newNS = curTS.tv_nsec;
		if(newNS < oldNS) newNS += 1e9; // Time wrapped around, so add back the lost second
		timeSinceDecrement += newNS - oldNS;
		if(timeSinceDecrement > 1e9/60)
		{
			if(interpreter.delayTimer > 0) interpreter.delayTimer -= 1;
			if(interpreter.soundTimer > 0) interpreter.soundTimer -= 1;
			timeSinceDecrement -= 1e9/60;
		}
		prevTS = curTS;

		char inbfr[64];
		int n = read(0, &inbfr, 64);
		if(waiting)
		{
			if(n == 0) continue;
			int value = translateChar(inbfr[0]); // Has an issue if multiple buttons are registered at once and
			// the first is not a valid button. Don't care.
			if(value == -1) continue;
			waiting = false;
			*keyReg = value;
		}

		updateButtonStates(&interpreter.bs, inbfr, n);
		enum ExecuteResult res = execNextInstruction(&interpreter, &keyReg);
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
			case EXEC_RET_EMPTY_STACK:
				running = false;
				break;
			case EXEC_WAIT_INPUT:
				waiting = true;
				break;
			case EXEC_OK:
				break;
		}
		draw(&interpreter.scr);
	}

	Interpreter_free(&interpreter);
	return 0;
}

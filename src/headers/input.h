#pragma once


#include "interpreter.h"

void configureInput();
void resetInput();

enum ProgramControls {
	CMD_OK,
	CMD_QUIT,
};

enum ProgramControls updateInterpreterKeys(struct C8Interpreter* interpreter);

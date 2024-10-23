#pragma once


#include <stdint.h>

#include "img.h"

#define RAM_SIZE 4096
#define RAM_RESERVED 512

#define ADDR_PROGRAM_START RAM_RESERVED

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE SCREEN_WIDTH * SCREEN_HEIGHT / 8

struct C8Interpreter {
	uint8_t memory[RAM_SIZE];
	uint8_t screen[SCREEN_SIZE];

	uint8_t gen_regs[16];
	uint16_t reg_i;

	uint8_t timer_delay, timer_sound;

	uint16_t pc;
	uint8_t sp;
	uint16_t stack[16];
};

enum InitStatus {
	INIT_OK,

	INIT_PRGM_TOO_BIG,
	INIT_NULL_PTR,
};
const char* InitStatus_asStr(enum InitStatus status);

enum ExecStatus {
	EXEC_OK,

	EXEC_INVALID_ADDRESS,
	EXEC_INVALID_INSTRUCTION,
	EXEC_STACK_OVERFLOW,
};
const char* ExecStatus_asStr(enum ExecStatus status);

enum InitStatus C8Interpreter_init(struct C8Interpreter* restrict interpreter, const uint8_t* restrict program_data, size_t program_size);
enum ExecStatus C8Interpreter_step(struct C8Interpreter* interpreter);
void C8Interpreter_drawToImage(struct Image* image);

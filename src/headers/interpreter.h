#pragma once


#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "img.h"

#define RAM_SIZE 4096
#define RAM_RESERVED 512

#define ADDR_PROGRAM_START RAM_RESERVED

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 8)

#define PIXEL_ON_R 220
#define PIXEL_ON_G 200
#define PIXEL_ON_B 255
#define PIXEL_OFF_R 20
#define PIXEL_OFF_G 5
#define PIXEL_OFF_B 30

#define CLOCK_RATE_HZ 60
#define CLOCK_NS_PER_STEP (1000000000 / CLOCK_RATE_HZ)

struct KeyState {
	bool keys_held[16];
	bool keys_changed[16];
};

struct C8Interpreter {
	uint8_t memory[RAM_SIZE];
	uint8_t screen[SCREEN_SIZE];

	uint8_t gen_regs[16];
	uint16_t reg_i;

	uint8_t timer_delay, timer_sound;

	uint16_t pc;
	uint8_t sp;
	uint16_t stack[16];

	struct KeyState keys;

	uint64_t clock_tick_progress_ns;
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
	EXEC_RET_STACK_EMPTY,
	EXEC_STACK_OVERFLOW,
	EXEC_INVALID_KEY,
	EXEC_INVALID_SPRITE,

	EXEC_TERMINATE,
};
const char* ExecStatus_asStr(enum ExecStatus status);

enum InitStatus C8Interpreter_init(struct C8Interpreter* restrict interpreter, const uint8_t* restrict program_data, size_t program_size);
enum ExecStatus C8Interpreter_step(struct C8Interpreter* interpreter, long time_elapsed_ns);
void C8Interpreter_toggleKeyPressed(struct C8Interpreter* interpreter, size_t key_index);
void C8Interpreter_drawToImage(struct C8Interpreter* restrict interpreter, struct Image* restrict image);

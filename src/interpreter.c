#include <string.h>

#include "img.h"

#include "interpreter.h"

const char* InitStatus_asStr(enum InitStatus status) {
	switch (status) {
		case INIT_OK:
			return "INIT_OK - Interpreter initalized successfully";

		case INIT_PRGM_TOO_BIG:
			return "INIT_PRGM_TOO_BIG - Program too large for interpreter ram";
		case INIT_NULL_PTR:
			return "INIT_NULL_PTR - Attempted to initialize a null pointer";
	}
}
const char* ExecStatus_asStr(enum ExecStatus status) {
	switch (status) {
		case EXEC_OK:
			return "EXEC_OK - Instruction successfully executed";

		case EXEC_INVALID_ADDRESS:
			return "EXEC_INVALID_ADDRESS - Instruction used an invalid address";
		case EXEC_INVALID_INSTRUCTION:
			return "EXEC_INVALID_INSTRUCTION - Instruction not recognized";
		case EXEC_STACK_OVERFLOW:
			return "EXEC_STACK_OVERFLOW - Stack ran out of space";
	}
}

const static uint8_t sprites[5*16] = {
	 0xF0, 0x90, 0x90, 0x90, 0xF0,
	 0x20, 0x60, 0x20, 0x20, 0x70,
	 0xF0, 0x10, 0xF0, 0x80, 0xF0,
	 0xF0, 0x10, 0xF0, 0x10, 0xF0,
	 0x90, 0x90, 0xF0, 0x10, 0x10,
	 0xF0, 0x80, 0xF0, 0x10, 0xF0,
	 0xF0, 0x80, 0xF0, 0x90, 0xF0,
	 0xF0, 0x10, 0x20, 0x40, 0x40,
	 0xF0, 0x90, 0xF0, 0x90, 0xF0,
	 0xF0, 0x90, 0xF0, 0x10, 0xF0,
	 0xF0, 0x90, 0xF0, 0x90, 0x90,
	 0xE0, 0x90, 0xE0, 0x90, 0xE0,
	 0xF0, 0x80, 0x80, 0x80, 0xF0,
	 0xE0, 0x90, 0x90, 0x90, 0xE0,
	 0xF0, 0x80, 0xF0, 0x80, 0xF0,
	 0xF0, 0x80, 0xF0, 0x80, 0x80,
};

enum InitStatus C8Interpreter_init(struct C8Interpreter* restrict interpreter, const uint8_t* restrict program_data, size_t program_size) {
	if (interpreter == NULL)
		return INIT_NULL_PTR;
	if (program_size > RAM_SIZE - RAM_RESERVED)
		return INIT_PRGM_TOO_BIG;

	memset(&interpreter->memory, 0x0, RAM_SIZE);
	memset(&interpreter->screen, 0x0, SCREEN_SIZE);

	memset(&interpreter->gen_regs, 0x0, 16 * sizeof(uint8_t));
	interpreter->reg_i = 0;
	
	interpreter->timer_delay = 0;
	interpreter->timer_sound = 0;

	interpreter->pc = ADDR_PROGRAM_START;
	interpreter->sp = 0;
	memset(&interpreter->stack, 0x0, 16 * sizeof(uint16_t));

	memset(&interpreter->keys.keys_held, 0x0, 16 * sizeof(bool));
	memset(&interpreter->keys.keys_changed, 0x0, 16 * sizeof(bool));

	memcpy(&interpreter->memory[0] + interpreter->pc, program_data, program_size);

	return INIT_OK;
}
enum ExecStatus C8Interpreter_step(struct C8Interpreter* interpreter) {

}
void C8Interpreter_drawToImage(struct Image* image) {

}

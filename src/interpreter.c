#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "img.h"

#include "debug_logger.h"

#include "interpreter.h"

#define min(a, b) (a > b ? b : a)

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
		case EXEC_RET_STACK_EMPTY:
			return "EXEC_RET_STACK_EMPTY - RET executed when no addresses are on the stack";
		case EXEC_STACK_OVERFLOW:
			return "EXEC_STACK_OVERFLOW - Stack ran out of space";
		case EXEC_INVALID_KEY:
			return "EXEC_INVALID_KEY - Requested state of nonexistent key";
		case EXEC_INVALID_SPRITE:
			return "EXEC_INVALID_SPRITE - Requested address of nonexistent sprite";

		case EXEC_TERMINATE:
			return "EXEC_TERMINATE - Execution ended normally";
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

	memset(&interpreter->memory[0], 0x0, RAM_SIZE);
	memcpy(&interpreter->memory[0], sprites, 5*16);
	memset(&interpreter->screen[0], 0x0, SCREEN_SIZE);

	memset(&interpreter->gen_regs[0], 0x0, 16 * sizeof(uint8_t));
	interpreter->reg_i = 0;
	
	interpreter->timer_delay = 0;
	interpreter->timer_sound = 0;

	interpreter->pc = ADDR_PROGRAM_START;
	interpreter->sp = 0;
	memset(&interpreter->stack[0], 0x0, 16 * sizeof(uint16_t));

	memset(&interpreter->keys.keys_held[0], 0x0, 16 * sizeof(bool));
	memset(&interpreter->keys.keys_changed[0], 0x0, 16 * sizeof(bool));

	interpreter->clock_tick_progress_ns = 0;

	memcpy(&interpreter->memory[0] + interpreter->pc, program_data, program_size);

	return INIT_OK;
}

#define n(op) (op & 0x0FFF)
#define k(op) (op & 0x00FF)
#define w(op) ((op & 0xF000) >> 12)
#define x(op) ((op & 0x0F00) >> 8)
#define y(op) ((op & 0x00F0) >> 4)
#define z(op) (op & 0x000F)
enum ExecStatus C8Interpreter_step(struct C8Interpreter* interpreter, long time_elapsed_ns) {
	if (interpreter->pc < RAM_RESERVED || interpreter->pc >= RAM_SIZE) return EXEC_INVALID_ADDRESS;

	// Update timers
	interpreter->clock_tick_progress_ns += time_elapsed_ns;
	uint64_t ticks = interpreter->clock_tick_progress_ns / CLOCK_NS_PER_STEP;
	interpreter->clock_tick_progress_ns = interpreter->clock_tick_progress_ns % CLOCK_NS_PER_STEP;

	interpreter->timer_delay -= min(ticks, interpreter->timer_delay);
	interpreter->timer_sound -= min(ticks, interpreter->timer_sound);

	// Update inputs
	for (size_t i = 0; i < 16; i++) {
		interpreter->keys.keys_held[i] ^= interpreter->keys.keys_changed[i];
	}

	// Execute
	uint16_t op = interpreter->memory[interpreter->pc] << 8 | interpreter->memory[interpreter->pc + 1];
	log_dbg("executing op %04hX at pc %04hX", op, interpreter->pc);
	bool inc_pc = true;
	switch (w(op)) {
		case 0x0:
			if (op == 0x00E0) // CLS
				memset(&interpreter->screen[0], 0, SCREEN_SIZE);
			if (op == 0x00EE) { // RET
				if (interpreter->sp == 0) { return EXEC_RET_STACK_EMPTY; }
				interpreter->sp--;
				interpreter->pc = interpreter->stack[interpreter->sp];
			}
			// SYS (ignored)
			break;
		case 0x1: // JP addr
			inc_pc = false;
			interpreter->pc = n(op);
			break;
		case 0x2: // CALL addr
			if (interpreter->sp == 16) { return EXEC_STACK_OVERFLOW; }
			interpreter->stack[interpreter->sp] = interpreter->pc;
			interpreter->sp++;
			interpreter->pc = n(op);
			inc_pc = false;
			break;
		case 0x3: // SE Vx, byte
			if (interpreter->gen_regs[x(op)] == k(op))
				interpreter->pc += 2;
			break;
		case 0x4: // SNE Vx, byte
			if (interpreter->gen_regs[x(op)] != k(op))
				interpreter->pc += 2;
			break;
		case 0x5: // SE Vx, Vy
			if (interpreter->gen_regs[x(op)] == interpreter->gen_regs[y(op)])
				interpreter->pc += 2;
			break;
		case 0x6: // LD Vx, byte
			interpreter->gen_regs[x(op)] = k(op);
			break;
		case 0x7: // ADD Vx, byte
			interpreter->gen_regs[x(op)] += k(op);
			break;
		case 0x8: // Misc reg ops
			switch (z(op)) {
				case 0x0: // LD Vx, Vy
					interpreter->gen_regs[x(op)] = interpreter->gen_regs[y(op)];
					break;
				case 0x1: // OR Vx, Vy
					interpreter->gen_regs[x(op)] |= interpreter->gen_regs[y(op)];
					break;
				case 0x2: // AND Vx, Vy
					interpreter->gen_regs[x(op)] &= interpreter->gen_regs[y(op)];
					break;
				case 0x3: // XOR Vx, Vy
					interpreter->gen_regs[x(op)] ^= interpreter->gen_regs[y(op)];
					break;
				case 0x4: // ADD Vx, Vy
					interpreter->gen_regs[x(op)] += interpreter->gen_regs[y(op)];
					if (interpreter->gen_regs[x(op)] < interpreter->gen_regs[y(op)])
						interpreter->gen_regs[15] = 1;
					break;
				case 0x5: // SUB Vx, Vy
					if (interpreter->gen_regs[x(op)] > interpreter->gen_regs[y(op)])
						interpreter->gen_regs[15] = 1;
					interpreter->gen_regs[x(op)] -= interpreter->gen_regs[y(op)];
					break;
				case 0x6: // SHR Vx
					interpreter->gen_regs[15] = x(op) & 0x1;
					interpreter->gen_regs[x(op)] >>= 1;
					break;
				case 0x7: // SUBN Vx, Vy
					if (interpreter->gen_regs[y(op)] > interpreter->gen_regs[x(op)])
						interpreter->gen_regs[15] = 1;
					interpreter->gen_regs[x(op)] = interpreter->gen_regs[y(op)] - interpreter->gen_regs[x(op)];
					break;
				case 0xE: // SHL Vx
					interpreter->gen_regs[15] = x(op) & 0x80;
					interpreter->gen_regs[x(op)] <<= 1;
					break;
				default:
					return EXEC_INVALID_INSTRUCTION;
			}
			break;
		case 0x9: // SNE Vx, Vy
			if (interpreter->gen_regs[x(op)] != interpreter->gen_regs[y(op)])
				interpreter->pc += 2;
			break;
		case 0xA: // LD I, addr
			interpreter->reg_i = n(op);
			break;
		case 0xB: // JP V0, addr
			inc_pc = false;
			interpreter->pc = interpreter->gen_regs[0] + n(op);
			break;
		case 0xC: // RND Vx, byte
			interpreter->gen_regs[x(op)] = (rand() & 0xFF) & k(op);
			break;
		case 0xD: { // DRW Vx, Vy, nibble
			uint8_t sprite_size = z(op);
			if (interpreter->reg_i + sprite_size >= RAM_SIZE)
				return EXEC_INVALID_ADDRESS;

			uint8_t x_pos = interpreter->gen_regs[x(op)];
			uint8_t y_pos = interpreter->gen_regs[y(op)];
			uint8_t collision = 0;

			for (size_t byte_index = 0; byte_index < sprite_size; byte_index++) {
				uint8_t sprite_byte = interpreter->memory[interpreter->reg_i + byte_index];
				for (size_t bit_index = 0; bit_index < 8; bit_index++) {
					uint8_t sprite_bit = (sprite_byte >> (7-bit_index)) & 0x1;
					size_t x = (x_pos + bit_index) % SCREEN_WIDTH;
					size_t y = (y_pos + byte_index) % SCREEN_HEIGHT;

					uint8_t target_bit = x % 8;
					uint8_t mask = sprite_bit << (7-target_bit);

					collision |= interpreter->screen[y * (SCREEN_WIDTH / 8) + x / 8] & mask;
					interpreter->screen[y * (SCREEN_WIDTH / 8) + x / 8] ^= mask;
				}
			}

			interpreter->gen_regs[15] = collision > 0;
			break;
		}
		case 0xE: // Skip on key state
			switch (k(op)) {
				case 0x9E: // SKP Vx
					if (interpreter->gen_regs[x(op)] > 15)
						return EXEC_INVALID_KEY;
					if (interpreter->keys.keys_held[interpreter->gen_regs[x(op)]])
						interpreter->pc += 2;
					break;
				case 0xA1: // SKNP Vx
					if (interpreter->gen_regs[x(op)] > 15)
						return EXEC_INVALID_KEY;
					if (!interpreter->keys.keys_held[interpreter->gen_regs[x(op)]])
						interpreter->pc += 2;
					break;
				default:
					return EXEC_INVALID_INSTRUCTION;
			}
			break;
		case 0xF: // Misc
			switch (k(op)) {
				case 0x07: // LD Vx, DT
					interpreter->gen_regs[x(op)] = interpreter->timer_delay;
					break;
				case 0x0A: { // LD Vx, K
					bool key_found = false;
					for (size_t i = 0; i < 16; i++) {
						if (interpreter->keys.keys_changed[i]) {
							interpreter->gen_regs[x(op)] = i;
							key_found = true;
							break;
						}
					}
					if (!key_found) // Stall on this instruction
						inc_pc = false;
					break;
				}
				case 0x15: // LD DT, Vx
					interpreter->timer_delay = interpreter->gen_regs[x(op)];
					break;
				case 0x18: // LD ST, Vx
					interpreter->timer_sound = interpreter->gen_regs[x(op)];
					break;
				case 0x1E: // ADD I, Vx
					interpreter->reg_i += interpreter->gen_regs[x(op)];
					break;
				case 0x29: // LD F, Vx
					if (interpreter->gen_regs[x(op)] > 15)
						return EXEC_INVALID_SPRITE;
					interpreter->reg_i = interpreter->gen_regs[x(op)] * 5;
					break;
				case 0x33: // LD B, Vx
					if (interpreter->reg_i + 2 >= RAM_SIZE)
						return EXEC_INVALID_ADDRESS;
					interpreter->memory[interpreter->reg_i] = interpreter->gen_regs[x(op)] / 100;
					interpreter->memory[interpreter->reg_i + 1] = interpreter->gen_regs[x(op)] % 100 / 10;
					interpreter->memory[interpreter->reg_i + 2] = interpreter->gen_regs[x(op)] % 10;
					break;
				case 0x55: // LD [I], Vx
					if (interpreter->reg_i + x(op) >= RAM_SIZE)
						return EXEC_INVALID_ADDRESS;
					for (size_t i = 0; i <= x(op); i++) {
						interpreter->memory[interpreter->reg_i + i] = interpreter->gen_regs[i];
					}
					break;
				case 0x65: // LD Vx, [I]
					if (interpreter->reg_i + x(op) >= RAM_SIZE)
						return EXEC_INVALID_ADDRESS;
					for (size_t i = 0; i <= x(op); i++) {
						interpreter->gen_regs[i] = interpreter->memory[interpreter->reg_i + i];
					}
					break;
				default:
					return EXEC_INVALID_INSTRUCTION;
			}
			break;
	}

	if (inc_pc)
		interpreter->pc += 2;

	// Clear changed keys
	memset(&interpreter->keys.keys_changed[0], false, 16);

	return EXEC_OK;
}
void C8Interpreter_toggleKeyPressed(struct C8Interpreter* interpreter, size_t key_index) {
	interpreter->keys.keys_changed[key_index] ^= 0b1;
}
void C8Interpreter_drawToImage(struct C8Interpreter* restrict interpreter, struct Image* restrict image) {
	for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
		for (size_t x = 0; x < SCREEN_WIDTH; x++) {
			size_t cur_byte = y * (SCREEN_WIDTH / 8) + x / 8;
			size_t cur_bit = x % 8;
			uint8_t mask = 0b1 << (7 - cur_bit);
			
			struct Pixel* cur_pixel = Image_getPixel(image, x, y);
			
			if (interpreter->screen[cur_byte] & mask) {
				cur_pixel->col.r = PIXEL_ON_R;
				cur_pixel->col.g = PIXEL_ON_G;
				cur_pixel->col.b = PIXEL_ON_B;
			} else {
				cur_pixel->col.r = PIXEL_OFF_R;
				cur_pixel->col.g = PIXEL_OFF_G;
				cur_pixel->col.b = PIXEL_OFF_B;
			}
		}
	}
}

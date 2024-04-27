#include "headers/interpreter.h"

#include "limits.h"
#include "stdlib.h"
#include "string.h"

void Interpreter_free(struct Interpreter *toFree)
{
	free(toFree->memory);
}

enum InitializeResult initInterpreter(struct Interpreter* interpreter, struct BinRead* readData)
{
	if(readData->size > MAX_PROG_SIZE)
		return INIT_SIZE_ERR;

	interpreter->memory = calloc(MEM_SIZE, 1);
	memset(interpreter->genRegs, 0, 16);
	interpreter->addrReg = 0;
	interpreter->delayTimer = 0;
	interpreter->soundTimer = 0;
	interpreter->pc = ENTRY_POINT;
	interpreter->sp = STACK_START;

	memcpy(interpreter->memory + ENTRY_POINT, readData->data, readData->size);

	return INIT_OK;
}

enum ExecuteResult execNextInstruction(struct Interpreter* interpreter)
{
	unsigned char* instructionPtr = interpreter->memory + interpreter->pc;
	unsigned short curInstruction;
	curInstruction = (*instructionPtr) << 8;
	curInstruction |= *(instructionPtr+1);
	interpreter->pc += 2;

	unsigned char hh = (curInstruction & 0xF000) >> 12;
	switch(hh)
	{
		case 0x00: // Misc
			switch(curInstruction)
			{
				case 0x00E0: // CLS
					break;
				case 0x00EE: // RET
					{
						if(interpreter->sp == STACK_START) return EXEC_RET_EMPTY_STACK;
						unsigned short retLocation = 
							*(interpreter->memory + interpreter->sp) << 8;
						retLocation |= *(interpreter->memory + interpreter->sp + 1);
						interpreter->sp -= 2;
						interpreter->pc = retLocation;
					}
					break;
				default:
					break;
			}
			break;
		case 0x01: // JP addr
			{
				unsigned short jpLocation = curInstruction & 0x0FFF;
				interpreter->pc = jpLocation;
			}
			break;
		case 0x02: // CALL addr
			{
				unsigned short jpLocation = curInstruction & 0x0FFF;
				interpreter->sp += 2;
				if(interpreter->sp >= STACK_START + STACK_SIZE)
					return EXEC_STACK_OVERFLOW;

				unsigned char* writeLoc = interpreter->memory + interpreter->sp;
				*writeLoc = (unsigned char)(interpreter->pc >> 8);
				*(writeLoc+1) = (unsigned char)(interpreter->pc & 0x00FF);
				interpreter->pc = jpLocation;
			}
			break;
		case 0x03: // SE reg, imm
			{
				unsigned char regIndex = (curInstruction & 0x0F00) >> 8;
				unsigned char cmpValue = curInstruction & 0x00FF;
				if(interpreter->genRegs[regIndex] == cmpValue)
					interpreter->pc += 2;
			}
			break;
		case 0x04: // SNE reg, imm
			{
				unsigned char regIndex = (curInstruction & 0x0F00) >> 8;
				unsigned char cmpValue = curInstruction & 0x00FF;
				if(interpreter->genRegs[regIndex] != cmpValue)
					interpreter->pc += 2;
			}
			break;
		case 0x05: // SE reg, reg
			{
				unsigned char regIndex1 = (curInstruction & 0x0F00) >> 8;
				unsigned char regIndex2 = (curInstruction & 0x00F0) >> 4;
				if(interpreter->genRegs[regIndex1] == interpreter->genRegs[regIndex2])
					interpreter->pc += 2;
			}
			break;
		case 0x06: // LD reg, imm
			{
				unsigned char regIndex = (curInstruction & 0x0F00) >> 8;
				unsigned char imm = curInstruction & 0x00FF;
				interpreter->genRegs[regIndex] = imm;
			}
			break;
		case 0x07: // ADD reg, imm
			{
				unsigned char regIndex = (curInstruction & 0x0F00) >> 8;
				unsigned char imm = curInstruction & 0x00FF;
				interpreter->genRegs[regIndex] += imm;
			}
			break;
		case 0x08: // Operations with two regs
			{
				unsigned char regIndex1 = (curInstruction & 0x0F00) >> 8;
				unsigned char regIndex2 = (curInstruction & 0x00F0) >> 4;
				unsigned char discrim = curInstruction & 0x000F;
				switch(discrim)
				{
					case 0x00: // LD reg, reg
						interpreter->genRegs[regIndex1] = interpreter->genRegs[regIndex2];
						break;
					case 0x01: // OR reg, reg
						interpreter->genRegs[regIndex1] |= interpreter->genRegs[regIndex2];
						break;
					case 0x02: // AND reg, reg
						interpreter->genRegs[regIndex1] &= interpreter->genRegs[regIndex2];
						break;
					case 0x03: // XOR reg, reg
						interpreter->genRegs[regIndex1] ^= interpreter->genRegs[regIndex2];
						break;
					case 0x04: // ADD reg, reg
						interpreter->genRegs[0xF] = 
							interpreter->genRegs[regIndex1] > UCHAR_MAX - interpreter->genRegs[regIndex2];
						interpreter->genRegs[regIndex1] += interpreter->genRegs[regIndex2];
						break;
					case 0x05: // SUB reg, reg
						interpreter->genRegs[0xF] =
							interpreter->genRegs[regIndex1] > interpreter->genRegs[regIndex2];
						interpreter->genRegs[regIndex1] -= interpreter->genRegs[regIndex2];
						break;
					case 0x06: // SHR reg
						interpreter->genRegs[0xF] = interpreter->genRegs[regIndex1] & 0b00000001;
						interpreter->genRegs[regIndex1] >>= 1;
						break;
					case 0x07: // SUBN reg, reg
						interpreter->genRegs[0xF] =
							interpreter->genRegs[regIndex2] > interpreter->genRegs[regIndex1];
						interpreter->genRegs[regIndex1] =
							interpreter->genRegs[regIndex2] - interpreter->genRegs[regIndex1];
						break;
					case 0x0E: // SHL reg
						interpreter->genRegs[0xF] = (interpreter->genRegs[regIndex1] & 0b10000000) >> 7;
						interpreter->genRegs[regIndex1] <<= 1;
						break;
					default:
						return EXEC_INVALID_INSTRUCTION;
						break;
				}
			}
			break;
		case 0x09: // SNE reg, reg
			{
				unsigned char regIndex1 = (curInstruction & 0x0F00) >> 8;
				unsigned char regIndex2 = (curInstruction & 0x00F0) >> 4;
				if(interpreter->genRegs[regIndex1] != interpreter->genRegs[regIndex2])
					interpreter->pc += 2;
			}
			break;
		case 0x0A: // LD I, addr
			{
				unsigned short addr = (curInstruction & 0x0FFF);
				interpreter->addrReg = addr;
			}
			break;
		case 0x0B: // JP V0 + addr
			{
				unsigned short addr = (curInstruction & 0x0FFF);
				addr += interpreter->genRegs[0];
				interpreter->pc = addr;
			}
			break;
		case 0x0C: // RND reg, imm
			{
				unsigned char regIndex = (curInstruction & 0x0F00) >> 8;
				unsigned char mask = curInstruction & 0x00FF;
				unsigned char randValue = rand();
				interpreter->genRegs[regIndex] = randValue & mask;
			}
			break;
		case 0x0D: // DRW reg, reg, imm(nibble)
			break;
		case 0x0E: // Skips on key state
			break;
		case 0x0F: // Misc
			break;
	}
	return EXEC_OK;
}

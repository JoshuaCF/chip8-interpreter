#pragma once
#include "sys/types.h"

#define MEM_SIZE 0x1000

typedef enum ReadResult { READ_OK, READ_ERR } ReadResult;
typedef struct BinRead
{
	unsigned short* memory;
	off_t size;
} BinRead;

ReadResult getFileContents(char* filename, BinRead* readStruct);

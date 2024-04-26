#pragma once
#include "sys/types.h"


enum ReadResult { READ_OK, READ_ERR };
struct BinRead
{
	unsigned short* data;
	off_t size;
};
void BinRead_free(struct BinRead* toFree);

enum ReadResult getFileContents(char* filename, struct BinRead* readStruct);

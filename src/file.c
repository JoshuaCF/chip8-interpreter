#include "headers/file.h"
#include "stdlib.h"
#include "stdio.h"

#include "sys/stat.h"

ReadResult getFileContents(char* filename, BinRead* readStruct)
{
	FILE* fp = fopen(filename, "r");
	if(fp == (void*)0)
	{
		return READ_ERR;
	}

	int fd = fileno(fp);
	struct stat fileStat;
	if(fstat(fd, &fileStat) != 0)
	{
		return READ_ERR;
	}
	off_t fileSize = fileStat.st_size;

	unsigned short* memory = calloc(MEM_SIZE, 1);
	fread(memory, 1, MEM_SIZE, fp);

	if(!feof(fp))
	{
		return READ_ERR;
	}

	readStruct->memory = memory;
	readStruct->size = fileSize;
	return READ_OK;
}

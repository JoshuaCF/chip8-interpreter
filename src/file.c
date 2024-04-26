#include "headers/file.h"

#include "stdlib.h"
#include "stdio.h"

#include "sys/stat.h"

void BinRead_free(struct BinRead* toFree)
{
	free(toFree->data);
}

enum ReadResult getFileContents(char* filename, struct BinRead* readStruct)
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

	unsigned short* data = calloc(fileSize, 1);
	unsigned long readChars = fread(data, 1, fileSize, fp);

	// Using this instead of feof since the EOF flag is not actually
	// set for this read operation, due to the fact that I do not
	// attempt to read past the end of the file, I read exactly what
	// the file contains.
	if(readChars != fileSize)
	{
		return READ_ERR;
	}

	readStruct->data = data;
	readStruct->size = fileSize;
	return READ_OK;
}

#include "headers/file.h"

#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"

enum argStatus { OK, TOO_FEW, TOO_MANY };
enum argStatus verifyArgs(int argc)
{
	if(argc > 2) { return TOO_MANY; }
	if(argc < 2) { return TOO_FEW; }
	return OK;
}

int main(int argc, char* argv[])
{
	enum argStatus status = verifyArgs(argc);
	switch(status)
	{
		case TOO_FEW:
			printf("Too few arguments passed to program.\n");
			return 1;
			break;
		case TOO_MANY:
			printf("Too many arguments passed to the program.\n");
			return 1;
			break;
		case OK:
			break;
	}

	char* filename = argv[1];
	printf("Running file '%s'...\n", filename);

	BinRead read;
	switch(getFileContents(filename, &read))
	{
		case READ_ERR:
			printf("Error when reading file.");
			break;
		default:
			break;
	}

	unsigned char* dataCasted = (unsigned char*)(read.memory);
	for(int i = 0; i < MEM_SIZE; i++)
	{
		printf("%hhx", dataCasted[i]);
		if(i == 128)
		{
			printf("\n");
		}
	}

	return 0;
}

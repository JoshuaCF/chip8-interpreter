#include <stdio.h>
#include <stdlib.h>

#include "img.h"
#include "term_ctrl.h"

#include "interpreter.h"

int main(int argc, char* argv[]) {
	// Check arguments
	if (argc != 2) {
		fprintf(stderr, "Invalid arguments. Valid usage is as follows:\n");
		fprintf(stderr, "chip8 BINARY\n\n");
		fprintf(stderr, "Where:\n");
		fprintf(stderr, "\tBINARY is a file containing chip8 bytes, usually ending in .ch8\n");
		return 1;
	}

	// Open file in argv[1]
	FILE* file = fopen(argv[1], "rb");
	if (file == NULL) {
		fprintf(stderr, "Error opening file: %s\n", argv[1]);
		return 1;
	}

	// Determine file size through seeking
	if (fseek(file, 0, SEEK_END) != 0) {
		fprintf(stderr, "Error determining file size: failed to seek to end\n");
		return 1;
	}
	long file_len = ftell(file);
	if (file_len < 0) {
		fprintf(stderr, "Error determining file size: failed to tell current position\n");
		return 1;
	}
	if (fseek(file, 0, SEEK_SET) != 0) {
		fprintf(stderr, "Error seeking to start of file\n");
		return 1;
	}

	// Read file contents
	uint8_t* read_buf = malloc(file_len + 1);
	read_buf[file_len] = '\x00';

	if (fread(read_buf, 1, file_len, file) < file_len) {
		fprintf(stderr, "Error reading file contents\n");
		return 1;
	}

	// Initialize interpreter
	struct C8Interpreter interpreter;
	enum InitStatus init_status = C8Interpreter_init(&interpreter, read_buf, file_len);
	if (init_status != INIT_OK) {
		fprintf(stderr, "Error initializing interpreter: %s", InitStatus_asStr(init_status));
		return 1;
	}

	// Free file resources
	fclose(file);
	free(read_buf);

	// Setup display
	struct TermCtrlQueue ctrl_queue = TermCtrlQueue_new(stdout);

	queueTermCtrlDisplayEnterAltBuffer(&ctrl_queue);
	queueTermCtrlCursorMoveToOrigin(&ctrl_queue);
	queueTermCtrlDisplayEraseAll(&ctrl_queue);

	TermCtrlQueue_free(&ctrl_queue);

	return 0;
}

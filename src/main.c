#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "img.h"
#include "input.h"
#include "term_ctrl.h"

#include "debug_logger.h"
#include "interpreter.h"

int main(int argc, char* argv[]) {
	enable_logging();

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
	uint8_t* read_buf = malloc(file_len);

	if (fread(read_buf, 1, file_len, file) < file_len) {
		fprintf(stderr, "Error reading file contents\n");
		return 1;
	}

	// Initialize interpreter
	struct C8Interpreter interpreter;
	enum InitStatus init_status = C8Interpreter_init(&interpreter, read_buf, file_len);
	if (init_status != INIT_OK) {
		fprintf(stderr, "Error initializing interpreter: %s\n", InitStatus_asStr(init_status));
		return 1;
	}

	// Free file resources
	fclose(file);
	free(read_buf);

	// Prepare image buffer
	struct Image image = Image_new(SCREEN_WIDTH, SCREEN_HEIGHT);

	// Set up input
	configureInput();

	// Configure terminal
	struct TermCtrlQueue ctrl_queue = TermCtrlQueue_new(stdout);

	queueTermCtrlDisplayEnterAltBuffer(&ctrl_queue);
	queueTermCtrlCursorMoveToOrigin(&ctrl_queue);
	queueTermCtrlDisplayEraseAll(&ctrl_queue);
	queueTermCtrlCursorSetInvisible(&ctrl_queue);
	TermCtrlQueue_exec(&ctrl_queue);
	fflush(stdout);

	// Set up time tracking
	// POSIX only atm
	struct timespec prev_time, cur_time;

	clock_gettime(CLOCK_REALTIME, &prev_time);

	// Seed RNG
	srand(time(NULL));

	// Control loop
	bool running = true;
	enum ExecStatus status = EXEC_OK;
	while (running && status == EXEC_OK) {
		// Compute time elapsed
		clock_gettime(CLOCK_REALTIME, &cur_time);

		long old_ns = prev_time.tv_nsec;
		long cur_ns = cur_time.tv_nsec;
		if (old_ns > cur_ns) old_ns -= 1e9;
		long delta_ns = cur_ns - old_ns;

		prev_time = cur_time;

		// Update input
		switch (updateInterpreterKeys(&interpreter)) {
			case CMD_QUIT:
				running = false;
				break;
			case CMD_OK:
				break;
		}

		// Prepare screen
		queueTermCtrlDisplayEraseFromCursor(&ctrl_queue);
		queueTermCtrlCursorMoveToOrigin(&ctrl_queue);
		TermCtrlQueue_exec(&ctrl_queue);
		fflush(stdout);

		// Tick
		status = C8Interpreter_step(&interpreter, delta_ns);
		C8Interpreter_drawToImage(&interpreter, &image);
		Image_draw(&image, stdout);
	}

	// Fix terminal
	queueTermCtrlDisplayLeaveAltBuffer(&ctrl_queue);
	queueTermCtrlCursorSetVisible(&ctrl_queue);
	TermCtrlQueue_exec(&ctrl_queue);

	// Fix input
	resetInput();

	printf("%s\n", ExecStatus_asStr(status));

	TermCtrlQueue_free(&ctrl_queue);
	Image_free(&image);

	disable_logging();
	return 0;
}

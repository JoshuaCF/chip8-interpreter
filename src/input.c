#include <stdio.h>

#include <termios.h>

#include "interpreter.h"

#include "input.h"

static struct termios original_settings;

void configureInput() {
	int fd = fileno(stdin);
	fprintf(stderr, "fileno: %d\n", fd);
	tcgetattr(fd, &original_settings);

	struct termios new_settings = original_settings;
	new_settings.c_lflag &= ~ECHO;
	new_settings.c_lflag &= ~ICANON;
	new_settings.c_cc[VMIN] = 0;
	new_settings.c_cc[VTIME] = 0;
	tcsetattr(fd, TCSANOW, &new_settings);

	setvbuf(stdin, NULL, _IONBF, BUFSIZ);
}
void resetInput() {
	setvbuf(stdin, NULL, _IOLBF, BUFSIZ);

	int fd = fileno(stdin);
	tcsetattr(fd, TCSANOW, &original_settings);
}

enum ProgramControls updateInterpreterKeys(struct C8Interpreter* interpreter) {
	enum ProgramControls return_val = CMD_OK;
	char buf[32];
	size_t objs_read = fread(&buf[0], sizeof(char), 32, stdin);
	for (size_t i = 0; i < objs_read; i++) {
		char cur_char = buf[i];
		if (cur_char == 'q') {
			return_val = CMD_QUIT;
		} else if (cur_char >= 'a' && cur_char <= 'p') {
			uint8_t button = cur_char - 'a';
			interpreter->keys.keys_changed[button] ^= 0b1;
		}
	}

	return return_val;
}

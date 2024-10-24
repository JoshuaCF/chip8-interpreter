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
		}
		switch (cur_char) {
			case 'm':
				interpreter->keys.keys_changed[0] ^= 0b1;
				break;
			case '6':
				interpreter->keys.keys_changed[1] ^= 0b1;
				break;
			case '7':
				interpreter->keys.keys_changed[2] ^= 0b1;
				break;
			case '8':
				interpreter->keys.keys_changed[3] ^= 0b1;
				break;
			case 'y':
				interpreter->keys.keys_changed[4] ^= 0b1;
				break;
			case 'u':
				interpreter->keys.keys_changed[5] ^= 0b1;
				break;
			case 'i':
				interpreter->keys.keys_changed[6] ^= 0b1;
				break;
			case 'h':
				interpreter->keys.keys_changed[7] ^= 0b1;
				break;
			case 'j':
				interpreter->keys.keys_changed[8] ^= 0b1;
				break;
			case 'k':
				interpreter->keys.keys_changed[9] ^= 0b1;
				break;
			case 'n':
				interpreter->keys.keys_changed[10] ^= 0b1;
				break;
			case ',':
				interpreter->keys.keys_changed[11] ^= 0b1;
				break;
			case '9':
				interpreter->keys.keys_changed[12] ^= 0b1;
				break;
			case 'o':
				interpreter->keys.keys_changed[13] ^= 0b1;
				break;
			case 'l':
				interpreter->keys.keys_changed[14] ^= 0b1;
				break;
			case '.':
				interpreter->keys.keys_changed[15] ^= 0b1;
				break;
		}
	}

	return return_val;
}

#include "headers/input.h"

#include "stdio.h"

#include "termios.h"

struct termios originalSettings;

void enableRawInput()
{
	// Switch to alternate screen buffer
	printf("\e[?1049h");
	// Turn off cursor
	printf("\eESC[?25l");

	int fd = fileno(stdin);
	tcgetattr(fd, &originalSettings);

	struct termios newSettings = originalSettings;
	newSettings.c_lflag &= ~(ECHO | ICANON);
	newSettings.c_cc[VMIN] = 0;
	newSettings.c_cc[VTIME] = 0;
	tcsetattr(fd, TCSANOW, &newSettings);
}

void disableRawInput()
{
	int fd = fileno(stdin);
	tcsetattr(fd, TCSANOW, &originalSettings);
	// Switch to main buffer
	printf("\e[?1049l");
	// Enable cursor
	printf("\eESC[?25h");
}

int translateChar(char c)
{
		switch(c)
		{
			case '6':
				return 1;
			case '7':
				return 2;
			case '8':
				return 3;
			case '9':
				return 12;
			case 'y':
				return 4;
			case 'u':
				return 5;
			case 'i':
				return 6;
			case 'o':
				return 13;
			case 'h':
				return 7;
			case 'j':
				return 8;
			case 'k':
				return 9;
			case 'l':
				return 14;
			case 'n':
				return 10;
			case 'm':
				return 0;
			case ',':
				return 11;
			case '.':
				return 15;
			default:
				return -1;
	}
}

void resetButtonStates(struct ButtonStates* bs)
{
	for(int i = 0; i < 16; i++)
		bs->state[i] = 0;
}

// 6789
// yuio
// hjkl
// nm,.
// ====
// 123C
// 456D
// 789E
// A0BF
void updateButtonStates(struct ButtonStates* bs, char* input, int n)
{
	for(int i = 0; i < n; i++)
	{
		int val = translateChar(input[i]);
		if(val == -1) continue;
		bs->state[val] ^= 0x1;
	}
}

void printButtonStates(struct ButtonStates *bs)
{
	// Move cursor to top left
	printf("\e[H");
	// Print button states
	for(int i = 0; i < 16; i++)
	{
		printf("%d: %d", i, bs->state[i]);
		// Clear rest of line and move down
		printf("\e[0K\n");
	}
}

#include "headers/input.h"

#include "stdio.h"

#include "termios.h"

struct termios originalSettings;

void enableRawInput()
{
	// Switch to alternate screen buffer
	printf("\e[?1049h");

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
		switch(input[i])
		{
			case '6':
				bs->state[1] ^= 0x1;
				break;
			case '7':
				bs->state[2] ^= 0x1;
				break;
			case '8':
				bs->state[3] ^= 0x1;
				break;
			case '9':
				bs->state[12] ^= 0x1;
				break;
			case 'y':
				bs->state[4] ^= 0x1;
				break;
			case 'u':
				bs->state[5] ^= 0x1;
				break;
			case 'i':
				bs->state[6] ^= 0x1;
				break;
			case 'o':
				bs->state[13] ^= 0x1;
				break;
			case 'h':
				bs->state[7] ^= 0x1;
				break;
			case 'j':
				bs->state[8] ^= 0x1;
				break;
			case 'k':
				bs->state[9] ^= 0x1;
				break;
			case 'l':
				bs->state[14] ^= 0x1;
				break;
			case 'n':
				bs->state[10] ^= 0x1;
				break;
			case 'm':
				bs->state[0] ^= 0x1;
				break;
			case ',':
				bs->state[11] ^= 0x1;
				break;
			case '.':
				bs->state[15] ^= 0x1;
				break;
			default:
				break;
		}
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

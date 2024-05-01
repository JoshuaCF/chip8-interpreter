#include "headers/output.h"

#include "stdbool.h"
#include "stdio.h"
#include "string.h"

void clearScreen(struct Screen* s)
{
	memset(s->cellState, false, WIDTH * HEIGHT);
}

bool writeSprite(struct Screen* s, int x, int y, unsigned char* sprite, int bytes)
{
	bool collision = false;

	for(int yOffset = 0; yOffset < bytes; yOffset++)
	{
		for(int xOffset = 0; xOffset < 8; xOffset++)
		{
			bool curBit = (sprite[yOffset] & (0x1 << (7-xOffset))) > 0;
			if(!curBit) continue;

			collision |= s->cellState[(xOffset + x) % WIDTH][(yOffset + y) % HEIGHT];
			s->cellState[(xOffset + x) % WIDTH][(yOffset + y) % HEIGHT] ^= 1;
		}
	}

	return collision;
}

void draw(struct Screen* s)
{
	// Move cursor to top left
	printf("\e[H");
	for(int y = 0; y < HEIGHT; y++)
	{
		// Write row
		for(int x = 0; x < WIDTH; x++)
		{
			if(s->cellState[x][y])
				printf("\e[47m"); // Background white
			else
				printf("\e[40m"); // Background black
			printf("  ");
		}
		// Clear after row
		printf("\e[0m\e[0k\n"); // Reset, erase to end of line, move to next line
	}
	// Clear rest of screen
	printf("\e[0j");
}

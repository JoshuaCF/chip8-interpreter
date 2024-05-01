#pragma once

#include "stdbool.h"
#include "stdio.h"

#define HEIGHT 32
#define WIDTH 64

struct Screen
{
	bool cellState[WIDTH][HEIGHT];
};

void clearScreen(struct Screen* s);
bool writeSprite(struct Screen* s, int x, int y, unsigned char* sprite, int bytes);
void draw(struct Screen* s);

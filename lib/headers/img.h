#pragma once

#include <stddef.h>

// Dependencies
#include "term_ctrl.h"

struct Color {
	unsigned char r, g, b;
};

struct Pixel {
	struct Color col;
	unsigned char transparency;
};

struct Image {
	size_t width, height;
	struct Pixel* data;
};

struct Image Image_new(size_t width, size_t height);
struct Pixel* Image_getPixel(struct Image* img, size_t x, size_t y);
void Image_draw(struct Image* img, Terminal term);
void Image_free(struct Image* img);

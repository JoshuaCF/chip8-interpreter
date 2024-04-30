#pragma once

#include <stdbool.h>

struct ButtonStates
{
	bool state[16]; // Indicates whether or not a button is currently active (held)
};

void enableRawInput();
void disableRawInput();
void resetButtonStates(struct ButtonStates* bs);
void updateButtonStates(struct ButtonStates* bs, char* input, int n);

// Used for debugging
void printButtonStates(struct ButtonStates* bs);

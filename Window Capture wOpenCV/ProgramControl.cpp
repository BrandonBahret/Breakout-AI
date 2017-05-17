#include "ProgramControl.h"

// Program Control Code

bool isPlusOrMinus(int value, int expected, int plusOrMinus) {
	return ((expected - plusOrMinus) <= value) && (value <= (expected + plusOrMinus));
}

bool isInBounds(double value, double minRange, double maxRange) {
	return (value >= minRange) && (value <= maxRange);
}

bool isCursorShown() {
	CURSORINFO ci;
	ci.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&ci);
	return ci.flags == CURSOR_SHOWING;
}
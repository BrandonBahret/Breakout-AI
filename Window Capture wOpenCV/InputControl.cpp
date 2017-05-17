#include "InputControl.h"
#include <stdio.h>

// Input Generation Code

void movePaddleTo(int pos, int dest) {
	if (pos != dest) {
		MOUSEINPUT mInput = {};
		INPUT input = {};

		int dist = abs(dest - pos);
		long dx = 0.15 * dist + 1;

		mInput.dx = dest > pos ? dx : -dx;
		mInput.dwFlags = MOUSEEVENTF_MOVE;
		input.type = INPUT_MOUSE;
		input.mi = mInput;

		SendInput(1, &input, sizeof(input));
	}
}

void newBall() {
	BYTE space = MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC);
	keybd_event(0, space, 0, 0);
	Sleep(5);
	keybd_event(0, space, KEYEVENTF_KEYUP, 0);
}
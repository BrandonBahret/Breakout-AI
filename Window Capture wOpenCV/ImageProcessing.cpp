#include "ImageProcessing.h"

#define threshval 88

// Parsing Code

#define isDistantBy(x,y,value) ((x > y + value) || (x < y - value))

#define randThree(x,y,z) (rand()%3 == 0? x : rand()%2 == 1? y : z)
#define randTwo(x,y) (rand()%2 == 0? x : y);

GameState Process(cv::Mat &input, cv::Mat &output, GameState &state, int vOffsetLowerBar) {
	cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
	cv::threshold(output, output, threshval, 255, cv::THRESH_BINARY);

	state.paddle_x = findPaddleStart(output);
	state.paddle_y = output.rows - FIND_PADDLE_Y;
	state.paddleWidth = findPaddleEnd(output) - state.paddle_x;
	state.paddleHalfWidth = state.paddleWidth / 2;

	state.prevBall = state.curBall;
	state.curBall = findBall(output);

	state.prevIsBallVisible = state.isBallVisible;
	state.isBallVisible = state.curBall.x != -1;

	if (state.isBallVisible && !state.prevIsBallVisible) {
		state.prevTargetDeviation = state.targetDeviation;
	}

	if (cv::getTickCount() / cv::getTickFrequency() - state.timestamp > .05) {

		int left = 0;
		int middle = 0;
		int right = 0;
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 18; x++) {
				if (state.bricks[x][y]) {
					left += x < 6;
					middle += x >= 6 && x < 12;
					right += x >= 12;
				}
			}
		}
		printf("LEFT: %d, MIDDLE: %d, RIGHT: %d         \r", left, middle, right);

		if (state.isBallVisible && !state.prevIsBallVisible) {
			int maxval = max(max(left, middle), right);
			if (maxval == left) {
				state.targetDeviation = state.paddleWidth * 0.2 + (rand() % 10);
			}
			else if (maxval == middle)
				state.targetDeviation = state.paddleHalfWidth + (rand()%2==0? -rand()%20: rand()%20);
			else {
				state.targetDeviation = state.paddleWidth * 0.8 - (rand() % 10);
			}
		}
		double tempSlope = getSlope(state.curBall, state.prevBall);

		if (state.isBallVisible && (tempSlope != 0) && (state.prevBall.y < state.curBall.y) &&
			(isDistantBy(state.curBall.x, state.prevBall.x, BALL_DELTAMIN) ||
				isDistantBy(state.curBall.y, state.prevBall.y, BALL_DELTAMIN)) &&
			(state.curBall.y > BAR && state.prevBall.y > BAR) &&
			(state.curBall.y < (LOWER_BAR-vOffsetLowerBar) && state.prevBall.y < (LOWER_BAR-vOffsetLowerBar) )) {

			state.ballSlope = tempSlope;
			state.newSlope = true;
		}
		else {
			state.newSlope = false;
		}

		if (isPlusOrMinus(state.paddle_x, state.paddleDest, PADDLE_PROXMIN)) {
			state.paddleDest = state.paddle_x;
		}

		cv::Point point = findYIntercept(state.ballSlope, state.curBall, output.rows - FIND_PADDLE_Y);
		if (!isInBounds(point.x, SCREEN_X, SCREEN_W)) {
			int newX = point.x < SCREEN_X ? SCREEN_X : SCREEN_W;
			int midY = state.ballSlope * newX + (state.curBall.y - (state.ballSlope * state.curBall.x));
			point = findYIntercept(-state.ballSlope, { newX, midY }, output.rows - FIND_PADDLE_Y);
		}
		if (point.x > 0 && state.newSlope)
			state.paddleDest = max(SCREEN_X, min(576, point.x - state.targetDeviation));

		state.timestamp = cv::getTickCount() / cv::getTickFrequency();
	}

	findBricks(output, state);

	return state;
}

int findPaddleStart(cv::Mat &input) {
	uchar *end = input.ptr<uchar>(input.rows - FIND_PADDLE_Y) + FIND_PADDLE_X;
	uchar *start = end;
	uchar paddleC = *(end - 40);

	while (*end != paddleC) end++;

	return (int)(end - start) + FIND_PADDLE_X;
}

int findPaddleEnd(cv::Mat &input) {
	uchar *end = input.ptr<uchar>(input.rows - FIND_PADDLE_Y) + 608;
	uchar *start = end;
	uchar paddleC = *end--;

	while (*end != paddleC) end--;

	return (int)(end - start + 608);
}

cv::Point findBall(cv::Mat &input) {
	//Find the ball inside of this rectangle { FIND_PADDLE_X - 3, 146+FIND_PADDLE_Y, 576, 190 }

	uchar ballC = *(input.ptr<uchar>(input.rows - FIND_PADDLE_Y) - 5);
	uchar* p;

	for (int y = 186; y < 376; ++y) {
		p = input.ptr<uchar>(y);
		for (int x = 32; x < 608; ++x) {
			if (p[x] == ballC)
				return{ x, y };
		}
	}

	return{ -1, -1 };
}

void findBricks(cv::Mat &input, GameState &state) {
	//Find the ball inside of this rectangle { FIND_PADDLE_X - 3, 146+FIND_PADDLE_Y, 576, 190 }

	uchar brickC = *(input.ptr<uchar>(input.rows - FIND_PADDLE_Y) - 5);
	uchar* p;

	for (int y = 0; y < 6; y++) {
		int cy = 114 + (y * 12);
		p = input.ptr<uchar>(cy);
		for (int x = 0; x < 18; x++) {
			int cx = (x * 32) + 32;
			if (p[cx] == brickC && p[cx + 15] == brickC) {
				p[cx] = 120;
				state.bricks[x][y] = true;
				//cv::rectangle(input, { cx,cy,32,12 }, cv::Scalar(120, 120, 120), 1);
			}
			else
				state.bricks[x][y] = false;
		}
	}
}
#pragma once
#include "main.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

typedef struct {
	int paddle_x;
	int paddle_y;

	int paddleWidth;
	float paddleHalfWidth;

	double targetDeviation;
	double prevTargetDeviation;
	int returnCount;
	int paddleDest;

	cv::Point prevBall;
	cv::Point curBall;

	double ballSlope;
	bool newSlope;
	bool isBallVisible;
	bool prevIsBallVisible;

	bool bricks[18][6];

	double timestamp;

}GameState;

#define FIND_PADDLE_Y 42
#define FIND_PADDLE_X 32

#define SCREEN_W 608
#define SCREEN_X 32

#define BALL_DELTAMIN  7
#define PADDLE_PROXMIN (state.paddleWidth * 0.1)
#define BAR 210
#define LOWER_BAR 370

// Parsing
GameState Process(cv::Mat &input, cv::Mat &output, GameState &state, int vOffsetLowerBar);

int findPaddleStart(cv::Mat &input);
int findPaddleEnd(cv::Mat &input);
cv::Point findBall(cv::Mat &input);

void findBricks(cv::Mat &input, GameState &state);
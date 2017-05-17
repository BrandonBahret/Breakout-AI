#include "main.h"

using namespace cv;

int vOffsetLowerBar;

void drawOverlays(Mat output, GameState &state) {

	// The location on the paddle which the ball is to land.
	int realPaddleDest = state.paddle_x + state.targetDeviation;

	// Draw rectangle around the ball search area.
	//rectangle(output, { FIND_PADDLE_X, 146 + FIND_PADDLE_Y, 576, 190 }, Scalar(50, 200, 100), 1);

	// Draw rectangle around the brick search area.
	rectangle(output, { FIND_PADDLE_X, 146 - 75 + FIND_PADDLE_Y, 576, 190 - 116 }, Scalar(50, 200, 250), 1);

	// Draw line through the upper bar.
	line(output, { SCREEN_X,BAR }, { SCREEN_W,BAR }, Scalar(230, 75, 60), 2);

	// Draw line through the lower bar.
	line(output, { SCREEN_X,LOWER_BAR - vOffsetLowerBar }, { SCREEN_W,LOWER_BAR - vOffsetLowerBar }, Scalar(230, 75, 250), 2);

	// Draw a rectangle around the ball
	rectangle(output, { state.curBall.x, state.curBall.y, 9, 9 }, Scalar(40, 200, 120));

	// Line above paddle indicating where the ball is to land.
	//line(output, { realPaddleDest, state.paddle_y }, { realPaddleDest, state.paddle_y - 3 }, Scalar(23, 150, 67), 3);

	// Green box where the paddle should be moved.
	rectangle(output, { state.paddleDest,output.rows - FIND_PADDLE_Y ,state.paddleWidth,10 }, Scalar(23, 150, 32), 2);

	// Draw the ball's path
	cv::Point finalBallDest;
	if (state.newSlope && state.isBallVisible) {
		cv::Point ballDest = findYIntercept(state.ballSlope, state.curBall, state.paddle_y);
		finalBallDest = ballDest;

		if (!isInBounds(ballDest.x, SCREEN_X, SCREEN_W)) {
			int newX = ballDest.x < SCREEN_X ? SCREEN_X : SCREEN_W;
			int midY = state.ballSlope * newX + (state.curBall.y - (state.ballSlope * state.curBall.x));
			ballDest = { newX, midY };

			finalBallDest = findYIntercept(-state.ballSlope, ballDest, state.paddle_y);

			// Reflection of the ball path.
			line(output, ballDest, finalBallDest, Scalar(75, 220, 50), 3);
		}

		// Initial ball path
		line(output, state.curBall, ballDest, Scalar(75, 220, 50), 3);
	}

}

void handleGameState(GameState &state) {
	if (!isCursorShown() && !isPlusOrMinus(state.paddle_x, state.paddleDest, PADDLE_PROXMIN)) {
		movePaddleTo(state.paddle_x, state.paddleDest);
	}

	else if (!state.isBallVisible)
		newBall();

}


int main(int argc, char **argv) {

	HWND SRC = GetHWNDFromExecutableName("Stella.exe");
	if (SRC == NULL) {
		printf("Make sure Stella.exe is open.\n");
		system("Pause");
		exit(0);
	}

	//Ensure the window we have a handle to isn't minimized.
	ShowWindow(SRC, SW_RESTORE);
	SetForegroundWindow(SRC);

	//We use WinCap object to capture the bitmap from Stella.exe's screen.
	WinCap capture(SRC);

	//Create matrix objects to store the image data for OpenCV
	Mat frame(capture.sourceHeight, capture.sourceWidth, CV_8UC4, capture.bitmap), operationsFrame;

	//Creates a resizable window. (for the main image)
	const char *WINDOWNAME = "Window Capture | esc to quit";
	cv::namedWindow(WINDOWNAME, cv::WINDOW_KEEPRATIO);
	cv::resizeWindow(WINDOWNAME, 640, 360);

	vOffsetLowerBar = 10;
	cv::createTrackbar("Lower_Bar", WINDOWNAME, &vOffsetLowerBar, 500);

	//This 'GameState' struct will hold the data parsed from the captured bitmaps.
	GameState gameState = {};
	double time, deltaTime = 0;

	while (true) {
		time = cv::getTickCount() / cv::getTickFrequency();
		Sleep(12);

		if (deltaTime > 0.03) {
			capture.updateBitmap();

			flip(frame, frame, 0);

			gameState = Process(frame, operationsFrame, gameState, vOffsetLowerBar);
			//imshow("op", opFrame);

			handleGameState(gameState);
		}

		drawOverlays(frame, gameState);

		//Display the images stored in both the main image and the operation image.
		if (!frame.empty())
			cv::imshow(WINDOWNAME, frame);

		char key = (char)cv::waitKey(1);
		if (key == ESCAPE_KEY)
			return 0;

		deltaTime += (cv::getTickCount() / cv::getTickFrequency()) - time;
	}

	return NULL;
}

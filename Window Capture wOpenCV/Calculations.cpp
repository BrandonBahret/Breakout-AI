#include "Calculations.h"
// Calculation Code

float getSlope(cv::Point p1, cv::Point p2) {
	if ((p1.x - p2.x) == 0)
		return 0;

	return (p1.y - p2.y) / (float)(p1.x - p2.x);
}

double roundDouble(double num, int n) {
	if (num == 0) {
		return 0;
	}

	double d = ceil(log10(num < 0 ? -num : num));
	int power = n - (int)d;

	double magnitude = pow(10, power);
	long shifted = round(num*magnitude);
	return shifted / magnitude;
}

cv::Point findYIntercept(double slope, cv::Point ballPos, double YValue) {

	double b = ballPos.y - (slope * ballPos.x);
	//Y = MX + B
	//X = (Y - B) / M
	int XIntercept = (YValue - b) / slope;
	int YIntercept = slope * XIntercept + b;

	return{ XIntercept ,YIntercept };
}
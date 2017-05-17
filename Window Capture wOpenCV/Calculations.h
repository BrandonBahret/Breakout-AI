#pragma once
#include <opencv2/imgproc/imgproc.hpp>

// Calculations
float getSlope(cv::Point p1, cv::Point p2);
double roundDouble(double num, int n);

cv::Point findYIntercept(double slope, cv::Point ballPos, double YValue);
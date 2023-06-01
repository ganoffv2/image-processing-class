#pragma once
#include <opencv2/core.hpp>

#define BSIZE 4
void rgb2ycbcr(cv::Mat &image);

void mozaic(std::vector<cv::Mat> &in);
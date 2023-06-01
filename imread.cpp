#include <cstdio>
#include <opencv2/opencv.hpp>

#include "mytools.hpp"
#include "vector"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("An input image file is missing.\n");
    return EXIT_FAILURE;
  }
  cv::Mat image;
  image = cv::imread(argv[1], cv::ImreadModes::IMREAD_COLOR);
  if (image.empty()) {
    printf("Image file is not found.\n");
    return EXIT_FAILURE;
  }
  rgb2ycbcr(image);
  cv::Mat ycrcb<cv::Mat> ycrcb;
  cv::split(image, ycrcb);

  mozaic(ycrcb);

  cv::marge(ycrcb, image);

  cv::cvtColor[image, image, cv::COLOR_YCbCR2BGR];
  cv::imshow("image", image);
  cv::waitKey();
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}
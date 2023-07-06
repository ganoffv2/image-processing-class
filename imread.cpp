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
  std::vector<cv::Mat> ycrcb;
  cv::split(image, ycrcb);

  for (int c = 0; c < image.channels(); ++c) {
    cv::Mat buf;
    ycrcb[c].convertTo(buf, CV_32F);

    blkproc(buf, blk::dct2);
    blkproc(buf, blk::quantize);
    blkproc(buf, blk::deuantize);
    blkproc(buf, blk::idct2);

    buf.converTo(ycrcb[c], ycrcb[c].type())
  }

  cv::marge(ycrcb, image);

  cv::cvtColor[image, image, cv::COLOR_YCbCR2BGR];
  cv::imshow("image", image);
  cv::waitKey();
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}
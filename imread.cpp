#include <cstdio>
#include <opencv2/opencv.hpp>

int main(int argc ,char *argv[]) {
  cv::Mat image;
  image = cv::imread(argv[1]);
  if (image.empty()) {
    printf("Image file is not found.\n");
    return EXIT_FAILURE;
  }
  cv::imshow("image", image);
  cv::waitKey();
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}

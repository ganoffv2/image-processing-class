#include <cstdio>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Erro");
    return EXIT_FAILURE;
  }
  cv::Mat image;
  image = cv::imread(argv[1]);
  if (image.empty()) {
    printf("Image file is not found.\n");
    return EXIT_FAILURE;
  }
  printf("wdth = %d, hight = %d", image.cols, image.rows);
  for (int y = 0; image.rows; ++y) {
    for (int x = 0; image.cols; ++x) {
      int val = image.data[y * image.cols + x];
      val /= 2;
      image.data[y * image.cols + x] = val;
    }
  }
  cv::imshow("image", image);
  cv::waitKey();
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}

#include <chrono>
#include <cstdio>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

#include "jpgheaders.hpp"
#include "mytools.hpp"
#include "ycctype.hpp"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("An input image file is missing.\n");
    return EXIT_FAILURE;
  }
  cv::Mat image;
  image = cv::imread(argv[1], cv::ImreadModes::IMREAD_ANYCOLOR);
  if (image.empty()) {
    printf("Image file is not found.\n");
    return EXIT_FAILURE;
  }
  cv::Mat original = image.clone();
  if (argc < 3) {
    printf("Qfactor is missing.\n");
    return EXIT_FAILURE;
  }
  int QF = strtol(argv[2], nullptr, 10);
  if (QF < 0 || QF > 100) {
    printf("Valid range for Qfactor is from 0 to 100.\n");
    return EXIT_FAILURE;
  }
  QF = (QF == 0) ? 1 : QF;
  float scale;
  if (QF < 50) {
    scale = 5000.0 / QF;
  } else {
    scale = 200 - QF * 2;
  }

  int qtable_L[64], qtable_C[64];
  create_qtable(0, scale, qtable_L);
  create_qtable(1, scale, qtable_C);

  bitstream enc;
  int YCCtype = (image.channels() == 3) ? YCC::YUV420 : YCC::GRAY;
  create_mainheader(image.cols, image.rows, image.channels(), qtable_L,
                    qtable_C, YCC::YUV420, enc);

  // auto start = std::chrono::high_resolution_clock::now();
  long long duration = 0, DCTtime = 0, Qtime = 0, Entropytime = 0;
  if (image.channels() == 3) bgr2ycrcb(image);
  std::vector<cv::Mat> ycrcb;
  std::vector<cv::Mat> buf(image.channels());
  cv::split(image, ycrcb);

  constexpr float D = 2;
  // encoder
  for (int c = 0; c < image.channels(); ++c) {
    int *qtable = qtable_L;
    if (c > 0) {
      qtable = qtable_C;
      cv::resize(ycrcb[c], ycrcb[c], cv::Size(), 1 / D, 1 / D,
                 cv::INTER_LINEAR_EXACT);
    }
    ycrcb[c].convertTo(buf[c], CV_32F);
    buf[c] -= 128.0;  // DC level shift
    // DCT の計測はじめ
    auto DCTstart = std::chrono::high_resolution_clock::now();
    blkproc(buf[c], blk::dct2);
    // DCT　の計測終わり
    auto DCTstop = std::chrono::high_resolution_clock::now();

    DCTtime += std::chrono::duration_cast<std::chrono::microseconds>(DCTstop -
                                                                     DCTstart)
                   .count();

    // 量子化計測はじめ
    auto Qstart = std::chrono::high_resolution_clock::now();
    blkproc(buf[c], blk::quantize, qtable);
    // 量子化計測終わり
    auto Qstop = std::chrono::high_resolution_clock::now();

    Qtime +=
        std::chrono::duration_cast<std::chrono::microseconds>(Qstop - Qstart)
            .count();
  }
  auto Entoropystart = std::chrono::high_resolution_clock::now();
  Encode_MCUs(buf, enc, YCCtype);
  auto Entropystop = std::chrono::high_resolution_clock::now();

  Entropytime += std::chrono::duration_cast<std::chrono::microseconds>(
                     Entropystop - Entoropystart)
                     .count();

  // auto stop = std::chrono::high_resolution_clock::now();

  // duration +=
  //  std::chrono::duration_cast<std::chrono::microseconds>(stop - start)
  //  .count();
  // printf("Elapsed Time: %f [ms]\n", duration / 1000.0);
  printf("DCT Time: %f [ms]\n", DCTtime / 1000.0);
  printf("Quantize Time: %f [ms]\n", Qtime / 1000.0);
  printf("Entropyuantize Time: %f [ms]\n", Entropytime / 1000.0);
  const std::vector<uint8_t> codestream = enc.finalize();
  printf("codestream size = %lld\n", codestream.size());

  FILE *fout = fopen("myjpg.jpg", "wb");
  fwrite(codestream.data(), sizeof(unsigned char), codestream.size(), fout);
  fclose(fout);
  // decoder
  for (int c = 0; c < image.channels(); ++c) {
    int *qtable = qtable_L;
    if (c > 0) {
      qtable = qtable_C;
    }
    blkproc(buf[c], blk::dequantize, qtable);
    blkproc(buf[c], blk::idct2);
    buf[c] += 128.0;  // inverse DC level shift
    buf[c].convertTo(ycrcb[c], ycrcb[c].type());
    if (c > 0) {
      cv::resize(ycrcb[c], ycrcb[c], cv::Size(), D, D, cv::INTER_LINEAR_EXACT);
    }
  }

  cv::merge(ycrcb, image);

  if (image.channels() == 3) cv::cvtColor(image, image, cv::COLOR_YCrCb2BGR);

  myPSNR(original, image);

  cv::imshow("image", image);
  cv::waitKey();
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}
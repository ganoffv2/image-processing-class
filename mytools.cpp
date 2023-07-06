#include "mytools.hpp"

void bgr2ycrcb(cv::Mat &image) {
  const int WIDTH = image.cols;
  const int HEIGHT = image.rows;
  const int NC = image.channels();
  // const int STRIDE = WIDTH * NC;
  uchar *p0, *p1, *p2;
  p0 = image.data;      // B
  p1 = image.data + 1;  // G
  p2 = image.data + 2;  // R
  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x, p0 += NC, p1 += NC, p2 += NC) {
      int B = *p0, G = *p1, R = *p2;

      double Y = 0.299 * R + 0.587 * G + 0.114 * B;
      double Cb = -0.1687 * R - 0.3313 * G + 0.5 * B + 128;
      double Cr = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;

      *p0 = static_cast<uchar>(roundl(Y));
      *p1 = static_cast<uchar>(roundl(Cr));
      *p2 = static_cast<uchar>(roundl(Cb));
    }
  }
}

void blk::mozaic(cv::Mat &in, int p0, float p1) {
  float *sp = (float *)in.data;
  in.forEach<float>([&](float &v, const int *pos) -> void { v = sp[0]; });
  // float *sp = (float *)in.data;
  // for (int i = 0; i < in.rows; ++i) {
  //   for (int j = 0; j < in.cols; ++j) {
  //     sp[i * in.cols + j] = sp[0];
  //   }
  // }
}

void blk::quantize(cv::Mat &in, int c, float scale) {
  if (scale < 0.0) {
    return;
  }
  in.forEach<float>([&](float &v, const int *pos) -> void {
    float stepsize = blk::qmatrix[c][sop[0] * in.col + sop[1] * scale];
    v /= stepsize;
    v = roundf(v);
  });
}

void blk::dequantize(cv::Mat &in, int c, float scale) {
  if (scale < 0.0) {
    return;
  }
  in.forEach<float>([&](float &v, const int *pos) -> void {
    float stepsize = blk::qmatrix[c][sop[0] * in.col + sop[1] * scale];
    v *= stepsize;
    v = roundf(v);
  });
}

void blk::dct2(cv::Mat &in, int p0, float p1) { cv::dct(in, in); }

void blk::idct2(cv::Mat &in, int p0, float p1) { cv::idct(in, in); }

void blkproc(cv::Mat &in, std::function<void(cv::Mat &, int, float)> func,
             int p0, float p1) {
  for (int y = 0; y < in.rows; y += BSIZE) {
    for (int x = 0; x < in.cols; x += BSIZE) {
      cv::Mat blk_in = in(cv::Rect(x, y, BSIZE, BSIZE)).clone();
      cv::Mat blk_out = in(cv::Rect(x, y, BSIZE, BSIZE));
      func(blk_in, p0, p1);
      blk_in.convertTo(blk_out, blk_out.type());
    }
  }
}

// double MSE( char* imageA,char* imageB,int WIDTH, int HEIGHT){
//   double mse = 0.0;
//   int Count = WIDTH * HEIGHT;
//   for(int i = 0; i < Count;i++){
//     double mseR = imageA[i * 3] - imageB[i*3];
//     double mseG = imageA[i * 3+1] - imageB[i*3+1];
//     double mseB = imageA[i * 3+2] - imageB[i*3+2];
//   }
//   mse /= (Count*3)

//   double PSNR = 10*log10((255^2)/mse);
// }
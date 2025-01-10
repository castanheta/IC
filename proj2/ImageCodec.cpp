#include "ImageCodec.h"
#include <cmath>

ImageCodec::ImageCodec(uint32_t m) : golomb(m) {}

static int predictPixel(const cv::Mat &image, int row, int col, int channel) {
  int a = (col > 0) ? image.at<cv::Vec3b>(row, col - 1)[channel] : 0;
  int b = (row > 0) ? image.at<cv::Vec3b>(row - 1, col)[channel] : 0;
  int c =
      (row > 0 && col > 0) ? image.at<cv::Vec3b>(row - 1, col - 1)[channel] : 0;

  // JPEG-LS prediction formula
  if (c >= std::max(a, b)) {
    return std::min(a, b);
  } else if (c <= std::min(a, b)) {
    return std::max(a, b);
  } else {
    return a + b - c;
  }
}

void ImageCodec::encode(const cv::Mat &image, const std::string &outputFile) {
  BitStream bitStream(outputFile, true);

  bitStream.writeBits(image.rows, 16);
  bitStream.writeBits(image.cols, 16);

  for (int row = 0; row < image.rows; ++row) {
    for (int col = 0; col < image.cols; ++col) {
      for (int channel = 0; channel < 3; ++channel) {
        int actualValue = image.at<cv::Vec3b>(row, col)[channel];
        int predictedValue = predictPixel(image, row, col, channel);
        int residual = actualValue - predictedValue;
        golomb.encodeInteger(residual, bitStream);
      }
    }
  }
}

cv::Mat ImageCodec::decode(const std::string &inputFile) {
  BitStream bitStream(inputFile, false);

  // Read image dimensions
  int rows = bitStream.readBits(16);
  int cols = bitStream.readBits(16);

  cv::Mat decodedImage(rows, cols, CV_8UC3);

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      for (int channel = 0; channel < 3; ++channel) {
        int predictedValue = predictPixel(decodedImage, row, col, channel);

        // Decode residual using Golomb coding
        int residual = golomb.decodeInteger(bitStream);

        int actualValue = predictedValue + residual;
        decodedImage.at<cv::Vec3b>(row, col)[channel] =
            static_cast<uchar>(actualValue);
      }
    }
  }
  return decodedImage;
}

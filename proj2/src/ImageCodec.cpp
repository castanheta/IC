#include "ImageCodec.h"
#include <cmath>

ImageCodec::ImageCodec(uint32_t m) : golomb(m), quantLevel(1) {}

void ImageCodec::setQuantizationLevel(uint32_t level) { quantLevel = level; }

static int predictPixel(const cv::Mat &image, int row, int col, int channel) {
  int a = (col > 0) ? image.at<cv::Vec3b>(row, col - 1)[channel] : 0;
  int b = (row > 0) ? image.at<cv::Vec3b>(row - 1, col)[channel] : 0;
  int c =
      (row > 0 && col > 0) ? image.at<cv::Vec3b>(row - 1, col - 1)[channel] : 0;

  if (c >= std::max(a, b)) {
    return std::min(a, b);
  } else if (c <= std::min(a, b)) {
    return std::max(a, b);
  } else {
    return a + b - c;
  }
}

static int quantize(int value, int quantLevel) {
  if (quantLevel < 0 || quantLevel > 7) {
    throw std::invalid_argument("quantLevel must be between 0 and 7");
  }
  return value >> quantLevel;
}

static int dequantize(int value, int quantLevel) {
  if (quantLevel < 0 || quantLevel > 7) {
    throw std::invalid_argument("quantLevel must be between 0 and 7");
  }
  return value << quantLevel;
}

void ImageCodec::encode(const cv::Mat &image, const std::string &outputFile) {
  cv::Mat modifiedImage = image.clone();
  BitStream bitStream(outputFile, true);

  bitStream.writeBits(image.rows, 16);
  bitStream.writeBits(image.cols, 16);

  for (int row = 0; row < image.rows; ++row) {
    for (int col = 0; col < image.cols; ++col) {
      for (int channel = 0; channel < 3; ++channel) {
        int actualValue = modifiedImage.at<cv::Vec3b>(row, col)[channel];
        int predictedValue = predictPixel(modifiedImage, row, col, channel);
        int residual = actualValue - predictedValue;

        int quantizedResidual = quantize(residual, quantLevel);
        // propagate the quantized residual to the actual value
        int quantizedActualValue =
            predictedValue + dequantize(quantizedResidual, quantLevel);
        modifiedImage.at<cv::Vec3b>(row, col)[channel] =
            static_cast<uchar>(std::clamp(quantizedActualValue, 0, 255));
        golomb.encodeInteger(quantizedResidual, bitStream);
      }
    }
  }
}

cv::Mat ImageCodec::decode(const std::string &inputFile) {
  BitStream bitStream(inputFile, false);

  int rows = bitStream.readBits(16);
  int cols = bitStream.readBits(16);

  cv::Mat decodedImage(rows, cols, CV_8UC3);

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      for (int channel = 0; channel < 3; ++channel) {
        int predictedValue = predictPixel(decodedImage, row, col, channel);

        int quantizedResidual = golomb.decodeInteger(bitStream);
        int residual = dequantize(quantizedResidual,
                                  quantLevel); // Dequantize the residual

        int actualValue = predictedValue + residual;

        decodedImage.at<cv::Vec3b>(row, col)[channel] =
            static_cast<uchar>(std::clamp(actualValue, 0, 255));
      }
    }
  }
  return decodedImage;
}

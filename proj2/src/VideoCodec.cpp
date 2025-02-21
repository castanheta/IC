#include "VideoCodec.h"
#include "BitStream.h"
#include <cmath>
#include <stdexcept>

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

VideoCodec::VideoCodec(uint32_t m) : golomb(m) {}

void VideoCodec::encode(const std::vector<cv::Mat> &frames,
                        const std::string &outputFile, uint32_t golombM,
                        int quantizationLevel) {
  if (frames.empty()) {
    throw std::runtime_error("No frames to encode.");
  }

  BitStream bitStream(outputFile, true);

  bitStream.writeBits(golombM, 32);

  int width = frames[0].cols;
  int height = frames[0].rows;
  int numFrames = static_cast<int>(frames.size());

  bitStream.writeBits(width, 32);
  bitStream.writeBits(height, 32);
  bitStream.writeBits(numFrames, 32);

  bitStream.writeBits(quantizationLevel, 16);

  for (const auto &originalFrame : frames) {
    if (originalFrame.cols != width || originalFrame.rows != height ||
        originalFrame.type() != CV_8UC3) {
      throw std::runtime_error(
          "All frames must have the same dimensions and type.");
    }

    cv::Mat frame = originalFrame.clone();

    for (int row = 0; row < frame.rows; ++row) {
      for (int col = 0; col < frame.cols; ++col) {
        for (int channel = 0; channel < 3; ++channel) {
          int actualValue = frame.at<cv::Vec3b>(row, col)[channel];
          int predictedValue = predictPixel(frame, row, col, channel);
          int residual = actualValue - predictedValue;

          int quantizedResidual = quantize(residual, quantizationLevel);

          // Propagate the quantized residual to the actual value
          int quantizedActualValue =
              predictedValue + dequantize(quantizedResidual, quantizationLevel);
          frame.at<cv::Vec3b>(row, col)[channel] =
              static_cast<uchar>(std::clamp(quantizedActualValue, 0, 255));

          golomb.encodeInteger(quantizedResidual, bitStream);
        }
      }
    }
  }
}

std::vector<cv::Mat> VideoCodec::decode(const std::string &inputFile) {
  BitStream bitStream(inputFile, false);

  uint32_t golombM = bitStream.readBits(32);
  int width = bitStream.readBits(32);
  int height = bitStream.readBits(32);
  int numFrames = bitStream.readBits(32);
  int quantizationLevel = bitStream.readBits(16);

  GolombCoding golombDecoder(golombM);

  std::vector<cv::Mat> frames;
  frames.reserve(numFrames);

  for (int i = 0; i < numFrames; ++i) {
    cv::Mat frame(height, width, CV_8UC3);
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        for (int channel = 0; channel < 3; ++channel) {
          int predictedValue = predictPixel(frame, row, col, channel);

          int quantizedResidual = golombDecoder.decodeInteger(bitStream);
          int residual = dequantize(quantizedResidual, quantizationLevel);
          int actualValue = predictedValue + residual;

          frame.at<cv::Vec3b>(row, col)[channel] =
              static_cast<uchar>(std::clamp(actualValue, 0, 255));
        }
      }
    }
    frames.push_back(frame);
  }

  return frames;
}

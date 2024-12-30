#include "VideoCodecIntra.h"
#include "BitStream.h"
#include <stdexcept>

// Helper function for spatial prediction
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

VideoCodecIntra::VideoCodecIntra(uint32_t m) : golomb(m) {}

void VideoCodecIntra::encode(const std::vector<cv::Mat> &frames,
                             const std::string &outputFile, uint32_t golombM) {
  if (frames.empty()) {
    throw std::runtime_error("No frames to encode.");
  }

  BitStream bitStream(outputFile, true);

  // Write Golomb parameter
  bitStream.writeBits(golombM, 32);

  // Write video dimensions and number of frames
  int width = frames[0].cols;
  int height = frames[0].rows;
  int numFrames = static_cast<int>(frames.size());

  bitStream.writeBits(width, 32);
  bitStream.writeBits(height, 32);
  bitStream.writeBits(numFrames, 32);

  // Encode each frame
  for (const auto &frame : frames) {
    if (frame.cols != width || frame.rows != height ||
        frame.type() != CV_8UC3) {
      throw std::runtime_error(
          "All frames must have the same dimensions and type.");
    }

    for (int row = 0; row < frame.rows; ++row) {
      for (int col = 0; col < frame.cols; ++col) {
        for (int channel = 0; channel < 3; ++channel) {
          int actualValue = frame.at<cv::Vec3b>(row, col)[channel];
          int predictedValue = predictPixel(frame, row, col, channel);
          int residual = actualValue - predictedValue;
          golomb.encodeInteger(residual, bitStream);
        }
      }
    }
  }
}

std::vector<cv::Mat> VideoCodecIntra::decode(const std::string &inputFile) {
  BitStream bitStream(inputFile, false);

  // Read Golomb parameter
  uint32_t golombM = bitStream.readBits(32);
  GolombCoding golombDecoder(golombM);

  // Read video dimensions and number of frames
  int width = bitStream.readBits(32);
  int height = bitStream.readBits(32);
  int numFrames = bitStream.readBits(32);

  std::vector<cv::Mat> frames;
  frames.reserve(numFrames);

  // Decode each frame
  for (int i = 0; i < numFrames; ++i) {
    cv::Mat frame(height, width, CV_8UC3);
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        for (int channel = 0; channel < 3; ++channel) {
          int predictedValue = predictPixel(frame, row, col, channel);

          // Decode residual using Golomb coding
          int residual = golombDecoder.decodeInteger(bitStream);

          int actualValue = predictedValue + residual;
          frame.at<cv::Vec3b>(row, col)[channel] =
              static_cast<uchar>(actualValue);
        }
      }
    }
    frames.push_back(frame);
  }

  return frames;
}

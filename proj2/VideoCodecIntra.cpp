#include "VideoCodecIntra.h"
#include "BitStream.h"
#include <stdexcept>

VideoCodecIntra::VideoCodecIntra(uint32_t m)
    : golomb(m, GolombCoding::ZIGZAG) {}

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
        cv::Vec3b pixel = frame.at<cv::Vec3b>(row, col);
        for (int channel = 0; channel < 3; ++channel) {
          golomb.encodeInteger(pixel[channel], bitStream);
        }
      }
    }
  }
}

std::vector<cv::Mat> VideoCodecIntra::decode(const std::string &inputFile) {
  BitStream bitStream(inputFile, false);

  // Read Golomb parameter
  uint32_t golombM = bitStream.readBits(32);
  GolombCoding golombDecoder(golombM, GolombCoding::ZIGZAG);

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
        cv::Vec3b pixel;
        for (int channel = 0; channel < 3; ++channel) {
          pixel[channel] =
              static_cast<uchar>(golombDecoder.decodeInteger(bitStream));
        }
        frame.at<cv::Vec3b>(row, col) = pixel;
      }
    }
    frames.push_back(frame);
  }

  return frames;
}

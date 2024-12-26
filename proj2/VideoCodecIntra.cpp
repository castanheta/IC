#include "VideoCodecIntra.h"
#include "BitStream.h"
#include <stdexcept>

VideoCodecIntra::VideoCodecIntra(uint32_t defaultM)
    : defaultM(defaultM), imageCodec(defaultM) {}

void VideoCodecIntra::encode(const std::vector<cv::Mat> &frames,
                             const std::string &outputFile, uint32_t m) {
  if (frames.empty()) {
    throw std::runtime_error("No frames to encode.");
  }

  BitStream bitStream(outputFile, true);

  // Write number of frames, frame dimensions, and Golomb parameter
  bitStream.writeBits(frames.size(), 32);
  bitStream.writeBits(frames[0].rows, 32);
  bitStream.writeBits(frames[0].cols, 32);
  bitStream.writeBits(m, 32);

  // Calculate optimal m dynamically for each frame
  for (const auto &frame : frames) {
    if (frame.empty()) {
      throw std::runtime_error("One of the frames is empty.");
    }
    uint32_t dynamicM = calculateOptimalM(frame);
    ImageCodec customImageCodec(dynamicM);
    customImageCodec.encode(frame, bitStream);
  }
}

uint32_t VideoCodecIntra::calculateOptimalM(const cv::Mat &frame) {
  cv::Scalar mean, stddev;
  cv::meanStdDev(frame, mean, stddev);
  double variance = stddev[0]; // Use the first channel for calculation.
  return static_cast<uint32_t>(std::max(1.0, sqrt(variance)));
}

std::vector<cv::Mat> VideoCodecIntra::decode(const std::string &inputFile) {
  BitStream bitStream(inputFile, false);

  // Read number of frames, frame dimensions, and Golomb parameter
  uint32_t numFrames = bitStream.readBits(32);
  int rows = bitStream.readBits(32);
  int cols = bitStream.readBits(32);
  uint32_t m = bitStream.readBits(32);

  ImageCodec customImageCodec(m);
  std::vector<cv::Mat> frames;
  for (uint32_t i = 0; i < numFrames; ++i) {
    cv::Mat frame = customImageCodec.decode(bitStream);
    if (frame.rows != rows || frame.cols != cols) {
      throw std::runtime_error("Frame dimensions mismatch during decoding.");
    }
    frames.push_back(frame);
  }

  return frames;
}

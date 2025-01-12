#ifndef VIDEO_CODEC_H
#define VIDEO_CODEC_H

#include "GolombCoding.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class VideoCodec {
public:
  VideoCodec(uint32_t m);

  void encode(const std::vector<cv::Mat> &frames, const std::string &outputFile,
              uint32_t golombM, int quantizationLevel);

  std::vector<cv::Mat> decode(const std::string &inputFile);

private:
  GolombCoding golomb;
};

#endif // VIDEO_CODEC_H

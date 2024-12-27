#ifndef VIDEO_CODEC_INTRA_H
#define VIDEO_CODEC_INTRA_H

#include "BitStream.h"
#include "GolombCoding.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class VideoCodecIntra {
public:
  explicit VideoCodecIntra(uint32_t m);

  // Encode and decode the video
  void encode(const std::vector<cv::Mat> &frames, const std::string &outputFile,
              uint32_t golombM);
  std::vector<cv::Mat> decode(const std::string &inputFile);

private:
  GolombCoding golomb;
};

#endif // VIDEO_CODEC_INTRA_H

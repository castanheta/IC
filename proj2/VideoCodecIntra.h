#ifndef VIDEO_CODEC_INTRA_H
#define VIDEO_CODEC_INTRA_H

#include "ImageCodec.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class VideoCodecIntra {
public:
  explicit VideoCodecIntra(uint32_t defaultM);

  void encode(const std::vector<cv::Mat> &frames, const std::string &outputFile,
              uint32_t m);
  std::vector<cv::Mat> decode(const std::string &inputFile);

private:
  uint32_t defaultM;
  ImageCodec imageCodec;

  uint32_t calculateOptimalM(const cv::Mat &frame);
};

#endif // VIDEO_CODEC_INTRA_H

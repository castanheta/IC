#ifndef IMAGE_CODEC_H
#define IMAGE_CODEC_H

#include "BitStream.h"
#include "GolombCoding.h"
#include <opencv2/opencv.hpp>
#include <string>

class ImageCodec {
public:
  explicit ImageCodec(uint32_t m);

  void encode(const cv::Mat &image, const std::string &outputFile);
  cv::Mat decode(const std::string &inputFile);

private:
  GolombCoding golomb;
};

#endif // IMAGE_CODEC_H

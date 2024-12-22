#ifndef IMAGE_CODEC_H
#define IMAGE_CODEC_H

#include "BitStream.h"
#include "GolombCoding.h"
#include <opencv2/opencv.hpp>
#include <string>

class ImageCodec {
public:
  ImageCodec(uint32_t golombM);
  void encode(const cv::Mat &image, const std::string &outputFile);
  cv::Mat decode(const std::string &inputFile);

private:
  uint32_t golombM;
  GolombCoding golomb;
  int predict(const cv::Mat &image, int row, int col, int channel) const;
};

#endif // IMAGE_CODEC_H

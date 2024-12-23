#ifndef IMAGE_CODEC_H
#define IMAGE_CODEC_H

#include "BitStream.h"
#include "GolombCoding.h"
#include <opencv2/opencv.hpp>
#include <string>

class ImageCodec {
public:
  explicit ImageCodec(uint32_t m);

  // Encode and decode from a file
  void encode(const cv::Mat &image, const std::string &outputFile);
  cv::Mat decode(const std::string &inputFile);

  // New: Encode and decode directly using BitStream
  void encode(const cv::Mat &image, BitStream &bitStream);
  cv::Mat decode(BitStream &bitStream);

private:
  GolombCoding golomb;
};

#endif // IMAGE_CODEC_H

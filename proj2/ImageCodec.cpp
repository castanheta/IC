#include "ImageCodec.h"
#include "BitStream.h"
#include <stdexcept>

ImageCodec::ImageCodec(uint32_t m) : golomb(m, GolombCoding::ZIGZAG) {}

void ImageCodec::encode(const cv::Mat &image, const std::string &outputFile) {
  if (image.empty()) {
    throw std::runtime_error("Input image is empty.");
  }
  BitStream bitStream(outputFile, true);

  // Write image dimensions
  bitStream.writeBits(image.rows, 32);
  bitStream.writeBits(image.cols, 32);

  // Encode pixel data
  for (int row = 0; row < image.rows; ++row) {
    for (int col = 0; col < image.cols; ++col) {
      cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);
      for (int channel = 0; channel < 3; ++channel) {
        std::cout << "Encoding pixel: " << pixel << std::endl;
        golomb.encodeInteger(pixel[channel], bitStream);
      }
    }
  }
}

cv::Mat ImageCodec::decode(const std::string &inputFile) {
  BitStream bitStream(inputFile, false);
  // Read image dimensions
  int rows = bitStream.readBits(32);
  int cols = bitStream.readBits(32);

  cv::Mat decodedImage(rows, cols, CV_8UC3);

  // Decode pixel data
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      cv::Vec3b pixel;
      for (int channel = 0; channel < 3; ++channel) {
        pixel[channel] = static_cast<uchar>(golomb.decodeInteger(bitStream));
      }
      decodedImage.at<cv::Vec3b>(row, col) = pixel;
    }
  }

  return decodedImage;
}

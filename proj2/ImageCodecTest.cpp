#include "ImageCodec.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdexcept>

using namespace std;

double calculatePSNR(const cv::Mat &original, const cv::Mat &decoded) {
  if (original.empty() || decoded.empty() ||
      original.size() != decoded.size() || original.type() != decoded.type()) {
    throw runtime_error("Invalid input for PSNR calculation.");
  }

  cv::Mat diff;
  cv::absdiff(original, decoded, diff);
  diff.convertTo(diff, CV_32F);
  diff = diff.mul(diff);

  cv::Scalar sse = cv::sum(diff); // Sum of squared errors

  double mse =
      (sse[0] + sse[1] + sse[2]) / (original.channels() * original.total());
  if (mse == 0) {
    return INFINITY; // No difference between images
  }

  double psnr = 10.0 * log10((255 * 255) / mse);
  return psnr;
}

bool verifyImages(const cv::Mat &original, const cv::Mat &decoded) {
  if (original.size() != decoded.size() || original.type() != decoded.type()) {
    return false;
  }

  for (int row = 0; row < original.rows; ++row) {
    for (int col = 0; col < original.cols; ++col) {
      if (original.at<cv::Vec3b>(row, col) != decoded.at<cv::Vec3b>(row, col)) {
        return false;
      }
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  if (argc != 6) {
    cerr << "Usage: " << argv[0]
         << " <input_image> <encoded_file> <decoded_image> <m(golomb "
            "parameter)> <quantization_level>\n";
    return 1;
  }

  string inputImage = argv[1];
  string encodedFile = argv[2];
  string decodedImage = argv[3];
  uint32_t m = stoi(argv[4]);
  uint32_t quantizationLevel = stoi(argv[5]);

  try {
    ImageCodec codec(m);

    codec.setQuantizationLevel(quantizationLevel);

    cv::Mat originalImage = cv::imread(inputImage, cv::IMREAD_COLOR);
    if (originalImage.empty()) {
      throw runtime_error("Failed to load input image.");
    }

    codec.encode(originalImage, encodedFile);

    cv::Mat decodedImageMat = codec.decode(encodedFile);

    cv::imwrite(decodedImage, decodedImageMat);

    ifstream inputFile(inputImage, ios::binary | ios::ate);
    ifstream encodedFileStream(encodedFile, ios::binary | ios::ate);

    size_t originalSize = inputFile.tellg();
    size_t encodedSize = encodedFileStream.tellg();

    double compressionRate =
        (1.0 - (double(encodedSize) / originalSize)) * 100.0;

    double psnr = calculatePSNR(originalImage, decodedImageMat);

    cout << originalSize << endl;
    cout << encodedSize << endl;
    cout << compressionRate << endl;
    cout << psnr << endl;

  } catch (const exception &e) {
    return 1;
  }

  return 0;
}

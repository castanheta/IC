#include "ImageCodec.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdexcept>

using namespace std;

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
  if (argc != 5) {
    cerr << "Usage: " << argv[0]
         << " <input_image> <encoded_file> <decoded_image> <m(golomb "
            "parameter)>\n";
    return 1;
  }

  string inputImage = argv[1];
  string encodedFile = argv[2];
  string decodedImage = argv[3];
  uint32_t m = stoi(argv[4]);

  try {
    ImageCodec codec(m);

    cout << "Loading input image...\n";
    cv::Mat originalImage = cv::imread(inputImage, cv::IMREAD_COLOR);
    if (originalImage.empty()) {
      throw runtime_error("Failed to load input image.");
    }

    cout << "Encoding image...\n";
    codec.encode(originalImage, encodedFile);

    cout << "Decoding image...\n";
    cv::Mat decodedImageMat = codec.decode(encodedFile);

    cout << "Saving decoded image...\n";
    cv::imwrite(decodedImage, decodedImageMat);

    cout << "Verifying results...\n";
    if (verifyImages(originalImage, decodedImageMat)) {
      cout << "All pixels successfully encoded and decoded!\n";
    } else {
      cout << "Mismatch detected in pixel data!\n";
    }
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}

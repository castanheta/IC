#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr
        << "Usage: ./image_processor <input_image_path> <output_image_path>"
        << std::endl;
    return 1;
  }

  // Load the input image
  cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
  if (image.empty()) {
    std::cerr << "Error: Could not open or find the image!" << std::endl;
    return 1;
  }

  // Convert the image to grayscale
  cv::Mat gray_image;
  cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

  // Save the grayscale image to the output path
  if (!cv::imwrite(argv[2], gray_image)) {
    std::cerr << "Error: Could not save the image!" << std::endl;
    return 1;
  }

  std::cout << "Image saved successfully to " << argv[2] << std::endl;

  return 0;
}

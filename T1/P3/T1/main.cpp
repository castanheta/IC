#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat loadImage(const std::string &imagePath) {
  cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
  if (image.empty()) {
    throw std::runtime_error("Error: Could not open or find the image!");
  }
  return image;
}

void displayImage(const cv::Mat &image, const std::string &window_name) {
  cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
  cv::imshow(window_name, image);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./image_processor <input_image_path>" << std::endl;
    return 1;
  }

  try {
    cv::Mat image = loadImage(argv[1]);
    displayImage(image, "Image");

    cv::waitKey(0); // Wait for a key press
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

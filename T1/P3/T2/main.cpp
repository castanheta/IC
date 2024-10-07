#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat loadImage(const std::string &imagePath) {
  cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
  if (image.empty()) {
    throw std::runtime_error("Error: Could not open or find the image!");
  }
  return image;
}

cv::Mat convertToGrayscale(const cv::Mat &image) {
  cv::Mat gray_image;
  cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
  return gray_image;
}

std::vector<cv::Mat> splitChannels(const cv::Mat &image) {
  std::vector<cv::Mat> channels(3);
  cv::split(image, channels);
  return channels;
}

cv::Mat createColorImage(const cv::Mat &channel, const std::string &color) {
  cv::Mat colored_image;
  cv::Mat blank_channel = cv::Mat::zeros(channel.size(), CV_8UC1);

  if (color == "red") {
    cv::merge(std::vector<cv::Mat>{blank_channel, blank_channel, channel},
              colored_image);
  } else if (color == "green") {
    cv::merge(std::vector<cv::Mat>{blank_channel, channel, blank_channel},
              colored_image);
  } else if (color == "blue") {
    cv::merge(std::vector<cv::Mat>{channel, blank_channel, blank_channel},
              colored_image);
  }

  return colored_image;
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
    cv::Mat gray_image = convertToGrayscale(image);
    std::vector<cv::Mat> channels = splitChannels(image);

    displayImage(gray_image, "Grayscale Image");
    displayImage(createColorImage(channels[2], "red"),
                 "Red Channel"); // Red channel
    displayImage(createColorImage(channels[1], "green"),
                 "Green Channel"); // Green channel
    displayImage(createColorImage(channels[0], "blue"),
                 "Blue Channel"); // Blue channel

    cv::waitKey(0); // Wait for a key press
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

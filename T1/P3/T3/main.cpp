#include <fstream>
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

cv::Mat calculateHistogram(const cv::Mat &gray_image) {
  int histSize = 256;       // Number of bins
  float range[] = {0, 256}; // Range of intensity values
  const float *histRange = {range};

  cv::Mat histogram;
  cv::calcHist(&gray_image, 1, 0, cv::Mat(), histogram, 1, &histSize,
               &histRange);

  // Normalize the histogram
  cv::normalize(histogram, histogram, 0, 255, cv::NORM_MINMAX);

  return histogram;
}

void exportHistogramToCSV(const cv::Mat &histogram,
                          const std::string &filePath) {
  std::ofstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Error: Could not open CSV file for writing!");
  }

  for (int i = 0; i < histogram.rows; i++) {
    file << i << "," << histogram.at<float>(i) << "\n";
  }

  file.close();
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: ./image_processor <input_image_path> <output_file_name>" << std::endl;
    return 1;
  }

  try {
    std::string outputFileName = std::string(argv[2]) + ".csv";
    cv::Mat image = loadImage(argv[1]);
    cv::Mat gray_image = convertToGrayscale(image);

    // Calculate histogram
    cv::Mat histogram = calculateHistogram(gray_image);

    // Export histogram to CSV
    exportHistogramToCSV(histogram, outputFileName);

    std::cout << "Histogram data exported to " +  outputFileName << std::endl;

  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

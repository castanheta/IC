#include <cmath>
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

cv::Mat applyGaussianFilter(const cv::Mat &image, int kernelSize,
                            double sigmaX) {
  if (kernelSize <= 0 || kernelSize % 2 == 0) {
    throw std::invalid_argument("Kernel size must be a positive odd integer.");
  }
  cv::Mat filtered_image;
  cv::GaussianBlur(image, filtered_image, cv::Size(kernelSize, kernelSize),
                   sigmaX);
  return filtered_image;
}

cv::Mat quantizeImage(const cv::Mat &image, int levels) {
  cv::Mat quantized_image = image.clone();
  double step = 256.0 / levels; // Step size for quantization

  // Apply quantization
  for (int i = 0; i < quantized_image.rows; i++) {
    for (int j = 0; j < quantized_image.cols; j++) {
      cv::Vec3b &pixel = quantized_image.at<cv::Vec3b>(i, j);
      for (int c = 0; c < 3; c++) {
        pixel[c] = static_cast<uchar>(std::round(pixel[c] / step) * step);
      }
    }
  }

  return quantized_image;
}

double calculateMSE(const cv::Mat &image1, const cv::Mat &image2) {
  if (image1.size() != image2.size()) {
    throw std::runtime_error(
        "Error: Images must have the same dimensions for MSE calculation!");
  }

  cv::Mat diff;
  cv::absdiff(image1, image2, diff);
  diff.convertTo(diff, CV_32F);
  diff = diff.mul(diff); // Square the differences

  return cv::sum(diff)[0] / (image1.rows * image1.cols);
}

double calculatePSNR(const cv::Mat &image1, const cv::Mat &image2) {
  double mse = calculateMSE(image1, image2);
  if (mse == 0)
    return 0; // MSE is zero means images are identical

  double maxPixelValue = 255.0;
  return 10 * log10((maxPixelValue * maxPixelValue) / mse);
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

cv::Mat calculateHistogram(const cv::Mat &gray_image) {
  int histSize = 256;       // Number of bins
  float range[] = {0, 256}; // Range of intensity values
  const float *histRange = {range};

  cv::Mat histogram;
  cv::calcHist(&gray_image, 1, 0, cv::Mat(), histogram, 1, &histSize,
               &histRange);

  // Normalize values to be in the range [0, 255]
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

void displayImage(const cv::Mat &image, const std::string &windowName) {
  cv::imshow(windowName, image);
  cv::waitKey(0); // Wait for a key press before closing the window
}

void processImageOptions(const cv::Mat &image, bool showOriginal,
                         bool showGrayscale, bool showRGB, bool showDiff,
                         const cv::Mat &originalImage) {
  if (showOriginal) {
    displayImage(image, "Original Image");
  }

  if (showGrayscale) {
    cv::Mat gray_image = convertToGrayscale(image);
    displayImage(gray_image, "Grayscale Image");
  }

  if (showRGB) {
    std::vector<cv::Mat> channels = splitChannels(image);
    displayImage(createColorImage(channels[2], "red"), "Red Channel");
    displayImage(createColorImage(channels[1], "green"), "Green Channel");
    displayImage(createColorImage(channels[0], "blue"), "Blue Channel");
  }

  if (showDiff) {
    if (!showOriginal) {
      displayImage(originalImage, "Original Image");
    }
    displayImage(image, "Transformed Image");
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr
        << "Usage: ./image_processor <input_image_path>\n\t[--show_original] "
           "\n\t[--show_grayscale] \n\t[--show_rgb] \n\t[--apply_gaussian "
           "<kernel_size>] "
           "\n\t[--quantize <levels>] \n\t[--export_histogram <output_file>] "
           "\n\t[--print_metrics] \n\t[--show_diff]"
        << std::endl;
    return 1;
  }

  try {
    std::string imagePath = argv[1];
    std::string outputFileName;
    std::string metricsFileName;

    // Default options
    bool showOriginal = false;
    bool showGrayscale = false;
    bool showRGB = false;
    bool applyGaussian = false;
    bool exportHistogram = false;
    bool printMetrics = false;
    bool showDiff = false;
    int gaussianKernelSize = 5;
    int quantizationLevels = 0;

    for (int i = 2; i < argc; i++) {
      std::string arg = argv[i];

      if (arg == "--show_original") {
        showOriginal = true;
      } else if (arg == "--show_grayscale") {
        showGrayscale = true;
      } else if (arg == "--show_rgb") {
        showRGB = true;
      } else if (arg == "--apply_gaussian" && i + 1 < argc) {
        applyGaussian = true;
        gaussianKernelSize = std::stoi(argv[++i]);
      } else if (arg == "--quantize" && i + 1 < argc) {
        quantizationLevels = std::stoi(argv[++i]);
      } else if (arg == "--export_histogram" && i + 1 < argc) {
        exportHistogram = true;
        outputFileName = argv[++i];
      } else if (arg == "--print_metrics") {
        printMetrics = true;
      } else if (arg == "--show_diff") {
        showDiff = true;
      }
    }

    cv::Mat image = loadImage(imagePath);
    cv::Mat originalImage = image.clone();

    if (applyGaussian) {
      image = applyGaussianFilter(image, gaussianKernelSize, 0);
    }

    if (quantizationLevels > 0) {
      image = quantizeImage(image, quantizationLevels);
    }

    processImageOptions(image, showOriginal, showGrayscale, showRGB, showDiff,
                        originalImage);

    if (printMetrics) {
      double mse = calculateMSE(originalImage, image);
      double psnr = calculatePSNR(originalImage, image);
      std::cout << mse << "," << psnr << "\n";
    }

    if (exportHistogram) {
      cv::Mat gray_image = convertToGrayscale(originalImage);
      cv::Mat histogram = calculateHistogram(gray_image);
      exportHistogramToCSV(histogram, outputFileName);
      std::cout << "Histogram exported to: " << outputFileName << std::endl;
    }

  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

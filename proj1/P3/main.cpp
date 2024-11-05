#include <cmath>
#include <cstdio>
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
  cv::Mat grayImage;
  cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
  return grayImage;
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

  if (image.channels() == 1) {
    // Handle grayscale quantization
    for (int i = 0; i < quantized_image.rows; i++) {
      for (int j = 0; j < quantized_image.cols; j++) {
        uchar &pixel = quantized_image.at<uchar>(i, j);
        pixel = static_cast<uchar>(std::round(pixel / step) * step);
      }
    }
  } else {
    // Handle color image quantization
    for (int i = 0; i < quantized_image.rows; i++) {
      for (int j = 0; j < quantized_image.cols; j++) {
        cv::Vec3b &pixel = quantized_image.at<cv::Vec3b>(i, j);
        for (int c = 0; c < 3; c++) {
          pixel[c] = static_cast<uchar>(std::round(pixel[c] / step) * step);
        }
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

  double mse;
  if (diff.channels() == 3) {
    mse = (cv::sum(diff)[0] + cv::sum(diff)[1] + cv::sum(diff)[2]) /
          (image1.rows * image1.cols * 3);
  } else {
    mse = cv::sum(diff)[0] / (image1.rows * image1.cols);
  }

  return mse;
}

double calculatePSNR(const cv::Mat &image1, const cv::Mat &image2) {
  double mse = calculateMSE(image1, image2);
  if (mse == 0)
    return 0; // MSE is zero means images are identical

  double maxPixelValue = 255.0;
  return 10 * log10((maxPixelValue * maxPixelValue) / mse);
}

cv::Mat calculateHistogram(const cv::Mat &grayImage) {
  int histSize = 256;       // Number of bins
  float range[] = {0, 256}; // Range of intensity values
  const float *histRange = {range};

  cv::Mat histogram;
  cv::calcHist(&grayImage, 1, 0, cv::Mat(), histogram, 1, &histSize,
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

void displayImages(const cv::Mat &original, const cv::Mat &processed) {
  cv::imshow("Original Image", original);
  cv::imshow("Processed Image", processed);
  cv::waitKey(0); // Wait for a key press to close the windows
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr
        << "Usage: ./image_processor <input_image_path>\n\t[--set_image "
           "<grayscale|red|green|blue|original>] \n\t[--apply_gaussian "
           "<kernel_size>] "
           "\n\t[--quantize <levels>] \n\t[--export_histogram <output_file>] "
           "\n\t[--print_metrics] \n\t[--show]"
        << std::endl;
    return 1;
  }

  try {
    std::string imagePath = argv[1];
    std::string outputFileName;
    std::string imageMode = "original"; // Default mode
    bool applyGaussian = false;
    bool exportHistogram = false;
    bool printMetrics = false;
    bool showImages = false;
    int gaussianKernelSize = 5;
    int quantizationLevels = 0;

    for (int i = 2; i < argc; i++) {
      std::string arg = argv[i];

      if (arg == "--set_image" && i + 1 < argc) {
        imageMode = argv[++i];
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
      } else if (arg == "--show") {
        showImages = true;
      }
    }

    cv::Mat originalImage = loadImage(imagePath);

    if (imageMode == "grayscale") {
      originalImage = convertToGrayscale(originalImage);
    } else if (imageMode == "red" || imageMode == "green" ||
               imageMode == "blue") {
      std::vector<cv::Mat> channels;
      cv::split(originalImage, channels);
      if (imageMode == "red") {
        channels[0] = cv::Mat::zeros(channels[0].size(), CV_8UC1);
        channels[1] = cv::Mat::zeros(channels[1].size(), CV_8UC1);
      } else if (imageMode == "green") {
        channels[0] = cv::Mat::zeros(channels[0].size(), CV_8UC1);
        channels[2] = cv::Mat::zeros(channels[2].size(), CV_8UC1);
      } else if (imageMode == "blue") {
        channels[1] = cv::Mat::zeros(channels[1].size(), CV_8UC1);
        channels[2] = cv::Mat::zeros(channels[2].size(), CV_8UC1);
      }
      cv::merge(channels, originalImage);
    }
    cv::Mat processedImage = originalImage.clone();

    if (applyGaussian) {
      processedImage =
          applyGaussianFilter(processedImage, gaussianKernelSize, 0);
    }

    if (quantizationLevels > 0) {
      processedImage = quantizeImage(processedImage, quantizationLevels);
    }

    if (printMetrics) {
      double mse = calculateMSE(originalImage, processedImage);
      double psnr = calculatePSNR(originalImage, processedImage);
      std::cout << mse << ", " << psnr << "\n";
    }

    if (exportHistogram) {
      cv::Mat grayImage;
      if (processedImage.channels() == 3) {
        grayImage = convertToGrayscale(processedImage);
      } else {
        grayImage = processedImage.clone();
      }
      cv::Mat histogram = calculateHistogram(grayImage);
      exportHistogramToCSV(histogram, outputFileName);
      std::cout << "Histogram exported to: " << outputFileName << std::endl;
    }

    if (showImages) {
      displayImages(originalImage, processedImage);
    }

  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

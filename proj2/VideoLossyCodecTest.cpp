#include "VideoLossyCodec.h"
#include <cstdlib>
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
    return INFINITY; // No difference between frames
  }

  double psnr = 10.0 * log10((255 * 255) / mse);
  return psnr;
}

void readVideoFrames(const string &videoPath, vector<cv::Mat> &frames,
                     int &width, int &height, int &fps) {
  cv::VideoCapture video(videoPath, cv::CAP_FFMPEG);
  if (!video.isOpened()) {
    throw runtime_error("Failed to open input video file.");
  }

  width = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
  height = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));
  fps = static_cast<int>(video.get(cv::CAP_PROP_FPS));

  cv::Mat frame;
  while (video.read(frame)) {
    frames.push_back(frame.clone());
  }
  video.release();

  if (frames.empty()) {
    throw runtime_error("No frames read from input video.");
  }
}

int main(int argc, char *argv[]) {
  if (argc != 6) {
    cerr << "Usage: " << argv[0]
         << " <input_video> <encoded_file> <decoded_video> <golomb_m> "
            "<quantization_level>\n";
    return 1;
  }

  string inputVideo = argv[1];
  string encodedFile = argv[2];
  string decodedVideo = argv[3];
  uint32_t golombM = stoi(argv[4]);
  uint32_t quantLevel = stoi(argv[5]);

  try {
    vector<cv::Mat> originalFrames;
    int width, height, fps;
    readVideoFrames(inputVideo, originalFrames, width, height, fps);

    VideoLossyCodec codec(golombM);
    codec.encode(originalFrames, encodedFile, golombM, quantLevel);

    vector<cv::Mat> decodedFrames = codec.decode(encodedFile);

    if (originalFrames.size() != decodedFrames.size()) {
      cerr << "Mismatch in frame count between original and decoded video."
           << endl;
      return 1;
    }

    double totalPSNR = 0.0;
    for (size_t i = 0; i < originalFrames.size(); ++i) {
      double psnr = calculatePSNR(originalFrames[i], decodedFrames[i]);
      totalPSNR += psnr;
    }
    double averagePSNR = totalPSNR / originalFrames.size();

    ifstream inputFile(inputVideo, ios::binary | ios::ate);
    ifstream encodedFileStream(encodedFile, ios::binary | ios::ate);

    size_t originalSize = inputFile.tellg();
    size_t encodedSize = encodedFileStream.tellg();

    double compressionRate =
        (1.0 - (double(encodedSize) / originalSize)) * 100.0;

    cout << originalSize << endl;
    cout << encodedSize << endl;
    cout << compressionRate << endl;
    cout << averagePSNR << endl;

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}

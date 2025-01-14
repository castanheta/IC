#include "VideoCodec.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdexcept>

using namespace std;
namespace fs = std::filesystem;

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

void writeDecodedFramesToImages(const vector<cv::Mat> &frames,
                                const string &outputFolder) {
  if (frames.empty()) {
    throw runtime_error("No frames to write to output folder.");
  }

  if (!fs::exists(outputFolder)) {
    fs::create_directories(outputFolder);
  }

  for (size_t i = 0; i < frames.size(); ++i) {
    string fileName = outputFolder + "/frame_" + to_string(i) + ".png";
    if (!cv::imwrite(fileName, frames[i])) {
      throw runtime_error("Failed to write frame to " + fileName);
    }
  }
}

void createVideoFromImages(const string &outputFolder,
                           const string &decodedVideoPath, int width,
                           int height, int fps) {
  // Define the frame rate as the exact fraction 30000/1001 (29.97 fps)
  string frameRate = "30000/1001";

  // SAR and DAR values for the input video
  int sar_width = 128;
  int sar_height = 117;

  // Calculate the correct DAR based on SAR and resolution
  int dar_width = sar_width * width;
  int dar_height = sar_height * height;

  string command =
      "ffmpeg -y -framerate " +
      frameRate + // Set the exact frame rate (30000/1001)
      " -i " + outputFolder + "/frame_%d.png" + // Input frames
      " -s " + to_string(width) + "x" +
      to_string(height) +   // Set resolution to 352x288
      " -pix_fmt yuv420p" + // Use 4:2:0 chroma subsampling
      " -aspect " + to_string(dar_width) + ":" +
      to_string(dar_height) + // Set DAR correctly
      " -sar " + to_string(sar_width) + ":" +
      to_string(sar_height) + // Set SAR correctly
      " -colorspace bt709" +  // Set color space to Rec.709 (standard for video)
      " -color_range tv" +    // Set color range to limited (16-235)
      " -strict -1 " +        // Allow legacy formats
      decodedVideoPath +      // Output path
      " > /dev/null 2>&1";    // Redirect stdout and stderr to null

  // Execute the command
  if (system(command.c_str()) != 0) {
    throw runtime_error("Failed to create decoded video using ffmpeg.");
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
  string tmpFolder = "tmp_decoded_frames";

  try {
    vector<cv::Mat> originalFrames;
    int width, height, fps;
    readVideoFrames(inputVideo, originalFrames, width, height, fps);

    VideoCodec codec(golombM);
    codec.encode(originalFrames, encodedFile, golombM, quantLevel);

    vector<cv::Mat> decodedFrames = codec.decode(encodedFile);

    if (originalFrames.size() != decodedFrames.size()) {
      cerr << "Mismatch in frame count between original and decoded video."
           << endl;
      return 1;
    }

    writeDecodedFramesToImages(decodedFrames, tmpFolder);
    createVideoFromImages(tmpFolder, decodedVideo, width, height, fps);

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

    fs::remove_all(tmpFolder);

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}

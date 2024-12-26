#include "VideoCodecIntra.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <vector>

using namespace std;

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

void writeVideoFrames(const string &videoPath, const vector<cv::Mat> &frames,
                      int width, int height, int fps) {
  if (frames.empty()) {
    throw runtime_error("No frames to write to output video.");
  }

  cv::VideoWriter writer(videoPath,
                         cv::VideoWriter::fourcc('F', 'F', 'V', '1'), fps,
                         cv::Size(width, height), true);

  if (!writer.isOpened()) {
    throw runtime_error("Failed to open output video file.");
  }

  for (const auto &frame : frames) {
    writer.write(frame);
  }

  writer.release();
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cerr << "Usage: " << argv[0]
         << " <input_video> <encoded_file> <decoded_video> <golomb_m>\n";
    return 1;
  }

  string inputVideo = argv[1];
  string encodedFile = argv[2];
  string decodedVideo = argv[3];
  uint32_t golombM = stoi(argv[4]);

  try {
    // Read input video frames
    vector<cv::Mat> originalFrames;
    int width, height, fps;
    readVideoFrames(inputVideo, originalFrames, width, height, fps);
    cout << "Read " << originalFrames.size() << " frames from input video."
         << endl;

    // Encode video
    VideoCodecIntra codec(golombM);
    codec.encode(originalFrames, encodedFile, golombM);
    cout << "Encoded video to file: " << encodedFile << endl;

    // Decode video
    vector<cv::Mat> decodedFrames = codec.decode(encodedFile);
    cout << "Decoded video from file: " << encodedFile << endl;

    // Verify frame count
    if (originalFrames.size() != decodedFrames.size()) {
      cerr << "Mismatch in frame count between original and decoded video."
           << endl;
      return 1;
    }

    // Verify frame data
    for (size_t i = 0; i < originalFrames.size(); ++i) {
      cv::Mat diff = originalFrames[i] != decodedFrames[i];
      cv::Mat diffGray;
      cv::cvtColor(diff, diffGray,
                   cv::COLOR_BGR2GRAY); // Convert to grayscale if needed
      if (cv::countNonZero(diffGray) != 0) {
        cerr << "Mismatch in frame " << i << endl;
        return 1;
      }
    }
    cout << "All frames match between original and decoded video." << endl;

    // Write decoded frames to output video
    writeVideoFrames(decodedVideo, decodedFrames, width, height, fps);
    cout << "Decoded video saved to: " << decodedVideo << endl;

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}

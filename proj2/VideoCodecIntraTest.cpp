#include "VideoCodecIntra.h"
#include <cstdlib>
#include <filesystem>
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

void writeDecodedFramesToImages(const vector<cv::Mat> &frames,
                                const string &outputFolder) {
  if (frames.empty()) {
    throw runtime_error("No frames to write to output folder.");
  }

  if (!filesystem::exists(outputFolder)) {
    filesystem::create_directories(outputFolder);
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

  // Construct the ffmpeg command
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
      " -color_range mpeg" +  // Set color range to limited (16-235)
      " -strict -1 " +        // Allow legacy formats
      decodedVideoPath;       // Output path

  // Execute the command
  if (system(command.c_str()) != 0) {
    throw runtime_error("Failed to create decoded video using ffmpeg.");
  }
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

  string tempOutputFolder = "./decoded_frames";

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

    // // Verify frame data
    // for (size_t i = 0; i < originalFrames.size(); ++i) {
    //   cv::Mat diff = originalFrames[i] != decodedFrames[i];
    //   if (cv::countNonZero(diff) != 0) {
    //     cerr << "Mismatch in frame " << i << endl;
    //     return 1;
    //   }
    // }
    // cout << "All frames match between original and decoded video." << endl;

    // Write decoded frames to images
    writeDecodedFramesToImages(decodedFrames, tempOutputFolder);

    // Create video from images using ffmpeg
    createVideoFromImages(tempOutputFolder, decodedVideo, width, height, fps);
    cout << "Decoded video saved to: " << decodedVideo << endl;

    // Cleanup temporary frames
    // filesystem::remove_all(tempOutputFolder);

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}

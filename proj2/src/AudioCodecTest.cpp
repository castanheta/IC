#include "AudioCodec.h"
#include <cmath>
#include <filesystem>
#include <iostream>
#include <sndfile.h>
#include <stdexcept>

using namespace std;

double calculatePSNR(const string &original, const string &decoded) {
  SF_INFO info1 = {0}, info2 = {0};
  SNDFILE *file1 = sf_open(original.c_str(), SFM_READ, &info1);
  SNDFILE *file2 = sf_open(decoded.c_str(), SFM_READ, &info2);

  if (!file1 || !file2) {
    throw runtime_error("Failed to open audio files for PSNR calculation.");
  }

  if (info1.channels != info2.channels ||
      info1.samplerate != info2.samplerate || info1.frames != info2.frames) {
    sf_close(file1);
    sf_close(file2);
    throw runtime_error(
        "Mismatch in audio file properties for PSNR calculation.");
  }

  const int BUFFER_SIZE = 1024;
  vector<short> buf1(BUFFER_SIZE), buf2(BUFFER_SIZE);
  double mse = 0.0;
  size_t totalSamples = 0;

  while (true) {
    sf_count_t count1 = sf_read_short(file1, buf1.data(), BUFFER_SIZE);
    sf_count_t count2 = sf_read_short(file2, buf2.data(), BUFFER_SIZE);

    if (count1 != count2) {
      sf_close(file1);
      sf_close(file2);
      throw runtime_error("Mismatch in sample count during PSNR calculation.");
    }

    if (count1 == 0) {
      break;
    }

    totalSamples += count1;
    for (sf_count_t i = 0; i < count1; ++i) {
      double diff = buf1[i] - buf2[i];
      mse += diff * diff;
    }
  }

  sf_close(file1);
  sf_close(file2);

  if (mse == 0) {
    return INFINITY;
  }

  mse /= totalSamples;
  double psnr = 10.0 * log10((32767.0 * 32767.0) / mse);
  return psnr;
}

int main(int argc, char *argv[]) {
  if (argc < 4 || argc > 5) {
    cerr << "Usage: " << argv[0]
         << " <input_audio> <encoded_file> <decoded_audio> <m(optional)>\n"
         << "If m is not provided, adaptive Golomb parameter will be used.\n";
    return 1;
  }

  string inputAudio = argv[1];
  string encodedFile = argv[2];
  string decodedAudio = argv[3];

  try {
    AudioCodec codec;
    if (argc == 5) {
      uint32_t m = stoi(argv[4]);
      codec = AudioCodec(true, m); // Use fixed M
    }

    codec.encode(inputAudio, encodedFile);
    codec.decode(encodedFile, decodedAudio);

    ifstream inputFile(inputAudio, ios::binary | ios::ate);
    ifstream encodedFileStream(encodedFile, ios::binary | ios::ate);

    size_t originalSize = inputFile.tellg();
    size_t encodedSize = encodedFileStream.tellg();

    double compressionRate =
        (1.0 - (double(encodedSize) / originalSize)) * 100.0;

    double psnr = calculatePSNR(inputAudio, decodedAudio);

    cout << originalSize << endl;
    cout << encodedSize << endl;
    cout << compressionRate << endl;
    cout << psnr << endl;

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}

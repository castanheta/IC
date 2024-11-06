#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>

using namespace std;

class BitStream {
private:
  ofstream outFile;
  ifstream inFile;
  uint8_t buffer = 0;
  uint8_t bitPos = 0;
  bool writingMode = false;
  bool eofReached = false;

  void flushBuffer();

public:
  BitStream(const std::string &filename, bool writeMode);
  ~BitStream();

  void writeBit(bool bit);
  bool readBit();
  void writeBits(std::uint64_t value, std::uint8_t numBits);
  uint64_t readBits(std::uint8_t numBits);
  void writeString(const std::string &str);
  string readString(std::size_t length);
  bool hasNext();
};

#endif // BITSTREAM_H

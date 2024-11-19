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
  uint8_t bitCount = 0;
  bool writingMode = false;
  bool eofReached = false;

  void flushBuffer();

public:
  BitStream(const string &filename, bool writeMode);
  ~BitStream();

  void writeBit(bool bit);
  bool readBit();

  void writeBits(uint64_t value, uint8_t numBits);
  uint64_t readBits(uint8_t numBits);

  void writeString(const string &str);
  string readString(uint8_t numChars);

  bool hasNext();
  void reset();
};

#endif // BITSTREAM_H

#include "BitStream.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

BitStream::BitStream(const string &filename, bool writeMode)
    : bitPos(0), buffer(0), eofReached(false) {
  writingMode = writeMode;
  if (writeMode) {
    outFile.open(filename, ios::binary);
    if (!outFile.is_open())
      throw runtime_error("Failed to open file for writing.");
  } else {
    inFile.open(filename, ios::binary);
    if (!inFile.is_open())
      throw runtime_error("Failed to open file for reading.");
  }
}

BitStream::~BitStream() {
  if (writingMode) {
    // Ensure any remaining bits are written
    flushBuffer();
    outFile.close();
  } else {
    inFile.close();
  }
}

void BitStream::flushBuffer() {
  if (bitPos > 0) {
    outFile.put(buffer);
    buffer = 0;
    bitPos = 0;
  }
}

void BitStream::writeBit(bool bit) {
  buffer = (buffer << 1) | (bit ? 1 : 0);
  bitPos++;
  if (bitPos == 8)
    flushBuffer();
}

bool BitStream::readBit() {
  if (bitPos == 0) {
    buffer = inFile.get();
    if (inFile.eof()) {
      eofReached = true;
      throw runtime_error("End of file reached.");
    }
    bitPos = 8;
  }
  bool bit = (buffer >> (bitPos - 1)) & 1;
  bitPos--;
  return bit;
}

bool BitStream::hasNext() {
  if (bitPos > 0) {
    return true; // There are still bits left in the buffer
  }

  // Peek the next byte to check for EOF without consuming it
  if (!eofReached) {
    char nextChar = inFile.peek();
    if (inFile.eof()) {
      eofReached = true;
    }
  }
  return !eofReached;
}

void BitStream::writeBits(uint64_t value, uint8_t numBits) {
  for (int i = numBits - 1; i >= 0; --i) {
    writeBit((value >> i) & 1);
  }
}

uint64_t BitStream::readBits(uint8_t numBits) {
  uint64_t value = 0;
  for (int i = 0; i < numBits; ++i) {
    value = (value << 1) | readBit();
  }
  return value;
}

void BitStream::writeString(const string &str) {
  for (char c : str) {
    writeBit(c == '1');
  }
}

string BitStream::readString(size_t length) {
  string result;
  result.reserve(length);
  for (size_t i = 0; i < length; ++i) {
    result.push_back(readBit() ? '1' : '0');
  }
  return result;
}

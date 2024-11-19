#include "BitStream.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

BitStream::BitStream(const string &filename, bool writeMode)
    : bitCount(0), buffer(0), eofReached(false) {
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
    flushBuffer();
    outFile.close();
  } else {
    inFile.close();
  }
}

void BitStream::flushBuffer() {
  if (bitCount > 0) {
    buffer <<= (8 - bitCount); // Pad remaining bits with zeros
    outFile.put(buffer);
    buffer = 0;
    bitCount = 0;
  }
}

void BitStream::writeBit(bool bit) {
  buffer = (buffer << 1) | (bit ? 1 : 0);
  bitCount++;
  if (bitCount == 8)
    flushBuffer();
}

bool BitStream::readBit() {
  if (bitCount == 0) {
    buffer = inFile.get();
    if (inFile.eof()) {
      eofReached = true;
      throw runtime_error("End of file reached.");
    }
    bitCount = 8;
  }
  bool bit = (buffer >> (bitCount - 1)) & 1;
  bitCount--;
  return bit;
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
    writeBits(c, 8);
  }
}

string BitStream::readString(uint8_t numChars) {
  string result;
  result.reserve(numChars * 8);
  for (size_t i = 0; i < numChars; ++i) {
    char c = readBits(8);
    result.push_back(c);
  }
  return result;
}

bool BitStream::hasNext() {
  if (bitCount > 0) {
    return true;
  }

  if (!eofReached) {
    char nextChar = inFile.peek();
    // if (nextChar == '\n') {
    //   inFile.get();  // Consume the newline
    //   return hasNext();  // Recursively call hasNext to check the next character
    // }
    if (inFile.eof()) {
      eofReached = true;
    }
  }
  return !eofReached;
}

void BitStream::reset() {
  if (writingMode) {
    throw runtime_error("Cannot reset a BitStream in write mode.");
  }
  inFile.clear();  // Clear EOF and other state flags
  inFile.seekg(0); // Seek to the beginning of the file
  bitCount = 0;    // Reset the bit buffer state
  buffer = 0;
  eofReached = false; // Reset EOF tracking
}

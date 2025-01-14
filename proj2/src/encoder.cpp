#include "BitStream.h"
#include <iostream>
#include <string>

using namespace std;

void encode(const string &inputFile, const string &outputFile) {
  BitStream inputBitStream(inputFile, false);
  BitStream outputBitStream(outputFile, true);

  // Write the bit count as a header (32 bits)
  uint64_t totalBits = 0;
  while (inputBitStream.hasNext()) {
    inputBitStream.readString(1);
    totalBits++;
  }
  outputBitStream.writeBits(totalBits, 32);

  inputBitStream.reset();
  while (inputBitStream.hasNext()) {
    string bitString = inputBitStream.readString(1);
    bool bit = (bitString == "1");
    outputBitStream.writeBit(bit);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " <input_text_file> <output_binary_file>\n";
    return 1;
  }

  string inputFile = argv[1];
  string outputFile = argv[2];

  try {
    encode(inputFile, outputFile);
    cout << "Encoding complete. Output saved to " << outputFile << ".\n";
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

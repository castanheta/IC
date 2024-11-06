#include "BitStream.h"
#include <iostream>
#include <string>

using namespace std;

void encode(const string &inputFile, const string &outputFile) {
  BitStream inputBitStream(inputFile, false);
  BitStream outputBitStream(outputFile, true);

  int bitCount = 0;

  while (inputBitStream.hasNext()) {
    bool bit = inputBitStream.readBit();
    outputBitStream.writeBit(bit);
    bitCount++;
  }

  while (bitCount % 8 != 0) {
    outputBitStream.writeBit(0);
    bitCount++;
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

#include "BitStream.h"
#include <iostream>

using namespace std;

void decode(const string &inputFile, const string &outputFile) {
  BitStream inputBitStream(inputFile, false);
  BitStream outputBitStream(outputFile, true);

  while (inputBitStream.hasNext()) {
    string bitString = inputBitStream.readString(8);
    outputBitStream.writeString(bitString);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " <input_binary_file> <output_text_file>\n";
    return 1;
  }

  string inputFile = argv[1];
  string outputFile = argv[2];

  try {
    decode(inputFile, outputFile);
    cout << "Decoding complete. Output saved to " << outputFile << ".\n";
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

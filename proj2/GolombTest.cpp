#include "BitStream.h"
#include "GolombCoding.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void readIntegersFromFile(const string &filename, vector<int> &integers) {
  ifstream inputFile(filename);
  if (!inputFile.is_open()) {
    throw runtime_error("Failed to open input file.");
  }

  int value;
  while (inputFile >> value) {
    integers.push_back(value);
  }
  inputFile.close();
}

// Writes integers to a binary file using Golomb encoding
void writeIntegersToBinary(const vector<int> &integers, const string &filename,
                           GolombCoding &golomb) {
  BitStream bitStream(filename, true);

  // Write the number of integers as the header (32 bits)
  bitStream.writeBits(integers.size(), 32);

  for (int value : integers) {
    golomb.encodeInteger(value, bitStream);
  }
}

// Reads integers from a binary file using Golomb decoding
void readIntegersFromBinary(const string &filename,
                            vector<int> &decodedIntegers,
                            GolombCoding &golomb) {
  BitStream bitStream(filename, false);

  // Read the number of integers from the header (32 bits)
  uint32_t count = bitStream.readBits(32);

  // Decode exactly `count` integers
  for (uint32_t i = 0; i < count; ++i) {
    decodedIntegers.push_back(golomb.decodeInteger(bitStream));
  }
}

// Verifies that the original and decoded integers match
void verifyResults(const vector<int> &original, const vector<int> &decoded) {
  if (original.size() != decoded.size()) {
    cout << "Mismatch in size between original and decoded data.\n";
    return;
  }

  bool allMatch = true;
  for (size_t i = 0; i < original.size(); ++i) {
    if (original[i] != decoded[i]) {
      allMatch = false;
      cout << "Mismatch at index " << i << ": original = " << original[i]
           << ", decoded = " << decoded[i] << endl;
    }
  }

  if (allMatch) {
    cout << "All integers successfully encoded and decoded!\n";
  } else {
    cout << "Encoding/decoding test failed for some integers.\n";
  }
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cerr << "Usage: " << argv[0]
         << " <input_file> <encodedFile> <decodedFile> <m(golomb parameter)>\n";
    return 1;
  }

  string inputFile = argv[1];
  string encodedFile = argv[2];
  string decodedFile = argv[3];
  uint32_t m = stoi(argv[4]);

  try {
    // Configure Golomb coding with the ZIGZAG mapping
    GolombCoding golomb(m, GolombCoding::ZIGZAG);

    // Read integers from the input text file
    vector<int> originalIntegers;
    readIntegersFromFile(inputFile, originalIntegers);

    // Encode and write integers to the binary file
    writeIntegersToBinary(originalIntegers, encodedFile, golomb);

    // Decode integers from the binary file
    vector<int> decodedIntegers;
    readIntegersFromBinary(encodedFile, decodedIntegers, golomb);

    // Verify that the original and decoded integers match
    verifyResults(originalIntegers, decodedIntegers);
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}

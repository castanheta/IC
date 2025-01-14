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

void writeIntegersToBinary(const vector<int> &integers, const string &filename,
                           GolombCoding &golomb) {
  BitStream bitStream(filename, true);

  bitStream.writeBits(integers.size(), 32);

  for (int value : integers) {
    golomb.encodeInteger(value, bitStream);
  }
}

void readIntegersFromBinary(const string &filename,
                            vector<int> &decodedIntegers,
                            GolombCoding &golomb) {
  BitStream bitStream(filename, false);

  uint32_t count = bitStream.readBits(32);

  for (uint32_t i = 0; i < count; ++i) {
    decodedIntegers.push_back(golomb.decodeInteger(bitStream));
  }
}

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

void writeIntegersToFile(const string &filename, const vector<int> &integers) {
  ofstream outputFile(filename);
  if (!outputFile.is_open()) {
    throw runtime_error("Failed to open output file.");
  }

  for (int value : integers) {
    outputFile << value << endl;
  }
  outputFile.close();
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
    GolombCoding golomb(m, GolombCoding::ZIGZAG);

    vector<int> originalIntegers;
    readIntegersFromFile(inputFile, originalIntegers);

    writeIntegersToBinary(originalIntegers, encodedFile, golomb);

    vector<int> decodedIntegers;
    readIntegersFromBinary(encodedFile, decodedIntegers, golomb);

    writeIntegersToFile(decodedFile, decodedIntegers);

    verifyResults(originalIntegers, decodedIntegers);
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}

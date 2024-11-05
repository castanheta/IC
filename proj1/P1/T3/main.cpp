#include <iostream>
#include <fstream>
#include <map>
#include <cctype> 

void countCharFrequencies(const std::string& input, std::map<char, int>& freqMap) {
    for (char c : input) {
        if(!std::isspace(c) && !std::ispunct(c)) {
            freqMap[std::tolower(c)]++;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./text_processor <input_text_path>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]); // Open the input file
    if (!file) {
        std::cerr << "Error: Unable to open input file!" << std::endl;
        return 1;
    }

    std::map<char, int> freqMap;
    std::string line;

    // Count character frequencies
    while (getline(file, line)) {
        countCharFrequencies(line, freqMap);
    }
    file.close();

    // Open CSV file to write output
    std::ofstream outputFile("char_frequencies.csv");
    if (!outputFile) {
        std::cerr << "Error: Unable to create output file!" << std::endl;
        return 1;
    }

    // Write CSV headers
    outputFile << "Character,Frequency\n";

    // Write character frequencies to CSV
    for (const auto& pair : freqMap) {
        outputFile << pair.first << "," << pair.second << "\n";
    }

    outputFile.close();
    std::cout << "Character frequencies have been written to char_frequencies.csv" << std::endl;
    
    return 0;
}

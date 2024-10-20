#include <iostream>
#include <fstream>
#include <map>
#include <sstream> // for stringstream
#include <algorithm> // for remove_if

void countWordFrequencies(const std::string& line, std::map<std::string, int>& wordFreq) {
    std::stringstream ss(line);
    std::string word;
    
    while (ss >> word) {
        // Debug: Print the raw word
        std::cout << "Raw word: " << word << std::endl;

        // Remove punctuation and convert to lowercase
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);

        // Debug: Print the processed word
        std::cout << "Processed word: " << word << std::endl;

        wordFreq[word]++;
    }
}


int main(int argc, char **argv) {
    std::ifstream file(argv[1]); // Open the file
    if (argc != 2) {
        std::cerr << "Usage: ./text_processor <input_text_path>" << std::endl;
        return 1;
    }
    else if (!file) {
        std::cerr << "Error: Unable to open file!" << std::endl;
    return 1;
    }

    std::map<std::string, int> wordFreq;
    std::string line;
    
    // Read the file line by line
    while (getline(file, line)) {
        countWordFrequencies(line, wordFreq);
    }

    // Close the input file
    file.close();

    // Open a CSV file for writing
    std::ofstream csvFile("word_frequencies.csv");
    if (!csvFile) {
        std::cerr << "Error: Unable to open CSV file for writing!" << std::endl;
        return 1;
    }

    // Write the CSV header
    csvFile << "Word,Frequency\n";

    // Write word-frequency pairs to the CSV file
    for (const auto& pair : wordFreq) {
        csvFile << pair.first << "," << pair.second << "\n";
    }

    // Close the CSV file
    csvFile.close();

    std::cout << "Word frequencies have been exported to word_frequencies.csv" << std::endl;

    return 0;
}

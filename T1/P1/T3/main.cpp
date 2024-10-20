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
    std::ifstream file(argv[1]); // Open the file
    if (argc != 2) {
        std::cerr << "Usage: ./text_processor <input_text_path>" << std::endl;
        return 1;
    }
    else if (!file) {
        std::cerr << "Error: Unable to open file!" << std::endl;
    return 1;
    }

    std::map<char, int> freqMap;
    std::string line;

    while(getline(file, line)) {
        countCharFrequencies(line, freqMap);
    }

    for (const auto& pair : freqMap) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    file.close();
    return 0;
}

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

int main() {
    std::ifstream file("sample1.txt");
    if (!file) {
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

#include <iostream>
#include <fstream>
#include <string>
#include <cctype>  // for tolower() and ispunct()

std::string processText(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (!std::ispunct(c)) { // Remove punctuation
            result += std::tolower(c); // Convert to lowercase
        }
    }
    return result;
}

int main() {
    std::ifstream file("Sample1.txt");
    if (!file) {
        std::cerr << "Error: Unable to open file!" << std::endl;
        return 1;
    }

    std::string line;
    while (getline(file, line)) {
        std::string processed = processText(line);
        std::cout << processed << std::endl;
    }
    
    file.close();
    return 0;
}

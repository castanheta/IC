#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char **argv) {
    std::ifstream file(argv[1]); // Open the file
    if (argc != 2) {
        std::cerr << "Usage: ./text_processor <input_text_path>" << std::endl;
    return 1;
  }
    
    std::string line;
    while (getline(file, line)) {
        std::cout << line << std::endl; // Print each line
    }
    
    file.close(); // Close the file
    return 0;
}

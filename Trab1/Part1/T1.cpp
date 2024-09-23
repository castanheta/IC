#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // For std::transform
#include <cctype>    // For std::tolower
using namespace std;

string convertToLowercase(const string& str)
{
    string result = "";

    for (char ch : str) {
        // Convert each character to lowercase using tolower
        result += tolower(ch);
    }

    return result;
}

string removePunctuations(string& s) {
    string result = "";
    for (char c : s) {
        if (!ispunct(c)) { // If c is not a punctuation character
            result += c;
        }
    }
    s = result;
    return s;
}

int main() {
    ifstream myfile; 
    myfile.open("SimpleSample.txt");
    string myline;

    if ( myfile.is_open() ) {
        while ( myfile) {
            getline(myfile, myline);
        }
    }

    string lowercasefilestr = convertToLowercase(myline);
    string nopunctfilestr = removePunctuations(lowercasefilestr);

    cout << myline << endl;

    return 0;
}





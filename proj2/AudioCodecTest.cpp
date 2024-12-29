#include <sndfile.h>

#include <iostream>
#include <stdexcept>

#include "AudioCodec.h"

using namespace std;

bool verifyAudio(const string& original, const string& decoded) {
    SF_INFO info1 = {0}, info2 = {0};
    SNDFILE* file1 = sf_open(original.c_str(), SFM_READ, &info1);
    SNDFILE* file2 = sf_open(decoded.c_str(), SFM_READ, &info2);

    if (!file1 || !file2) {
        return false;
    }

    // Check if audio properties match
    if (info1.channels != info2.channels ||
        info1.samplerate != info2.samplerate || info1.frames != info2.frames) {
        sf_close(file1);
        sf_close(file2);
        return false;
    }

    // Compare audio data
    const int BUFFER_SIZE = 1024;
    vector<short> buf1(BUFFER_SIZE), buf2(BUFFER_SIZE);

    while (true) {
        sf_count_t count1 = sf_read_short(file1, buf1.data(), BUFFER_SIZE);
        sf_count_t count2 = sf_read_short(file2, buf2.data(), BUFFER_SIZE);

        if (count1 != count2) {
            sf_close(file1);
            sf_close(file2);
            return false;
        }

        if (count1 == 0) break;

        for (sf_count_t i = 0; i < count1; i++) {
            if (buf1[i] != buf2[i]) {
                sf_close(file1);
                sf_close(file2);
                return false;
            }
        }
    }

    sf_close(file1);
    sf_close(file2);
    return true;
}

void displayChannelStats(const string& filename) {
    SF_INFO sfInfo = {0};
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfInfo);
    if (!file) return;

    vector<short> buffer(1024);
    sf_count_t count = sf_read_short(file, buffer.data(), 1024);

    cout << "File: " << filename << endl;
    cout << "First few samples:" << endl;
    for (int i = 0; i < min(10, static_cast<int>(count / 2)); i++) {
        cout << "Frame " << i << ": L=" << buffer[i * 2]
             << " R=" << buffer[i * 2 + 1] << endl;
    }

    sf_close(file);
}

int main(int argc, char* argv[]) {
    if (argc < 4 || argc > 5) {
        cerr << "Usage: " << argv[0]
             << " <input_audio> <encoded_file> <decoded_audio> <m(optional)>\n"
             << "If m is not provided, adaptive Golomb parameter will be "
                "used.\n";
        return 1;
    }

    string inputAudio = argv[1];
    string encodedFile = argv[2];
    string decodedAudio = argv[3];

    try {
        cout << "Loading input audio...\n";
        cout << "\nInput file analysis:" << endl;
        displayChannelStats(inputAudio);

        AudioCodec codec;
        if (argc == 5) {
            uint32_t m = stoi(argv[4]);
            codec = AudioCodec(true, m);  // Use fixed M
            cout << "Using fixed Golomb parameter M = " << m << "\n";
        } else {
            cout << "Using adaptive Golomb parameter\n";
        }

        cout << "Encoding audio...\n";
        codec.encode(inputAudio, encodedFile);

        cout << "Decoding audio...\n";
        codec.decode(encodedFile, decodedAudio);

        cout << "Verifying results...\n";
        if (verifyAudio(inputAudio, decodedAudio)) {
            cout << "Audio successfully encoded and decoded!\n";
        } else {
            cout << "Mismatch detected in audio data!\n";
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
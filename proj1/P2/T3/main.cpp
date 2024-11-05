#include <iostream>
#include <sndfile.hh>
#include <vector>
#include <fstream>
#include "wav_hist.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536;  // Buffer for reading frames

int main(int argc, char *argv[]) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " <input file> <output file name> <channel> <bin size>\n";
        return 1;
    }

    SndfileHandle sndFile{ argv[1] };
    if (sndFile.error()) {
        cerr << "Error: invalid input file\n";
        return 1;
    }

    if ((sndFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: file is not in WAV format\n";
        return 1;
    }

    if ((sndFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: file is not in PCM_16 format\n";
        return 1;
    }

    string eqChannel { argv[3] };
    int channel{};
    if (eqChannel != "mid" && eqChannel != "side") {
        try
        {
            channel = stoi(eqChannel);
        }
        catch(const exception& e)
        {
            cerr << e.what() << '\n';
            return 1;
        }

        if (channel >= sndFile.channels()) {
                cerr << "Error: invalid channel requested\n";
                return 1;
            }
    }

    size_t binSize { };
    try
    {
        binSize = stoi(argv[4]);
        if ((binSize & (binSize - 1)) != 0) {
            cerr << "Error: bin size must be a power of 2\n";
            return 1;
        }
    }
    catch(const exception& e)
    {
        cerr << e.what() << '\n';
        return 1;
    }

    size_t nFrames;
    vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    WAVHist hist{ sndFile, binSize };
    while ((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
        samples.resize(nFrames * sndFile.channels());
        hist.update(samples);
    }

    string outputFilename = string(argv[2]) + ".csv";
    ofstream out(outputFilename);

    if (eqChannel == "mid") {
        hist.dumpMid(out);
    } else if (eqChannel == "side") {
        hist.dumpSide(out);
    } else {
        hist.dump(channel, out);
    }
    cout << "Histogram data written to " << outputFilename << "\n";
    out.close();

    return 0;
}
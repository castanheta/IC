#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sndfile.hh>
#include <vector>

using namespace std;

short quantize(short sample, int quantizationBits) {
    int levels = 1 << quantizationBits;
    int maxLevel = levels / 2;
    float normalizedSample = sample / 32768.0f;
    float quantizedValue = round(normalizedSample * maxLevel) / maxLevel;

    return static_cast<short>(quantizedValue * 32767);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <input file> <output file name> <quantization bits>\n";
        return 1;
    }

    SndfileHandle sndFile{argv[1]};
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

    int quantizationBits{stoi(argv[3])};
    if (quantizationBits < 1 || quantizationBits > 16) {
        cerr << "Error: quantization bits must be between 1 and 16\n";
        return 1;
    }

    vector<short> samples(sndFile.frames() * sndFile.channels());

    sndFile.readf(samples.data(), sndFile.frames());

    string outputFilename = string(argv[2]) + ".csv";
    ofstream out(outputFilename);
    out << "Original,Quantized\n";

    for (size_t i = 0; i < samples.size(); ++i) {
        short quantizedSample = quantize(samples[i], quantizationBits);
        out << samples[i] << "," << quantizedSample << "\n";
        samples[i] = quantizedSample;
    }
    out.close();
    cout << "Quantization data written to " << outputFilename << "\n";

    SndfileHandle outputWavFile{"quantized_" + string(argv[2]) + ".wav", SFM_WRITE, sndFile.format(), sndFile.channels(), sndFile.samplerate()};
    outputWavFile.writef(samples.data(), sndFile.frames());

    return 0;
}
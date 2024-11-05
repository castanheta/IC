#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sndfile.hh>
#include <vector>

using namespace std;

double calculateMSE(const vector<short> &original, const vector<short> &quantized) {
    double mse = 0;
    for (size_t i = 0; i < original.size(); ++i) {
        mse += pow(original[i] - quantized[i], 2);
    }

    return mse / original.size();
}

double calculateSNR(const vector<short> &original, const vector<short> &quantized) {
    double signalPower = 0.0, noisePower = 0.0;
    size_t N = original.size();
    for (size_t i = 0; i < N; ++i) {
        double signal = static_cast<double>(original[i]);
        double noise = static_cast<double>(original[i]) - static_cast<double>(quantized[i]);
        signalPower += signal * signal;
        noisePower += noise * noise;
    }
    return 10.0 * log10(signalPower / noisePower);
}

vector<short> quantize(const vector<short> &samples, size_t quantizationBits) {
    vector<short> quantizedSamples;
    int levels = 1 << quantizationBits;
    int maxLevel = levels / 2;

    for (short sample : samples) {
        float normalizedSample = sample / 32768.0f;
        float quantizedValue = round(normalizedSample * maxLevel) / maxLevel;
        quantizedSamples.push_back(static_cast<short>(quantizedValue * 32767));
    }

    return quantizedSamples;
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

    vector<short> quantizedSamples = quantize(samples, quantizationBits);

    string outputFilename = string(argv[2]) + ".csv";
    ofstream out(outputFilename);

    out << "Value,Counter\n";
    for (size_t i = 0; i < quantizedSamples.size(); ++i) {
        out << quantizedSamples[i] << "," << i << "\n";
    }

    cout << calculateMSE(samples, quantizedSamples) << ", " << calculateSNR(samples, quantizedSamples) << "\n";

    return 0;
}
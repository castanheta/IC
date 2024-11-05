#include <fstream>
#include <iostream>
#include <sndfile.hh>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input file> <output file name>" << endl;
        return 1;
    }

    SndfileHandle sfhIn{ argv[1] };
    if (sfhIn.error()) {
        cerr << "Error: Invalid input file\n";
        return 1;
    }

    if ((sfhIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: file is not in WAV format\n";
        return 1;
    }

    if ((sfhIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: file is not in PCM_16 format\n";
        return 1;
    }

    size_t numSamples = sfhIn.frames() * sfhIn.channels();
    vector<short> samples(numSamples);

    sfhIn.readf(samples.data(), sfhIn.frames());

    vector<double> time(numSamples);
    for (size_t i = 0; i < numSamples; i++) {
        time[i] = i / sfhIn.samplerate();
    }

    string outputFilename = string(argv[2]) + ".csv";
    ofstream out(outputFilename);
    out << "Time,Amplitude" << endl;
    for (size_t i = 0; i < numSamples; i++) {
        out << time[i] << "," << samples[i] << endl;
    }
    out.close();

    cout << "Audio data written to " << outputFilename << "\n";
    return 0;
}
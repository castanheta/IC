#include <iostream>
#include <vector>
#include <sndfile.hh>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input file>\n";
        return 1;
    }

    SndfileHandle sfhIn { argv[1] };
    if (sfhIn.error()) {
        cerr << "Error: Invalid input file\n";
        return 1;
    }

    if((sfhIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

    if((sfhIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file is not in PCM_16 format\n";
		return 1;
	}

    int samplerate = sfhIn.samplerate();
    int channels = sfhIn.channels();
    int frames = sfhIn.frames();
    double duration = frames / samplerate;

    cout << "Input file has:\n";
    cout << '\t' << samplerate << " Hz\n";
    cout << '\t' << channels << " channels\n";
    cout << '\t' << frames << " frames\n";
    cout << '\t' << duration << " seconds\n";

    return 0;
}
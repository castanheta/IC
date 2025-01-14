#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include <sndfile.hh>
#include <string>
#include <vector>

#include "BitStream.h"
#include "GolombCoding.h"

class AudioCodec {
   private:
    static constexpr int WINDOW_SIZE = 4;
    static constexpr int ADAPT_INTERVAL = 1024;

    bool useFixedM;
    uint32_t fixedM;

    struct AudioHeader {
        bool isStereo;
        int sampleRate;
        int bitsPerSample;
        uint32_t numSamples;
        bool useFixedM;
        uint32_t fixedM;

        void write(BitStream& bs);
        void read(BitStream& bs);
    };

    uint32_t calculateOptimalM(const std::vector<int>& residuals, int start,
                               int length);
    int predictSample(const std::vector<int>& samples, int currentPos);
    int predictStereoSample(const std::vector<int>& leftChannel,
                            const std::vector<int>& rightChannel,
                            int currentPos, bool isRight);

    std::vector<std::vector<int>> readAudioFile(const std::string& filename,
                                                AudioHeader& header);
    void writeAudioFile(const std::string& filename,
                        const std::vector<std::vector<int>>& channels,
                        const AudioHeader& header);

    int predictNextSample(const std::vector<std::vector<int>>& channels,
                          int channel, size_t pos);

    int calculatePrediction(const std::vector<std::vector<int>>& channels,
                            int channelIdx, size_t pos);

   public:
    AudioCodec(bool useFixedM = false, uint32_t fixedM = 1)
        : useFixedM(useFixedM), fixedM(fixedM) {}
    void encode(const std::string& inputFile, const std::string& outputFile);
    void decode(const std::string& inputFile, const std::string& outputFile);
};

#endif  // AUDIO_CODEC_H

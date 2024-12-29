#include "AudioCodec.h"

#include <cmath>
#include <iostream>
#include <sndfile.hh>
#include <stdexcept>

void AudioCodec::AudioHeader::write(BitStream& bs) {
    bs.writeBit(isStereo);
    bs.writeBits(sampleRate, 32);
    bs.writeBits(bitsPerSample, 8);
    bs.writeBits(numSamples, 32);
    bs.writeBit(useFixedM);
    if (useFixedM) {
        bs.writeBits(fixedM, 16);
    }
}

void AudioCodec::AudioHeader::read(BitStream& bs) {
    isStereo = bs.readBit();
    sampleRate = bs.readBits(32);
    bitsPerSample = bs.readBits(8);
    numSamples = bs.readBits(32);
    useFixedM = bs.readBit();
    if (useFixedM) {
        fixedM = bs.readBits(16);
    }
}

uint32_t AudioCodec::calculateOptimalM(const std::vector<int>& residuals,
                                       int start, int length) {
    double mean = 0;
    for (int i = start; i < start + length; i++) {
        mean += std::abs(residuals[i]);
    }
    mean /= length;
    return 1 << (static_cast<int>(std::log2(mean)));
}

int AudioCodec::predictSample(const std::vector<int>& samples, int currentPos) {
    if (currentPos <= 0) return 0;

    // Use just the previous sample for more stable prediction
    return samples[currentPos - 1];
}

int AudioCodec::predictStereoSample(const std::vector<int>& leftChannel,
                                    const std::vector<int>& rightChannel,
                                    int currentPos, bool isRight) {
    if (currentPos < 0) return 0;

    // Get temporal prediction (from same channel)
    int temporalPred =
        predictSample(isRight ? rightChannel : leftChannel, currentPos);

    // Only do cross-channel prediction if we have valid samples
    if (currentPos >= 0) {
        if (isRight) {
            // For right channel, use current left sample
            return (temporalPred + leftChannel[currentPos]) / 2;
        } else {
            // For left channel, just use temporal prediction
            return temporalPred;
        }
    }

    return temporalPred;
}

std::vector<std::vector<int>> AudioCodec::readAudioFile(
    const std::string& filename, AudioHeader& header) {
    SF_INFO sfInfo;
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfInfo);
    if (!file) {
        throw std::runtime_error("Failed to open audio file: " + filename);
    }

    header.isStereo = sfInfo.channels == 2;
    header.sampleRate = sfInfo.samplerate;
    header.bitsPerSample = 16;  // Assuming 16-bit PCM
    header.numSamples = sfInfo.frames;

    std::vector<std::vector<int>> channels(sfInfo.channels);
    for (auto& channel : channels) {
        channel.resize(sfInfo.frames);
    }

    std::vector<short> buffer(sfInfo.frames * sfInfo.channels);
    sf_read_short(file, buffer.data(), buffer.size());

    for (size_t i = 0; i < sfInfo.frames; i++) {
        for (int ch = 0; ch < sfInfo.channels; ch++) {
            channels[ch][i] = buffer[i * sfInfo.channels + ch];
        }
    }

    sf_close(file);
    return channels;
}

void AudioCodec::writeAudioFile(const std::string& filename,
                                const std::vector<std::vector<int>>& channels,
                                const AudioHeader& header) {
    SF_INFO sfInfo = {0};
    sfInfo.channels = header.isStereo ? 2 : 1;
    sfInfo.samplerate = header.sampleRate;
    sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    SNDFILE* file = sf_open(filename.c_str(), SFM_WRITE, &sfInfo);
    if (!file) {
        throw std::runtime_error("Failed to create audio file: " + filename);
    }

    std::vector<short> buffer(header.numSamples * sfInfo.channels);
    for (size_t i = 0; i < header.numSamples; i++) {
        for (int ch = 0; ch < sfInfo.channels; ch++) {
            buffer[i * sfInfo.channels + ch] = channels[ch][i];
        }
    }

    sf_write_short(file, buffer.data(), buffer.size());
    sf_close(file);
}

void AudioCodec::encode(const std::string& inputFile,
                        const std::string& outputFile) {
    AudioHeader header;
    std::vector<std::vector<int>> channels = readAudioFile(inputFile, header);

    header.useFixedM = useFixedM;
    header.fixedM = fixedM;

    BitStream bs(outputFile, true);
    header.write(bs);

    // Process each channel
    for (int ch = 0; ch < channels.size(); ch++) {
        // Write channel marker for debugging
        std::cout << "Encoding channel " << ch << std::endl;

        // First calculate all residuals for this channel
        std::vector<int> residuals(header.numSamples);
        for (size_t i = 0; i < header.numSamples; i++) {
            int predicted;
            if (header.isStereo && ch == 1) {  // Right channel
                predicted = (i > 0)
                                ? channels[1][i - 1]
                                : 0;  // Use previous sample from right channel
                if (i < header.numSamples) {
                    predicted = (predicted + channels[0][i]) /
                                2;  // Mix with current left sample
                }
            } else {  // Left channel or mono
                predicted = (i > 0) ? channels[ch][i - 1]
                                    : 0;  // Just use previous sample
            }

            residuals[i] = channels[ch][i] - predicted;

            if (i < 5) {
                std::cout << "Sample " << i << ": original=" << channels[ch][i]
                          << " predicted=" << predicted
                          << " residual=" << residuals[i] << std::endl;
            }
        }

        // Then encode the residuals in blocks
        for (size_t i = 0; i < header.numSamples; i += ADAPT_INTERVAL) {
            uint32_t m;
            if (useFixedM) {
                m = fixedM;
                if (i == 0) {
                    bs.writeBits(m, 16);
                }
            } else {
                int length = std::min(ADAPT_INTERVAL,
                                      static_cast<int>(header.numSamples - i));
                m = calculateOptimalM(residuals, i, length);
                bs.writeBits(m, 16);
            }

            GolombCoding gc(m);
            int length = std::min(ADAPT_INTERVAL,
                                  static_cast<int>(header.numSamples - i));
            for (int j = 0; j < length; j++) {
                gc.encodeInteger(residuals[i + j], bs);
            }
        }
    }
}

int AudioCodec::predictNextSample(const std::vector<std::vector<int>>& channels,
                                  int channel, size_t pos) {
    if (pos == 0) return 0;

    if (channels.size() == 2 && channel == 1) {  // Right channel of stereo
        int prevRight = channels[1][pos - 1];
        int currLeft = channels[0][pos];
        return (prevRight + currLeft) / 2;
    } else {  // Left channel or mono
        return channels[channel][pos - 1];
    }
}

void AudioCodec::decode(const std::string& inputFile,
                        const std::string& outputFile) {
    BitStream bs(inputFile, false);
    AudioHeader header;
    header.read(bs);

    std::cout << "Decoded header - Channels: " << (header.isStereo ? 2 : 1)
              << " Samples: " << header.numSamples << std::endl;

    std::vector<std::vector<int>> channels(header.isStereo ? 2 : 1);
    for (auto& channel : channels) {
        channel.resize(header.numSamples, 0);  // Initialize with zeros
    }

    uint32_t fixedM = 0;
    if (header.useFixedM) {
        fixedM = bs.readBits(16);
        std::cout << "Read fixed M value: " << fixedM << std::endl;
    }

    try {
        for (int ch = 0; ch < channels.size(); ch++) {
            std::cout << "Decoding channel " << ch << std::endl;

            for (size_t i = 0; i < header.numSamples; i += ADAPT_INTERVAL) {
                uint32_t m = header.useFixedM ? fixedM : bs.readBits(16);
                if (i == 0) {
                    std::cout << "Using M value: " << m
                              << " for block at position " << i << std::endl;
                }

                GolombCoding gc(m);
                int length = std::min(ADAPT_INTERVAL,
                                      static_cast<int>(header.numSamples - i));

                for (int j = 0; j < length; j++) {
                    int residual = gc.decodeInteger(bs);
                    int predicted = predictNextSample(channels, ch, i + j);
                    channels[ch][i + j] = predicted + residual;

                    if (i + j < 5) {
                        std::cout
                            << "Sample " << (i + j) << ": residual=" << residual
                            << " predicted=" << predicted
                            << " final=" << channels[ch][i + j] << std::endl;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during decoding: " << e.what() << std::endl;
        throw;
    }

    writeAudioFile(outputFile, channels, header);
}
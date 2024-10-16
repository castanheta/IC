#ifndef WAVHIST_H
#define WAVHIST_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sndfile.hh>
#include <vector>

class WAVHist {
   private:
    std::vector<std::map<short, size_t>> counts;
    std::map<short, size_t> midCounts;
    std::map<short, size_t> sideCounts;
    size_t binSize;

    short updateBin(short value) const { return (value / binSize) * binSize; }

   public:
    WAVHist(const SndfileHandle& sfh, size_t binSize = 2) : binSize(binSize) {
        counts.resize(sfh.channels());
    }

    void update(const std::vector<short>& samples) {
        size_t numChannels = counts.size();
        size_t nSamples = samples.size() / numChannels;

        for (size_t i = 0; i < nSamples; i++) {
            for (size_t ch = 0; ch < numChannels; ch++) {
                short sample = samples[i * numChannels + ch];
                short binnedSample = updateBin(sample);
                counts[ch][binnedSample]++;
            }

            if (numChannels == 2) {
                short left = samples[i * 2];
                short right = samples[i * 2 + 1];

                short mid = (left + right) / 2;
                short side = (left - right) / 2;

                short binnedMid = updateBin(mid);
                short binnedSide = updateBin(side);

                midCounts[binnedMid]++;
                sideCounts[binnedSide]++;
            }
        }
    }

    void dump(const size_t channel, std::ofstream& out) const {
        out << "Value,Counter\n";
        for (auto [value, counter] : counts[channel])
            out << value << ',' << counter << '\n';
    }

    void dumpMid(std::ofstream& out) const {
        out << "Value,Counter\n";
        for (auto [value, counter] : midCounts)
            out << value << ',' << counter << '\n';
    }

    void dumpSide(std::ofstream& out) const {
        out << "Value,Counter\n";
        for (auto [value, counter] : sideCounts)
            out << value << ',' << counter << '\n';
    }
};

#endif

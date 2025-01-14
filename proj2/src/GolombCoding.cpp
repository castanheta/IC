#include "GolombCoding.h"
#include <cmath>
#include <stdexcept>

using namespace std;

GolombCoding::GolombCoding(uint32_t m, NegativeEncoding encoding)
    : m(m), negativeMode(encoding) {
  if (m == 0) {
    throw invalid_argument("Golomb parameter m must be greater than 0.");
  }
}

uint32_t GolombCoding::mapToNonNegative(int value) const {
  if (negativeMode == SIGN_MAGNITUDE) {
    uint32_t magnitude = static_cast<uint32_t>(abs(value));
    return (value < 0) ? (magnitude << 1) | 1 : magnitude << 1;
  } else { // ZIGZAG
    return (value >= 0) ? (value << 1) : ((-value << 1) - 1);
  }
}

int GolombCoding::mapToSigned(uint32_t value) const {
  if (negativeMode == SIGN_MAGNITUDE) {
    bool isNegative = value & 1;
    uint32_t magnitude = value >> 1;
    return isNegative ? -static_cast<int>(magnitude)
                      : static_cast<int>(magnitude);
  } else { // ZIGZAG
    return (value & 1) ? -(static_cast<int>((value + 1) >> 1))
                       : static_cast<int>(value >> 1);
  }
}

void GolombCoding::encodeInteger(int value, BitStream &bitStream) const {
  uint32_t mappedValue = mapToNonNegative(value);

  uint32_t q = mappedValue / m;
  uint32_t r = mappedValue % m;

  for (uint32_t i = 0; i < q; ++i) {
    bitStream.writeBit(1);
  }
  bitStream.writeBit(0);

  uint32_t b = static_cast<uint32_t>(ceil(log2(m)));
  uint32_t threshold = (1U << b) - m;

  if (r < threshold) {
    bitStream.writeBits(r, b - 1);
  } else {
    r += threshold;
    bitStream.writeBits(r, b);
  }
}

int GolombCoding::decodeInteger(BitStream &bitStream) const {
  uint32_t q = 0;
  while (bitStream.hasNext() && bitStream.readBit()) {
      ++q;
  }

  uint32_t b = static_cast<uint32_t>(ceil(log2(m)));
  uint32_t threshold = (1U << b) - m;

  uint32_t r = 0;
  if (bitStream.hasNext()) {
    r = bitStream.readBits(b - 1);
    if (r >= threshold && bitStream.hasNext()) {
        uint32_t extraBit = bitStream.readBit();
        r = (r << 1) | extraBit;
        r -= threshold;
    }
  } else {
    throw runtime_error("Unexpected end of stream while decoding remainder.");
  }

  uint32_t mappedValue = q * m + r;
  return mapToSigned(mappedValue);
}

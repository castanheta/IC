#ifndef GOLOMB_CODING_H
#define GOLOMB_CODING_H

#include "BitStream.h"

class GolombCoding {
public:
  enum NegativeEncoding { SIGN_MAGNITUDE, ZIGZAG };

  GolombCoding(uint32_t m, NegativeEncoding encoding = ZIGZAG);

  void encodeInteger(int value, BitStream &bitStream) const;
  int decodeInteger(BitStream &bitStream) const;

private:
  uint32_t m;
  NegativeEncoding negativeMode;

  uint32_t mapToNonNegative(int value) const;
  int mapToSigned(uint32_t value) const;
  void encodeUnsignedInteger(uint32_t value, BitStream &bitStream) const;
  uint32_t decodeUnsignedInteger(BitStream &bitStream) const;
};

#endif // GOLOMB_CODING_H

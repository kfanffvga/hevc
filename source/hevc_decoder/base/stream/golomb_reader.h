#ifndef _GOLOMB_READER_H_
#define _GOLOMB_READER_H_

#include <stdint.h>

class BitStream;

class GolombReader
{
public:
    GolombReader(BitStream* bit_stream);
    ~GolombReader();

    uint32_t ReadUnsignedValue();
    int32_t ReadSignedValue();

private:
    BitStream* bit_stream_;
};

#endif
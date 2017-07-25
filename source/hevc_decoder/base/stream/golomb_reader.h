#ifndef _GOLOMB_READER_H_
#define _GOLOMB_READER_H_

#include "hevc_decoder/base/basic_types.h"

class BitStream;

class GolombReader
{
public:
    GolombReader(BitStream* bit_stream);
    ~GolombReader();

    uint32 ReadUnsignedValue();
    int32 ReadSignedValue();

private:
    BitStream* bit_stream_;
};

#endif
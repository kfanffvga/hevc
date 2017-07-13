#ifndef _GOLOMB_READER_H_
#define _GOLOMB_READER_H_

#include "hevc_decoder/base/basic_types.h"

class BitStream;

class GolombReadr
{
public:
    GolombReadr(BitStream* bit_stream);
    ~GolombReadr();

    uint32 ReadUnsignedValue();
    int32 ReadSignedValue();

private:
    BitStream* bit_stream_;
};

#endif
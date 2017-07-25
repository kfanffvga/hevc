#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/base/stream/bit_stream.h"

GolombReader::GolombReader(BitStream* bit_stream)
    : bit_stream_(bit_stream)
{
    assert(bit_stream);
}

GolombReader::~GolombReader()
{
    
}

uint32 GolombReader::ReadUnsignedValue()
{
    int zero_bits = 0;
    while (!bit_stream_->Read<uint8>(1))
        ++zero_bits;

    return (1 << zero_bits) - 1 + bit_stream_->Read<uint32>(zero_bits);
}

int32 GolombReader::ReadSignedValue()
{
    int32 code_num = static_cast<int32>(ReadUnsignedValue());
    return (code_num & 0x01) ? (-((code_num + 1) >> 1)) : (code_num >> 1);
}

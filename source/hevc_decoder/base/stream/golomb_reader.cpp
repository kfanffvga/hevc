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

uint32_t GolombReader::ReadUnsignedValue()
{
    int zero_bits = 0;
    while (!bit_stream_->Read<uint8_t>(1))
        ++zero_bits;

    return (1 << zero_bits) - 1 + bit_stream_->Read<uint32_t>(zero_bits);
}

int32_t GolombReader::ReadSignedValue()
{
    int32_t code_num = static_cast<int32_t>(ReadUnsignedValue());
    return (code_num & 0x01) ? (-((code_num + 1) >> 1)) : (code_num >> 1);
}

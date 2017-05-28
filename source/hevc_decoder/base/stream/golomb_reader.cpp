#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/base/stream/bit_stream.h"

GolombReadr::GolombReadr(BitStream* bit_stream)
    : bit_stream_(bit_stream)
{

}

GolombReadr::~GolombReadr()
{
    bit_stream_ = nullptr;
}

bool GolombReadr::Read(uint64* read_result)
{
    if (!bit_stream_)
        return false;

    int leading_zero_bits = 0;
    while (!bit_stream_->Read<uint8>(1))
        ++leading_zero_bits;

    int code_num = (1 << leading_zero_bits) - 1 + bit_stream_->Read<uint8>(
        leading_zero_bits);

    if (read_result)
        *read_result = code_num;

    return true;
}

bool GolombReadr::Read(int64* read_result)
{
    if (!read_result)
        return false;

    int32 code_num = 0;
    Read(reinterpret_cast<uint64*>(&code_num));
    code_num >>= 1;
    *read_result = (code_num & 0x01) ? (-code_num) : (code_num);
    return true;
}

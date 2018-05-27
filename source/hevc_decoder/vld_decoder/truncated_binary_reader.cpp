#include "hevc_decoder/vld_decoder/truncated_binary_reader.h"

#include "hevc_decoder/base/math.h"

TruncatedBinaryReader::TruncatedBinaryReader(const BitReader& bit_reader)
    : bit_reader_(bit_reader)
{

}

TruncatedBinaryReader::~TruncatedBinaryReader()
{

}

uint32_t TruncatedBinaryReader::Read(uint32_t max_value)
{
    uint32_t n = max_value + 1;
    uint32_t k = Log2(n);
    uint32_t u = (1 << (k + 1)) - n;
    uint32_t r = 0;
    while (k > 0)
    {
        r = (r << 1) | bit_reader_();
        --k;
    }
    return r >= u ? (r << 1 | bit_reader_()) - u : r;
}
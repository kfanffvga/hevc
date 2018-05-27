#include "hevc_decoder/vld_decoder/limited_exponential_golomb_reader.h"

LimitedExponentialGolombReader::LimitedExponentialGolombReader(
    const BitReader& bit_reader)
    : bit_reader_(bit_reader)
{

}

LimitedExponentialGolombReader::~LimitedExponentialGolombReader()
{

}

uint32_t LimitedExponentialGolombReader::Read(uint32_t log2_transform_range, 
                                              uint32_t rice_param)
{
    if (log2_transform_range > 28)
        return 0;

    uint32_t prefix_bit_count = 0;
    while (bit_reader_() == 1)
        ++prefix_bit_count;

    uint32_t suffix_bit_count = log2_transform_range;
    if ((28 - log2_transform_range) != prefix_bit_count)
    {
        suffix_bit_count = prefix_bit_count + rice_param;
        bit_reader_();
    }
    uint32_t ret = ((1 << prefix_bit_count) - 1) << rice_param;
    for (uint32_t i = 0; i < suffix_bit_count; ++i)
        ret = (ret << 1) | bit_reader_();

    return ret;
}


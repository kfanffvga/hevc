#include "hevc_decoder/vld_decoder/exponential_golomb_reader.h"

ExponentialGolombReader::ExponentialGolombReader(const BitReader& reader)
    : bit_reader_(reader)
{

}

ExponentialGolombReader::~ExponentialGolombReader()
{

}

uint32_t ExponentialGolombReader::Read(uint32_t exp_value)
{
    uint32_t prefix_count = 0;
    while (bit_reader_() != 0)
        ++prefix_count;

    uint32_t r = 0;
    for (uint32_t i = 0; i < prefix_count; ++i)
        r = (r << 1) | bit_reader_();

    return (((1 << prefix_count) - 1) << exp_value) + r;
}


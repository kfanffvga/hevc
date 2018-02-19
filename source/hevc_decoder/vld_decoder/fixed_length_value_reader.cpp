#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

FixedLengthValueReader::FixedLengthValueReader(const BitReader& bit_reader)
    : bit_reader_(bit_reader)
{

}

FixedLengthValueReader::~FixedLengthValueReader()
{

}

uint32_t FixedLengthValueReader::Read(uint32_t read_bits)
{
    if (!read_bits)
        return 0;

    uint32_t r = bit_reader_();
    for (uint32_t i = 0; i < read_bits - 1; ++i)
        r = (r << 1) | bit_reader_();

    return r;
}

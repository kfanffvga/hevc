#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"

TruncatedRiceValueReader::TruncatedRiceValueReader(const BitReader& bit_reader)
    : bit_reader_(bit_reader)
{

}

TruncatedRiceValueReader::~TruncatedRiceValueReader()
{

}

uint32_t TruncatedRiceValueReader::Read(uint32_t bit_count)
{
    uint32_t r = 0;
    while (r < bit_count)
    {
        if (!bit_reader_())
            break;

        ++r;
    }
    return r;
}
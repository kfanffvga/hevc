#include "hevc_decoder/vld_decoder/sao_offset_abs_reader.h"

#include <functional>
#include <algorithm>
#include <cassert>

#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"

using std::bind;
using std::min;

// 按照文档9-43表格与ffmpeg，此处都是需要读的位的数量均为
// (1 << (min(sample_bit_depth, 10ui32) - 5)) - 1
// 但按照文档9-48的表格，此处需要读的位的最大数量为5，不太明白为什么？

SAOOffsetAbsReader::SAOOffsetAbsReader(CABACReader* cabac_reader, 
                                       uint32_t sample_bit_depth)
    : CommonCABACSyntaxReader(cabac_reader)
    , max_read_bits_((1 << (min(sample_bit_depth, 10ui32) - 5)) - 1)
{
    
}

SAOOffsetAbsReader::~SAOOffsetAbsReader()
{

}

uint32_t SAOOffsetAbsReader::Read()
{
    auto bit_reader = bind(&SAOOffsetAbsReader::ReadBit, this);
    return TruncatedRiceValueReader(bit_reader).Read(max_read_bits_);
}

uint32_t SAOOffsetAbsReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName SAOOffsetAbsReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    SAOOffsetAbsReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

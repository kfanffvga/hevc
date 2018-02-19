#include "hevc_decoder/vld_decoder/sao_offset_sign_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

SAOOffsetSignReader::SAOOffsetSignReader(CABACReader* reader)
    : CommonCABACSyntaxReader(reader)
{

}

SAOOffsetSignReader::~SAOOffsetSignReader()
{

}

bool SAOOffsetSignReader::Read()
{
    auto bit_reader = bind(&SAOOffsetSignReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t SAOOffsetSignReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName SAOOffsetSignReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    SAOOffsetSignReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

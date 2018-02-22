#include "hevc_decoder/vld_decoder/sao_eo_class_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

SAOEOClassReader::SAOEOClassReader(CABACReader* reader)
    : CommonCABACSyntaxReader(reader)
{

}

SAOEOClassReader::~SAOEOClassReader()
{

}

uint32_t SAOEOClassReader::Read()
{
    auto bit_reader = bind(&SAOEOClassReader::ReadBit, this);
    return FixedLengthValueReader(bit_reader).Read(2);
}

uint32_t SAOEOClassReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName SAOEOClassReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    SAOEOClassReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

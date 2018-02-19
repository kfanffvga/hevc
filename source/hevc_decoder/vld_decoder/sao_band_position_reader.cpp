#include "hevc_decoder/vld_decoder/sao_band_position_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

SAOBandPositionReader::SAOBandPositionReader(CABACReader* reader)
    : CommonCABACSyntaxReader(reader)
{

}

SAOBandPositionReader::~SAOBandPositionReader()
{

}

uint32_t SAOBandPositionReader::Read()
{
    auto bit_reader = bind(&SAOBandPositionReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(5);
}

uint32_t SAOBandPositionReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName SAOBandPositionReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    SAOBandPositionReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

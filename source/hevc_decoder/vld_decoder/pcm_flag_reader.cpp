#include "hevc_decoder/vld_decoder/pcm_flag_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

PCMFlagReader::PCMFlagReader(CABACReader* cabac_reader)
    : CommonCABACSyntaxReader(cabac_reader)
{

}

PCMFlagReader::~PCMFlagReader()
{

}

bool PCMFlagReader::Read()
{
    auto bit_reader = bind(&PCMFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t PCMFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName PCMFlagReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PCMFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::TERMIRATE_READER;
}

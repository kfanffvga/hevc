#include "hevc_decoder/vld_decoder/last_sig_coeff_suffix_reader.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

LastSigCoeffSuffixReader::LastSigCoeffSuffixReader(CABACReader* cabac_reader)
    : CommonCABACSyntaxReader(cabac_reader)
{

}

LastSigCoeffSuffixReader::~LastSigCoeffSuffixReader()
{

}

uint32_t LastSigCoeffSuffixReader::Read(uint32_t prefix)
{
    auto bit_reader = bind(&LastSigCoeffSuffixReader::ReadBit, this);
    uint32_t bits = (prefix >> 1) - 1;
    return FixedLengthValueReader(bit_reader).Read(bits);
}

uint32_t LastSigCoeffSuffixReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName LastSigCoeffSuffixReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    LastSigCoeffSuffixReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}


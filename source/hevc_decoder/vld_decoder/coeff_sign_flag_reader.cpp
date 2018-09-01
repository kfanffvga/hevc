#include "hevc_decoder/vld_decoder/coeff_sign_flag_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

CoeffSignFlagReader::CoeffSignFlagReader(CABACReader* cabac_reader, 
                                         CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
{

}

CoeffSignFlagReader::~CoeffSignFlagReader()
{

}

bool CoeffSignFlagReader::Read()
{
    auto bit_reader = bind(&CoeffSignFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t CoeffSignFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName CoeffSignFlagReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CoeffSignFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

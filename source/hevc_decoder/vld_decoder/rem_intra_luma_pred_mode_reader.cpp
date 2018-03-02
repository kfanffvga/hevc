#include "hevc_decoder/vld_decoder/rem_intra_luma_pred_mode_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

RemIntraLumaPredModeReader::RemIntraLumaPredModeReader(
    CABACReader* cabac_reader)
    : CommonCABACSyntaxReader(cabac_reader)
{

}

RemIntraLumaPredModeReader::~RemIntraLumaPredModeReader()
{

}

uint32_t RemIntraLumaPredModeReader::Read()
{
    auto bit_reader = bind(&RemIntraLumaPredModeReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(5);
}

uint32_t RemIntraLumaPredModeReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName RemIntraLumaPredModeReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    RemIntraLumaPredModeReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}


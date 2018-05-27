#include "hevc_decoder/vld_decoder/palette_escape_val_present_flag_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

PaletteEscapeValPresentFlagReader::PaletteEscapeValPresentFlagReader(
    CABACReader* reader)
    : CommonCABACSyntaxReader(reader)
{

}

PaletteEscapeValPresentFlagReader::~PaletteEscapeValPresentFlagReader()
{

}

bool PaletteEscapeValPresentFlagReader::Read()
{
    auto bit_reader = bind(&PaletteEscapeValPresentFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t PaletteEscapeValPresentFlagReader::GetArithmeticContextIndex(
    uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName PaletteEscapeValPresentFlagReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PaletteEscapeValPresentFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

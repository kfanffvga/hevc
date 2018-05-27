#include "hevc_decoder/vld_decoder/palette_escape_val_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"
#include "hevc_decoder/vld_decoder/exponential_golomb_reader.h"

using std::bind;

PaletteEscapeValReader::PaletteEscapeValReader(CABACReader* cabac_reader)
    : CommonCABACSyntaxReader(cabac_reader)
{

}

PaletteEscapeValReader::~PaletteEscapeValReader()
{

}

uint32_t PaletteEscapeValReader::Read(bool is_cu_transquant_bypass, 
                                      uint32_t bit_depth)
{
    auto bit_reader = bind(&PaletteEscapeValReader::ReadBit, this);
    if (is_cu_transquant_bypass)
        return FixedLengthValueReader(bit_reader).Read(bit_depth);
    
    return ExponentialGolombReader(bit_reader).Read(3);
}

uint32_t PaletteEscapeValReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName PaletteEscapeValReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PaletteEscapeValReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}

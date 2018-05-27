#include "hevc_decoder/vld_decoder/palette_index_idc_reader.h"

#include "hevc_decoder/vld_decoder/truncated_binary_reader.h"

using std::bind;

PaletteIndexIdcReader::PaletteIndexIdcReader(CABACReader* cabac_reader)
    :  CommonCABACSyntaxReader(cabac_reader)
{

}

PaletteIndexIdcReader::~PaletteIndexIdcReader()
{

}

uint32_t PaletteIndexIdcReader::Read(bool first_invoke_in_cb, 
                                     uint32_t max_palette_index)
{
    static uint32_t current_max_palette_index = 0;
    current_max_palette_index = 
        first_invoke_in_cb ? max_palette_index : current_max_palette_index - 1;

    auto bit_reader = bind(&PaletteIndexIdcReader::ReadBit, this);
    return TruncatedBinaryReader(bit_reader).Read(current_max_palette_index);
}

uint32_t PaletteIndexIdcReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName PaletteIndexIdcReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PaletteIndexIdcReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}


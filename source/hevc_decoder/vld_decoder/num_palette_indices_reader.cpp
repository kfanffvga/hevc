#include "hevc_decoder/vld_decoder/num_palette_indices_reader.h"

#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"
#include "hevc_decoder/vld_decoder/exponential_golomb_reader.h"

using std::bind;

NumPaletteIndicesReader::NumPaletteIndicesReader(CABACReader* reader)
    : CommonCABACSyntaxReader(reader)
{

}

NumPaletteIndicesReader::~NumPaletteIndicesReader()
{

}

uint32_t NumPaletteIndicesReader::Read(uint32_t max_palette_index)
{
    uint32_t rice_param = 3 + ((max_palette_index + 1) >> 3);
    uint32_t prefix_max_value = 4 << rice_param;
    auto bit_reader = bind(&NumPaletteIndicesReader::ReadBit, this);
    uint32_t prefix_value = 
        TruncatedRiceValueReader(bit_reader).Read(prefix_max_value, rice_param);
    if ((prefix_value >> rice_param) != 0xf)
        return prefix_value;

    uint32_t suffix_value = 
        ExponentialGolombReader(bit_reader).Read(rice_param + 1);
    return prefix_max_value + suffix_value;
}

uint32_t NumPaletteIndicesReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return 0;
}

SyntaxElementName NumPaletteIndicesReader::GetSyntaxElementName()
{
    return SYNTAX_ELEMENT_NAME_COUNT;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    NumPaletteIndicesReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::BYPASS_READER;
}


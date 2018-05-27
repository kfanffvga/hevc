#include "hevc_decoder/vld_decoder/palette_transpose_flag_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"

using std::bind;

PaletteTransposeFlagReader::PaletteTransposeFlagReader(
    CABACReader* cabac_reader, CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(PALETTE_TRANSPOSE_FLAG, 
                                                init_type))
{

}

PaletteTransposeFlagReader::~PaletteTransposeFlagReader()
{

}

bool PaletteTransposeFlagReader::Read()
{
    auto bit_reader = bind(&PaletteTransposeFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t PaletteTransposeFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName PaletteTransposeFlagReader::GetSyntaxElementName()
{
    return PALETTE_TRANSPOSE_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PaletteTransposeFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}


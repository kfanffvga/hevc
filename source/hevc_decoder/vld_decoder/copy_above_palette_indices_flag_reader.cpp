#include "hevc_decoder/vld_decoder/copy_above_palette_indices_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

CopyAbovePaletteIndicesFlagReader::CopyAbovePaletteIndicesFlagReader(
    CABACReader* cabac_reader, CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(COPY_ABOVE_PALETTE_INDICES_FLAG,
                                                init_type))
{

}

CopyAbovePaletteIndicesFlagReader::~CopyAbovePaletteIndicesFlagReader()
{

}

bool CopyAbovePaletteIndicesFlagReader::Read()
{
    auto bit_reader = bind(&CopyAbovePaletteIndicesFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t CopyAbovePaletteIndicesFlagReader::GetArithmeticContextIndex(
    uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName CopyAbovePaletteIndicesFlagReader::GetSyntaxElementName()
{
    return COPY_ABOVE_PALETTE_INDICES_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CopyAbovePaletteIndicesFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}


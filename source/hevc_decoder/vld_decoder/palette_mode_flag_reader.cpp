#include "hevc_decoder/vld_decoder/palette_mode_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

PaletteModeFlagReader::PaletteModeFlagReader(CABACReader* reader, 
                                             CABACInitType init_type)
    : CommonCABACSyntaxReader(reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(PALETTE_MODE_FLAG, init_type))
{

}

PaletteModeFlagReader::~PaletteModeFlagReader()
{

}

bool PaletteModeFlagReader::Read()
{
    auto bit_reader = bind(&PaletteModeFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t PaletteModeFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName PaletteModeFlagReader::GetSyntaxElementName()
{
    return PALETTE_MODE_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PaletteModeFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}


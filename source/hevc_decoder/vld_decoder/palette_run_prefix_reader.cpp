#include "hevc_decoder/vld_decoder/palette_run_prefix_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"
#include "hevc_decoder/base/math.h"

using std::bind;

PaletteRunPrefixReader::PaletteRunPrefixReader(CABACReader* cabac_reader, 
                                               CABACInitType init_type, 
                                               bool is_copy_above_palette_index, 
                                               uint32_t palette_index_idc)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(PALETTE_RUN_PREFIX, init_type))
    , is_copy_above_palette_index_(is_copy_above_palette_index)
    , palette_index_idc_(palette_index_idc)
{

}

PaletteRunPrefixReader::~PaletteRunPrefixReader()
{

}

uint32_t PaletteRunPrefixReader::Read(uint32_t palette_max_run)
{
    auto bit_reader = bind(&PaletteRunPrefixReader::ReadBit, this);
    uint32_t log2_palette_max_run = CeilLog2(palette_max_run);
    return TruncatedRiceValueReader(bit_reader).Read(log2_palette_max_run, 0);
}

uint32_t PaletteRunPrefixReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    if (bin_idx > 4)
        return 0;

    uint32_t context_increase = 0;
    if (is_copy_above_palette_index_)
    {
        if (0 == bin_idx)
        {
            context_increase = 
                palette_index_idc_ < 1 ? 0 : (palette_index_idc_ < 3 ? 1 : 2);
        }
        else if (bin_idx <= 4)
        {
            const uint32_t coef[] = {3, 3, 4, 4};
            context_increase = coef[bin_idx - 1];
        }
    }
    else
    {
        const uint32_t coef[] = {5, 6, 6, 7, 7};
        context_increase = coef[bin_idx];
    }
    return lowest_context_index_ + context_increase;
}

SyntaxElementName PaletteRunPrefixReader::GetSyntaxElementName()
{
    return PALETTE_RUN_PREFIX;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    PaletteRunPrefixReader::GetFunctionIndex(uint16_t bin_idx)
{
    return bin_idx > 4 ? CommonCABACSyntaxReader::BYPASS_READER :
        CommonCABACSyntaxReader::ARITHMETIC_READER;
}

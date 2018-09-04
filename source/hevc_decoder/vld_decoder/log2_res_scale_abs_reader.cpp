#include "hevc_decoder/vld_decoder/log2_res_scale_abs_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"

using std::bind;

Log2ResScaleAbsReader::Log2ResScaleAbsReader(CABACReader* cabac_reader, 
                                             CABACInitType init_type,
                                             uint32_t color_index_dec1)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(LOG2_RES_SCALE_ABS, init_type))
    , color_index_dec1_(color_index_dec1)
{

}

Log2ResScaleAbsReader::~Log2ResScaleAbsReader()
{

}

int32_t Log2ResScaleAbsReader::Read()
{
    auto bit_reader = bind(&Log2ResScaleAbsReader::ReadBit, this);
    return static_cast<int>(TruncatedRiceValueReader(bit_reader).Read(4, 0)) - 1;
}

uint32_t Log2ResScaleAbsReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_ + (color_index_dec1_ << 2) + bin_idx;
}

SyntaxElementName Log2ResScaleAbsReader::GetSyntaxElementName()
{
    return LOG2_RES_SCALE_ABS;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    Log2ResScaleAbsReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
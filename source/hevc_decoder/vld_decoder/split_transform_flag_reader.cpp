#include "hevc_decoder/vld_decoder/split_transform_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

SplitTransformFlagReader::SplitTransformFlagReader(
    CABACReader* cabac_reader, CABACInitType init_type,
    uint32_t log2_transform_block_size_y)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(SPLIT_TRANSFORM_FLAG, init_type))
    , log2_transform_block_size_y_(log2_transform_block_size_y)
{

}

SplitTransformFlagReader::~SplitTransformFlagReader()
{

}

bool SplitTransformFlagReader::Read()
{
    auto bit_reader = bind(&SplitTransformFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t SplitTransformFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_ + 5 - log2_transform_block_size_y_;
}

SyntaxElementName SplitTransformFlagReader::GetSyntaxElementName()
{
    return SPLIT_TRANSFORM_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    SplitTransformFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}

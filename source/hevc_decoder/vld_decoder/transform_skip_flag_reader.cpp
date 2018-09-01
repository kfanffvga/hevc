#include "hevc_decoder/vld_decoder/transform_skip_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

namespace
{
const SyntaxElementName transform_skip_flag_names[3] = 
{
    TRANSFORM_SKIP_FLAG_FOR_LUMA, TRANSFORM_SKIP_FLAG_FOR_CHROMA, 
    TRANSFORM_SKIP_FLAG_FOR_CHROMA
};
}

TransformSkipFlagReader::TransformSkipFlagReader(CABACReader* cabac_reader, 
                                                 CABACInitType init_type,
                                                 uint32_t color_index)
    : CommonCABACSyntaxReader(cabac_reader)
    , color_index_(color_index)
    , transform_skip_flag_name_(transform_skip_flag_names[color_index % 3])
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(
            transform_skip_flag_names[color_index % 3], init_type))
{

}

TransformSkipFlagReader::~TransformSkipFlagReader()
{

}

bool TransformSkipFlagReader::Read()
{
    auto bit_reader = bind(&TransformSkipFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t TransformSkipFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName TransformSkipFlagReader::GetSyntaxElementName()
{
    return transform_skip_flag_name_;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    TransformSkipFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
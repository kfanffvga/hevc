#include "hevc_decoder/vld_decoder/last_sig_coeff_prefix_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"

using std::bind;

LastSigCoeffPrefixReader::LastSigCoeffPrefixReader(CABACReader* cabac_reader, 
                                                   CABACInitType init_type, 
                                                   uint32_t log2_transform_size, 
                                                   uint32_t color_index,
                                                   SyntaxElementName name)
    : CommonCABACSyntaxReader(cabac_reader)
    , color_index_(color_index)
    , log2_transform_size_(log2_transform_size)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(name, init_type))
    , name_(name)
{

}

LastSigCoeffPrefixReader::~LastSigCoeffPrefixReader()
{

}

uint32_t LastSigCoeffPrefixReader::Read()
{
    auto bit_reader = bind(&LastSigCoeffPrefixReader::ReadBit, this);
    uint32_t max_value = (log2_transform_size_ << 1) - 1;
    return TruncatedRiceValueReader(bit_reader).Read(max_value, 0);
}

uint32_t LastSigCoeffPrefixReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    uint32_t context_offset = 0;
    uint32_t context_shift = 0;
    if (0 == color_index_)
    {
        context_offset = 3 * (log2_transform_size_ - 2) + 
            ((log2_transform_size_ - 1) >> 2);
        context_shift = (log2_transform_size_ + 1) >> 2;
    }
    else
    {
        context_offset = 15;
        context_shift = log2_transform_size_ - 2;
    }
    uint32_t context_increase = (bin_idx >> context_shift) + context_offset;
    return lowest_context_index_ + context_increase;
}

SyntaxElementName LastSigCoeffPrefixReader::GetSyntaxElementName()
{
    return name_;
}

CommonCABACSyntaxReader::ReadFunctionIndex
    LastSigCoeffPrefixReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}

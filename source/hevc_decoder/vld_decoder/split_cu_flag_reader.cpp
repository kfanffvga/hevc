#include "hevc_decoder/vld_decoder/split_cu_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

SplitCUFlagReader::SplitCUFlagReader(CABACReader* cabac_reader, 
                                     CABACInitType init_type,
                                     ISplitCUFlagReaderContext* context)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(SPLIT_CU_FLAG, init_type))
    , context_(context)
{

}

SplitCUFlagReader::~SplitCUFlagReader()
{

}

bool SplitCUFlagReader::Read()
{
    auto bit_reader = bind(&SplitCUFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t SplitCUFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    uint32_t left_cb_parameter = static_cast<uint32_t>(
        context_->IsLeftBlockAvailable() && 
        (context_->GetLeftBlockLayer() > context_->GetCurrentBlockLayer()));
    uint32_t up_cb_parameter = static_cast<uint32_t>(
        context_->IsUpBlockAvailable() &&
        (context_->GetUpBlockLayer() > context_->GetCurrentBlockLayer()));

    return lowest_context_index_ + left_cb_parameter + up_cb_parameter;
}

SyntaxElementName SplitCUFlagReader::GetSyntaxElementName()
{
    return SPLIT_CU_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    SplitCUFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}

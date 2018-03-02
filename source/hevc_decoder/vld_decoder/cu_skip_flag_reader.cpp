#include "hevc_decoder/vld_decoder/cu_skip_flag_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"

using std::bind;

CUSkipFlagReader::CUSkipFlagReader(CABACReader* cabac_reader, 
                                   CABACInitType init_type,
                                   ICUSkipFlagReaderContext* context)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(CU_SKIP_FLAG, init_type))
    , context_(context)
{

}

CUSkipFlagReader::~CUSkipFlagReader()
{

}

bool CUSkipFlagReader::Read()
{
    auto bit_reader = bind(&CUSkipFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t CUSkipFlagReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    uint32_t left_cb_parameter = static_cast<uint32_t>(
        context_->IsLeftBlockAvailable() &&
        (context_->GetLeftBlockLayer() > context_->GetCurrentBlockLayer()));
    uint32_t up_cb_parameter = static_cast<uint32_t>(
        context_->IsUpBlockAvailable() &&
        (context_->GetUpBlockLayer() > context_->GetCurrentBlockLayer()));

    return lowest_context_index_ + left_cb_parameter + up_cb_parameter;
}

SyntaxElementName CUSkipFlagReader::GetSyntaxElementName()
{
    return CU_SKIP_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CUSkipFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}
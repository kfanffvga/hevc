#include "hevc_decoder/vld_decoder/copy_above_indices_for_final_run_flag_reader.h"

#include "hevc_decoder/vld_decoder/fixed_length_value_reader.h"
#include "hevc_decoder/vld_decoder/cabac_context_storage.h"

using std::bind;

CopyAboveIndicesForFinalRunFlagReader::CopyAboveIndicesForFinalRunFlagReader(
    CABACReader* cabac_reader, CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(
            COPY_ABOVE_INDICES_FOR_FINAL_RUN_FLAG, init_type))
{

}

CopyAboveIndicesForFinalRunFlagReader::~CopyAboveIndicesForFinalRunFlagReader()
{

}

bool CopyAboveIndicesForFinalRunFlagReader::Read()
{
    auto bit_reader = bind(&CopyAboveIndicesForFinalRunFlagReader::ReadBit, this);
    return !!FixedLengthValueReader(bit_reader).Read(1);
}

uint32_t CopyAboveIndicesForFinalRunFlagReader::GetArithmeticContextIndex(
    uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName CopyAboveIndicesForFinalRunFlagReader::GetSyntaxElementName()
{
    return COPY_ABOVE_INDICES_FOR_FINAL_RUN_FLAG;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CopyAboveIndicesForFinalRunFlagReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}


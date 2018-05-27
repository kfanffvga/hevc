#include "hevc_decoder/vld_decoder/cu_qp_delta_abs_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"
#include "hevc_decoder/vld_decoder/exponential_golomb_reader.h"

using std::bind;

CUQPDeltaAbsReader::CUQPDeltaAbsReader(CABACReader* reader, 
                                       CABACInitType init_type)
    : CommonCABACSyntaxReader(reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(CU_QP_DELTA_ABS, init_type))
{

}

CUQPDeltaAbsReader::~CUQPDeltaAbsReader()
{

}

uint32_t CUQPDeltaAbsReader::Read()
{
    auto bit_reader = bind(&CUQPDeltaAbsReader::ReadBit, this);
    uint32_t value = TruncatedRiceValueReader(bit_reader).Read(5, 0);
    if (value < 5)
        return value;

    return value + ExponentialGolombReader(bit_reader).Read(0);
}

uint32_t CUQPDeltaAbsReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return (bin_idx > 0) && (bin_idx < 5) ? 1 : 0;
}

SyntaxElementName CUQPDeltaAbsReader::GetSyntaxElementName()
{
    return CU_QP_DELTA_ABS;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CUQPDeltaAbsReader::GetFunctionIndex(uint16_t bin_idx)
{
    return bin_idx < 5 ? ARITHMETIC_READER : BYPASS_READER;
}


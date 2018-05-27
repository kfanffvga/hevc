#include "hevc_decoder/vld_decoder/cu_chroma_qp_offset_idx_reader.h"

#include "hevc_decoder/vld_decoder/cabac_context_storage.h"
#include "hevc_decoder/vld_decoder/truncated_rice_value_reader.h"

using std::bind;

CUChromaQPOffsetIDXReader::CUChromaQPOffsetIDXReader(CABACReader* cabac_reader, 
                                                     CABACInitType init_type)
    : CommonCABACSyntaxReader(cabac_reader)
    , lowest_context_index_(
        CABACContextStorage::GetLowestContextID(CU_CHROMA_QP_OFFSET_IDX,
                                                init_type))
{

}

CUChromaQPOffsetIDXReader::~CUChromaQPOffsetIDXReader()
{

}

uint32_t CUChromaQPOffsetIDXReader::Read(uint32_t chroma_qp_offset_list_len)
{
    auto bit_reader = bind(&CUChromaQPOffsetIDXReader::ReadBit, this);
    return TruncatedRiceValueReader(bit_reader).Read(chroma_qp_offset_list_len, 
                                                     0);
}

uint32_t CUChromaQPOffsetIDXReader::GetArithmeticContextIndex(uint16_t bin_idx)
{
    return lowest_context_index_;
}

SyntaxElementName CUChromaQPOffsetIDXReader::GetSyntaxElementName()
{
    return CU_CHROMA_QP_OFFSET_IDX;
}

CommonCABACSyntaxReader::ReadFunctionIndex 
    CUChromaQPOffsetIDXReader::GetFunctionIndex(uint16_t bin_idx)
{
    return CommonCABACSyntaxReader::ARITHMETIC_READER;
}


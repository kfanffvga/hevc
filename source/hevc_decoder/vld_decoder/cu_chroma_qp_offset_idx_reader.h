#ifndef _CU_CHROMA_QP_OFFSET_IDX_READER_H_
#define _CU_CHROMA_QP_OFFSET_IDX_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CUChromaQPOffsetIDXReader : public CommonCABACSyntaxReader
{
public:
    CUChromaQPOffsetIDXReader(CABACReader* cabac_reader, CABACInitType init_type);
    virtual ~CUChromaQPOffsetIDXReader();

    uint32_t Read(uint32_t chroma_qp_offset_list_len);

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
};

#endif

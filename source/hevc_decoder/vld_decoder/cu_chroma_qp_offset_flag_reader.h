#ifndef _CU_CHROMA_QP_OFFSET_FLAG_READER_H_
#define _CU_CHROMA_QP_OFFSET_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CUChromaQPOffsetFlagReader : public CommonCABACSyntaxReader
{
public:
    CUChromaQPOffsetFlagReader(CABACReader* cabac_reader, 
                               CABACInitType init_type);

    virtual ~CUChromaQPOffsetFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
};

#endif

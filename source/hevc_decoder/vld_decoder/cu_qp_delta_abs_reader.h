#ifndef _CU_QP_DELTA_ABS_READER_H_
#define _CU_QP_DELTA_ABS_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CUQPDeltaAbsReader : public CommonCABACSyntaxReader
{
public:
    CUQPDeltaAbsReader(CABACReader* reader, CABACInitType init_type);
    virtual ~CUQPDeltaAbsReader();

    uint32_t Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;

};

#endif

#ifndef _CU_QP_DELTA_SIGN_FLAG_READER_H_
#define _CU_QP_DELTA_SIGN_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CUQPDeltaSignFlagReader : public CommonCABACSyntaxReader
{
public:
    CUQPDeltaSignFlagReader(CABACReader* reader);
    virtual ~CUQPDeltaSignFlagReader();
    
    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif

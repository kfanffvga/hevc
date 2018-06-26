#ifndef _TU_RESIDUAL_ACT_FLAG_READER_H_
#define _TU_RESIDUAL_ACT_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class TUResidualActFlagReader : public CommonCABACSyntaxReader
{
public:
    TUResidualActFlagReader(CABACReader* cabac_reader, CABACInitType init_type);
    virtual ~TUResidualActFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
};
#endif 

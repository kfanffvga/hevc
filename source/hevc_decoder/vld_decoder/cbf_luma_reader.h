#ifndef _CBF_LUMA_READER_H_
#define _CBF_LUMA_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CBFLumaReader : public CommonCABACSyntaxReader
{
public:
    CBFLumaReader(CABACReader* cabac_reader, CABACInitType init_type, 
                  uint32_t depth);

    virtual ~CBFLumaReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    uint32_t depth_;
};

#endif

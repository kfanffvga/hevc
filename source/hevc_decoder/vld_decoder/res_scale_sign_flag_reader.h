#ifndef _RES_SCALE_SIGN_FLAG_READER_H_
#define _RES_SCALE_SIGN_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class ResScaleSignFlagReader : public CommonCABACSyntaxReader
{
public:
    ResScaleSignFlagReader(CABACReader* cabac_reader, CABACInitType init_type,
                           uint32_t color_index_dec1);

    virtual ~ResScaleSignFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    uint32_t color_index_dec1_;
};

#endif

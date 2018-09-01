#ifndef _COEFF_SIGN_FLAG_READER_H_
#define _COEFF_SIGN_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CoeffSignFlagReader : public CommonCABACSyntaxReader
{
public:
    CoeffSignFlagReader(CABACReader* cabac_reader, CABACInitType init_type);
    virtual ~CoeffSignFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif

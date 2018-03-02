#ifndef _REM_INTRA_LUMA_PRED_MODE_READER_H_
#define _REM_INTRA_LUMA_PRED_MODE_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class RemIntraLumaPredModeReader : public CommonCABACSyntaxReader
{
public:
    RemIntraLumaPredModeReader(CABACReader* cabac_reader);
    virtual ~RemIntraLumaPredModeReader();

    uint32_t Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif

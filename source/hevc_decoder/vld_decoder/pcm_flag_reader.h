#ifndef _PCM_FLAG_READER_H_
#define _PCM_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class PCMFlagReader : public CommonCABACSyntaxReader
{
public:
    PCMFlagReader(CABACReader* cabac_reader);
    virtual ~PCMFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif

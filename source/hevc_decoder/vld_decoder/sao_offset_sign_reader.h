#ifndef _SAO_OFFSET_SIGN_READER_H_
#define _SAO_OFFSET_SIGN_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class SAOOffsetSignReader : public CommonCABACSyntaxReader
{
public:
    SAOOffsetSignReader(CABACReader* reader);
    virtual ~SAOOffsetSignReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};
#endif

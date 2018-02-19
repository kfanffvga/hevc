#ifndef _SAO_EO_CLASS_READER_H_
#define _SAO_EO_CLASS_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class SAOEOClassReader : public CommonCABACSyntaxReader
{
public:
    SAOEOClassReader(CABACReader* reader);
    virtual ~SAOEOClassReader();

    uint32_t Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

};
#endif

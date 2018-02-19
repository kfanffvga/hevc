#ifndef _SAO_BAND_POSITION_READER_H_
#define _SAO_BAND_POSITION_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class SAOBandPositionReader : public CommonCABACSyntaxReader
{
public:
    SAOBandPositionReader(CABACReader* reader);
    virtual ~SAOBandPositionReader();

    uint32_t Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

};
#endif

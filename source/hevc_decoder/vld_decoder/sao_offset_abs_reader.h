#ifndef _SAO_OFFSET_ABS_READER_H_
#define _SAO_OFFSET_ABS_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class SAOOffsetAbsReader : public CommonCABACSyntaxReader
{
public:
    SAOOffsetAbsReader(CABACReader* cabac_reader, uint32_t sample_bit_depth);
    virtual ~SAOOffsetAbsReader();

    uint32_t Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t max_values_;
};

#endif

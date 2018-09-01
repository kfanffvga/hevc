#ifndef _COEFF_ABS_LEVEL_GREATER_2_FLAG_READER_H_
#define _COEFF_ABS_LEVEL_GREATER_2_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CoeffAbsLevelGreaterNumberFlagReaderContext;

class CoeffAbsLevelGreater2FlagReader : public CommonCABACSyntaxReader
{
public:
    CoeffAbsLevelGreater2FlagReader(
        CABACReader* cabac_reader, CABACInitType init_type, 
        CoeffAbsLevelGreaterNumberFlagReaderContext* context, 
        uint32_t color_index);

    virtual ~CoeffAbsLevelGreater2FlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    uint32_t color_index_;
    CoeffAbsLevelGreaterNumberFlagReaderContext* context_;
};

#endif

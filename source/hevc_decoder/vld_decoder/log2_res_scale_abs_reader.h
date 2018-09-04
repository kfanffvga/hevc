#ifndef _LOG2_RES_SCALE_ABS_READER_H_
#define _LOG2_RES_SCALE_ABS_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class Log2ResScaleAbsReader : public CommonCABACSyntaxReader
{
public:
    Log2ResScaleAbsReader(CABACReader* cabac_reader, CABACInitType init_type,
                          uint32_t color_index_dec1);
    virtual ~Log2ResScaleAbsReader();

    int32_t Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
    
    uint32_t lowest_context_index_;
    uint32_t color_index_dec1_;

};

#endif

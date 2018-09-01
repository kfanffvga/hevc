#ifndef _LAST_SIG_COEFF_PREFIX_READER_H_
#define _LAST_SIG_COEFF_PREFIX_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class LastSigCoeffPrefixReader : public CommonCABACSyntaxReader
{
public:    
    virtual ~LastSigCoeffPrefixReader();
    uint32_t Read();

protected:
    LastSigCoeffPrefixReader(CABACReader* cabac_reader, CABACInitType init_type, 
                             uint32_t log2_transform_size,uint32_t color_index, 
                             SyntaxElementName name);

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t color_index_;
    uint32_t log2_transform_size_;
    uint32_t lowest_context_index_;
    SyntaxElementName name_;
};

#endif

#ifndef _CODED_SUB_BLOCK_FLAG_READER_H_
#define _CODED_SUB_BLOCK_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CodedSubBlockFlagReader : public CommonCABACSyntaxReader
{
public:
    CodedSubBlockFlagReader(CABACReader* cabac_reader, CABACInitType init_type,
                            bool has_coded_sub_block_on_right, 
                            bool has_coded_sub_block_on_bottom, 
                            uint32_t color_index);

    virtual ~CodedSubBlockFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    bool has_coded_sub_block_on_right_;
    bool has_coded_sub_block_on_bottom_;
    uint32_t color_index_;
};
#endif
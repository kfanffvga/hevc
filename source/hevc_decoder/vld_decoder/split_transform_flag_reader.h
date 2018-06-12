#ifndef _SPLIT_TRANSFORM_FLAG_READER_H_
#define _SPLIT_TRANSFORM_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class SplitTransformFlagReader : public CommonCABACSyntaxReader
{
public:
    SplitTransformFlagReader(CABACReader* cabac_reader, CABACInitType init_type,
                             uint32_t log2_transform_block_size_y);

    virtual ~SplitTransformFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    uint32_t log2_transform_block_size_y_;
};

#endif

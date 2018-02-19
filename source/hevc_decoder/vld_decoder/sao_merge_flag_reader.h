#ifndef _SAO_MERGE_FLAG_READER_H_
#define _SAO_MERGE_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class SAOMergeFlagReader : public CommonCABACSyntaxReader
{
public:
    SAOMergeFlagReader(CABACReader* cabac_reader, CABACInitType init_type);
    virtual ~SAOMergeFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
};

#endif

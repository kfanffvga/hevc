#ifndef _SAO_TYPE_INDEX_READER_H_
#define _SAO_TYPE_INDEX_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class SAOTypeIndexReader : public CommonCABACSyntaxReader
{
public:
    SAOTypeIndexReader(CABACReader* cabac_reader, CABACInitType init_type);
    virtual ~SAOTypeIndexReader();

    uint32_t Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
};
#endif

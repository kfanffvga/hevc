#ifndef _CBF_CB_READER_H_
#define _CBF_CB_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CBFCBReader : public CommonCABACSyntaxReader
{
public:
    CBFCBReader(CABACReader* cabac_reader, CABACInitType init_type, 
                uint32_t layer);

    virtual ~CBFCBReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    uint32_t layer_;
};

#endif

#ifndef _INFRA_CHROMA_PRED_MODE_READER_H_
#define _INFRA_CHROMA_PRED_MODE_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

enum IntraPredModeType;

class IntraChromaPredModeReader : public CommonCABACSyntaxReader
{
public:
    IntraChromaPredModeReader(CABACReader* cabac_reader, 
                              CABACInitType init_type);
    virtual ~IntraChromaPredModeReader();

    uint32_t Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
};

#endif

#ifndef _COPY_ABOVE_INDICES_FOR_FINAL_RUN_FLAG_READER_H_
#define _COPY_ABOVE_INDICES_FOR_FINAL_RUN_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class CopyAboveIndicesForFinalRunFlagReader : public CommonCABACSyntaxReader
{
public:
    CopyAboveIndicesForFinalRunFlagReader(CABACReader* cabac_reader,
                                          CABACInitType init_type);
    virtual ~CopyAboveIndicesForFinalRunFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
};
#endif

#ifndef _PRED_MODE_FLAG_READER_H_
#define _PRED_MODE_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"
#include "hevc_decoder/base/basic_types.h"

class PredModeFlagReader : public CommonCABACSyntaxReader
{
public:
    PredModeFlagReader(CABACReader* cabac_reader, CABACInitType init_type);
    virtual ~PredModeFlagReader();

    PredModeType Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
};

#endif

#ifndef _MPM_IDX_READER_H_
#define _MPM_IDX_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class MPMIdxReader : public CommonCABACSyntaxReader
{
public:
    MPMIdxReader(CABACReader* cabac_reader);
    virtual ~MPMIdxReader();

    uint32_t Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

};

#endif

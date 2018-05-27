#ifndef _PALETTE_RUN_SUFFIX_READER_H_
#define _PALETTE_RUN_SUFFIX_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class PaletteRunSuffixReader : public CommonCABACSyntaxReader
{
public:
    PaletteRunSuffixReader(CABACReader* reader);
    virtual ~PaletteRunSuffixReader();

    uint32_t Read(uint32_t prefix_offset, uint32_t palette_max_run);

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif

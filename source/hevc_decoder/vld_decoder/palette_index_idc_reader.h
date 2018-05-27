#ifndef _PALETTE_INDEX_IDC_READER_H_
#define _PALETTE_INDEX_IDC_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class PaletteIndexIdcReader : public CommonCABACSyntaxReader
{
public:
    PaletteIndexIdcReader(CABACReader* cabac_reader);
    virtual ~PaletteIndexIdcReader();

    uint32_t Read(bool first_invoke_in_cb, uint32_t max_palette_index);

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif

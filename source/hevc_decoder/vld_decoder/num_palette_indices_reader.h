#ifndef _NUM_PALETTE_INDICES_READER_H_ 
#define _NUM_PALETTE_INDICES_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class NumPaletteIndicesReader : public CommonCABACSyntaxReader
{
public:
    NumPaletteIndicesReader(CABACReader* reader);
    virtual ~NumPaletteIndicesReader();

    uint32_t Read(uint32_t max_palette_index);

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif 

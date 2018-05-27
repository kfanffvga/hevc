#ifndef _PALETTE_ESCAPE_VAL_READER_H_
#define _PALETTE_ESCAPE_VAL_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class PaletteEscapeValReader : public CommonCABACSyntaxReader
{
public:
    PaletteEscapeValReader(CABACReader* cabac_reader);
    virtual ~PaletteEscapeValReader();

    uint32_t Read(bool is_cu_transquant_bypass, uint32_t bit_depth);

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif

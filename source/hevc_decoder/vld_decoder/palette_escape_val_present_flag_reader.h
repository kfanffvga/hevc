#ifndef _PALETTE_ESCAPE_VAL_PRESENT_FLAG_READER_H_
#define _PALETTE_ESCAPE_VAL_PRESENT_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class PaletteEscapeValPresentFlagReader : public CommonCABACSyntaxReader
{
public:
    PaletteEscapeValPresentFlagReader(CABACReader* reader);
    virtual ~PaletteEscapeValPresentFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif // !_PALETTE_ESCAPE_VAL_PRESENT_FLAG_READER_H_

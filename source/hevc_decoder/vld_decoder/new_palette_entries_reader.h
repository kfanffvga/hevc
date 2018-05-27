#ifndef _NEW_PALETTE_ENTRIES_READER_H_
#define _NEW_PALETTE_ENTRIES_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class NewPaletteEntriesReader : public CommonCABACSyntaxReader
{
public:
    NewPaletteEntriesReader(CABACReader* reader);
    virtual ~NewPaletteEntriesReader();

    uint32_t Read(uint32_t bit_depth);

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

#endif 

#ifndef _PALETTE_TRANSPOSE_FLAG_READER_H_
#define _PALETTE_TRANSPOSE_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class PaletteTransposeFlagReader : public CommonCABACSyntaxReader
{
public:
    PaletteTransposeFlagReader(CABACReader* cabac_reader, 
                               CABACInitType init_type);
    virtual ~PaletteTransposeFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
};

#endif

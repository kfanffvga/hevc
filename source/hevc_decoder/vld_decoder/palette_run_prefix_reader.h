#ifndef _PALETTE_RUN_PREFIX_READER_H_
#define _PALETTE_RUN_PREFIX_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class PaletteRunPrefixReader : public CommonCABACSyntaxReader
{
public:
    PaletteRunPrefixReader(CABACReader* cabac_reader, CABACInitType init_type,
                           bool is_copy_above_palette_index, 
                           uint32_t palette_index_idc);
    virtual ~PaletteRunPrefixReader();

    uint32_t Read(uint32_t palette_max_run);

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    bool is_copy_above_palette_index_;
    uint32_t palette_index_idc_;
};
#endif

#ifndef _EXPLICIT_RDPCM_FLAG_READER_H_
#define _EXPLICIT_RDPCM_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class ExplicitRDPCMFlagReader : public CommonCABACSyntaxReader
{
public:
    ExplicitRDPCMFlagReader(CABACReader* cabac_reader, CABACInitType init_type,
                            uint32_t color_index);
    virtual ~ExplicitRDPCMFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t color_index_;
    SyntaxElementName explicit_rdpcm_flag_name_;
    uint32_t lowest_context_index_;
};

#endif

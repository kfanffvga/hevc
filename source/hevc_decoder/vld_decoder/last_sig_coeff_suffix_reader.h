#ifndef _LAST_SIG_COEFF_SUFFIX_READER_H_
#define _LAST_SIG_COEFF_SUFFIX_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class LastSigCoeffSuffixReader : public CommonCABACSyntaxReader
{
public:
    uint32_t Read(uint32_t prefix);

protected:
    LastSigCoeffSuffixReader(CABACReader* cabac_reader);
    virtual ~LastSigCoeffSuffixReader();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    SyntaxElementName name_;

};
#endif

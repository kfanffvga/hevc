#ifndef _LAST_SIG_COEFF_Y_SUFFIX_READER_H_
#define _LAST_SIG_COEFF_Y_SUFFIX_READER_H_

#include "hevc_decoder/vld_decoder/last_sig_coeff_suffix_reader.h"

class LastSigCoeffYSuffixReader : public LastSigCoeffSuffixReader
{
public:
    LastSigCoeffYSuffixReader(CABACReader* cabac_reader)
        : LastSigCoeffSuffixReader(cabac_reader)
    {

    }
    virtual ~LastSigCoeffYSuffixReader()
    {

    }
};

#endif

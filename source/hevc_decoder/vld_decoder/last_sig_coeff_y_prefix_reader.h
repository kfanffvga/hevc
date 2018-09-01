#ifndef _LAST_SIG_COEFF_Y_PREFIX_READER_H_
#define _LAST_SIG_COEFF_Y_PREFIX_READER_H_

#include "hevc_decoder/vld_decoder/last_sig_coeff_prefix_reader.h"

class LastSigCoeffYPrefixReader : public LastSigCoeffPrefixReader
{
public:
    LastSigCoeffYPrefixReader(CABACReader* cabac_reader, CABACInitType init_type,
                              uint32_t log2_transform_size, uint32_t color_index)
        : LastSigCoeffPrefixReader(cabac_reader, init_type, log2_transform_size,
                                   color_index, LAST_SIG_COEFF_Y_PREFIX)
    {

    }

    virtual ~LastSigCoeffYPrefixReader()
    {

    }
};


#endif

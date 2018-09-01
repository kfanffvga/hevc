#ifndef _LAST_SIG_COEFF_X_PREFIX_READER_H_
#define _LAST_SIG_COEFF_X_PREFIX_READER_H_

#include "hevc_decoder/vld_decoder/last_sig_coeff_prefix_reader.h"

class LastSigCoeffXPrefixReader : public LastSigCoeffPrefixReader
{
public:
    LastSigCoeffXPrefixReader(CABACReader* cabac_reader, CABACInitType init_type,
                              uint32_t log2_transform_size, uint32_t color_index)
        : LastSigCoeffPrefixReader(cabac_reader, init_type, log2_transform_size, 
                                   color_index, LAST_SIG_COEFF_X_PREFIX)
    {

    }

    virtual ~LastSigCoeffXPrefixReader()
    {

    }
};


#endif

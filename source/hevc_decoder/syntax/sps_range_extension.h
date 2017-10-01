#ifndef _SPS_RANGE_EXTENSION_H_
#define _SPS_RANGE_EXTENSION_H_

#include "hevc_decoder/syntax/base_syntax.h"

class SPSRangeExtension : public BaseSyntax
{
public:
    SPSRangeExtension();
    virtual ~SPSRangeExtension();

    virtual bool Parse(BitStream* bit_stream) override;
};

#endif
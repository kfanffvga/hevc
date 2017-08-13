#ifndef _PPS_RANGE_EXTENSION_H_
#define _PPS_RANGE_EXTENSION_H_

#include "hevc_decoder/syntax/base_syntax.h"

class PPSRangeExtension : public BaseSyntax
{
public:
    PPSRangeExtension(bool is_transform_skip_enabled);
    virtual ~PPSRangeExtension();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    bool is_transform_skip_enabled_;
};

#endif
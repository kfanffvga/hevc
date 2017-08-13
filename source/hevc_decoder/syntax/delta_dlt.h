#ifndef _DELTA_DLT_H_
#define _DELTA_DLT_H_

#include <stdint.h>

#include "hevc_decoder/syntax/base_syntax.h"

class DeltaDlt : public BaseSyntax
{
public:
    DeltaDlt(uint32_t depth_max_value);
    virtual ~DeltaDlt();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    uint32_t depth_max_value_;
};

#endif
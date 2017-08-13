#ifndef _PPS_SCREEN_CONTENT_CODING_EXTENSION_H_
#define _PPS_SCREEN_CONTENT_CODING_EXTENSION_H_

#include "hevc_decoder/syntax/base_syntax.h"

class PPSScreenContentCodingExtension : public BaseSyntax
{
public:
    PPSScreenContentCodingExtension();
    virtual ~PPSScreenContentCodingExtension();

    virtual bool Parse(BitStream* bit_stream) override;
};

#endif
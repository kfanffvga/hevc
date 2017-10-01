#ifndef _SPS_MULTILAYER_EXTENSION_H_
#define _SPS_MULTILAYER_EXTENSION_H_

#include "hevc_decoder/syntax/base_syntax.h"

class SPSMultilayerExtension : public BaseSyntax
{
public:
    SPSMultilayerExtension();
    virtual ~SPSMultilayerExtension();

    virtual bool Parse(BitStream* bit_stream) override;
};

#endif
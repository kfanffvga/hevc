#ifndef _PPS_3D_EXTENSION_H_
#define _PPS_3D_EXTENSION_H_

#include "hevc_decoder/syntax/base_syntax.h"

class PPS3DExtension : public BaseSyntax
{
public:
    PPS3DExtension();
    virtual ~PPS3DExtension();

    virtual bool Parse(BitStream* bit_stream) override;
};

#endif
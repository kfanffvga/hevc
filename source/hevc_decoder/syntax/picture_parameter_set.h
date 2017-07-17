#ifndef _PICTURE_PARAMETER_SET_H_
#define _PICTURE_PARAMETER_SET_H_

#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;

class PictureParameterSet : public BaseSyntax
{
    PictureParameterSet();
    virtual ~PictureParameterSet();

    virtual bool Parse(BitStream* bit_stream) override;
};
#endif
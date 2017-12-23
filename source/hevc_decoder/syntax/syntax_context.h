#ifndef _SYNTAX_CONTEXT_H_
#define _SYNTAX_CONTEXT_H_

#include <inttypes.h>

#include "hevc_decoder/syntax/nal_unit_types.h"

class IBaseSyntaxContext
{
public: 
    virtual uint8_t GetNuhLayerID() const = 0;
    virtual NalUnitType GetNalUnitType() const = 0;
};
#endif
#ifndef _FRAME_SYNTAX_CONTEXT_H_
#define _FRAME_SYNTAX_CONTEXT_H_

#include <inttypes.h>

struct PictureOrderCount;

class IFrameSyntaxContext
{
public:
    virtual uint8_t GetLayerID(uint32_t poc_value) const = 0;
    virtual bool GetPreviewPictureOrderCount(PictureOrderCount* poc) const = 0;
};

#endif
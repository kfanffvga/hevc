#ifndef _FRAME_H_
#define _FRAME_H_

#include "hevc_decoder/base/basic_types.h"

enum ColorSpace
{
    COLOR_SPACE_YV12
};

class Frame
{
public:
    Frame();
    ~Frame();

    const uint8* GetY();
    const uint8* GetU();
    const uint8* GetV();
    ColorSpace GetColorSpace();
    int GetWidth();
    int GetHeight();
    int GetFrameNumber();


};

#endif
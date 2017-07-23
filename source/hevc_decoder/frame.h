#ifndef _FRAME_H_
#define _FRAME_H_

#include <stdint.h>

enum ColorSpace
{
    COLOR_SPACE_YV12
};

class Frame
{
public:
    Frame();
    ~Frame();

    const uint8_t* GetY();
    const uint8_t* GetU();
    const uint8_t* GetV();
    ColorSpace GetColorSpace();
    int GetWidth();
    int GetHeight();
    int GetFrameNumber();


};

#endif
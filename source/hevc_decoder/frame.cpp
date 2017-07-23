#include "hevc_decoder/frame.h"

Frame::Frame()
{

}

Frame::~Frame()
{

}

const uint8_t* Frame::GetY()
{
    return nullptr;
}

const uint8_t* Frame::GetU()
{
    return nullptr;
}

const uint8_t* Frame::GetV()
{
    return nullptr;
}

ColorSpace Frame::GetColorSpace()
{
    return COLOR_SPACE_YV12;
}

int Frame::GetWidth()
{
    return 0;
}

int Frame::GetHeight()
{
    return 0;
}

int Frame::GetFrameNumber()
{
    return 0;
}
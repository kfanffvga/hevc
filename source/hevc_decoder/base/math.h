#ifndef _MATH_H_
#define _MATH_H_

#include "hevc_decoder/base/basic_types.h"

template<typename T> inline T UpAlign(T a, uint32 b)
{
    return (a + (1 << b) - 1) >> b << b;
}

#endif
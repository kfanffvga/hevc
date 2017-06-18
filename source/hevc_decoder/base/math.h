#ifndef _MATH_H_
#define _MATH_H_

#include <cassert>
#include <algorithm>

#include "hevc_decoder/base/basic_types.h"

template<typename T> inline T UpAlign(T a, uint32 b)
{
    return (a + (1 << b) - 1) >> b << b;
}

template<typename T> inline T Clip3(T x, T y, T z)
{
    assert(x < y);
    return std::max(x, std::min(z, y));
}

#endif
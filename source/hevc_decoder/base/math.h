#ifndef _MATH_H_
#define _MATH_H_

#include <cassert>
#include <algorithm>
#include <stdint.h>

template<typename T> inline T UpAlign(T a, uint32_t b)
{
    return (a + (1 << b) - 1) >> b << b;
}

template<typename T> inline T Clip3(T x, T y, T z)
{
    assert(x < y);
    return std::max(x, std::min(z, y));
}

inline uint32_t Log2(uint32_t value)
{
    unsigned long index = 0;
    uint8_t r = _BitScanReverse(&index, static_cast<unsigned long>(value));
    return r > 0 ? static_cast<uint32_t>(index) : 0;
}

inline uint32_t CeilLog2(uint32_t value)
{
    uint32_t r = Log2(value);
    return (value - (1 << r) > 0) ? r + 1 : r;
}

inline uint32_t UpAlignRightShift(uint32_t value, uint32_t shift_bits)
{
    return static_cast<uint32_t>(-(-static_cast<int64_t>(value) >> shift_bits));
}

inline uint32_t UpAlignDiv(uint32_t dividend, uint32_t divisor)
{
    if (0 == divisor)
        return 0;

    return (dividend + divisor - 1) / divisor;
}

#endif
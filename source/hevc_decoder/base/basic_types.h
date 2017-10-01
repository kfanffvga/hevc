#ifndef _BASIC_TYPES_H_
#define _BASIC_TYPES_H_

#include <stdint.h>

struct Coordinate
{
    uint32_t x;
    uint32_t y;
};

struct Rect
{
    uint32_t left;
    uint32_t right;
    uint32_t top;
    uint32_t bottom;
};

#pragma pack(push, 1)
struct int24
{
    unsigned char a;
    unsigned char b;
    char c;

    bool operator==(const int24& r) const
    {
        return (a == r.a) && (b == r.b) && (c == r.c);
    }
};
#pragma pack(pop)

inline int int24_to_int(int24 in)
{
    if (in.c < 0)
    {
        return in.a | (in.b << 8) | (in.c << 16) | 0xff000000;
    }
    else
    {
        return in.a | (in.b << 8) | (in.c << 16);
    }
}

inline int24 int_to_int24(int in)
{
    int24 out;
    out.a = in;
    out.b = in >> 8;
    out.c = in >> 16;
    return out;
}

#endif
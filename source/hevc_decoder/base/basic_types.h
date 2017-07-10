#ifndef _BASIC_TYPES_H_
#define _BASIC_TYPES_H_

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef int int32;
typedef unsigned int uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

struct Coordinate
{
    uint32 x;
    uint32 y;
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
#ifndef _BASIC_TYPES_H_
#define _BASIC_TYPES_H_

#include <stdint.h>

enum SliceType
{
    B_SLICE = 0,
    P_SLICE = 1,
    I_SLICE = 2
};

enum ChromaFormatType
{
    MONO_CHROME = 0,
    YUV_420 = 1,
    YUV_422 = 2,
    YUV_444 = 3,
    YUV_MONO_CHROME = 0xffff   // 此处可以认为chroma_array_type为0
};

enum PredModeType
{
    MODE_INTER = 0,
    MODE_INTRA = 1,
    MODE_SKIP = 2
};

enum PartModeType
{
    PART_2Nx2N = 0,
    PART_2NxN = 1,
    PART_Nx2N = 2,
    PART_NxN = 3,
    PART_2NxnU = 4,
    PART_2NxnD = 5,
    PART_nLx2N = 6, 
    PART_nRx2N = 7
};

// 坐标的表示是与笛卡尔直角坐标系一致，由于二维数组的下标是[行][列]，因此在使用时候要注意
struct Coordinate
{
    uint32_t x;
    uint32_t y;
};

struct PictureOrderCount
{
    uint32_t msb;
    uint32_t lsb;
    uint32_t value;
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
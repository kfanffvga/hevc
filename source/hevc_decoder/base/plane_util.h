#ifndef _PLANE_UTIL_H_
#define _PLANE_UTIL_H_

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/coordinate.h"

inline bool IsPointInRect(const Coordinate& point,
                          const Coordinate& left_top_point, uint32_t width,
                          uint32_t height)
{
    if ((point.GetX() < left_top_point.GetX()) || 
        (point.GetY() < left_top_point.GetY()))
        return false;

    if ((point.GetX() > left_top_point.GetX() + width) || 
        (point.GetY() > left_top_point.GetY() + height))
        return false;

    return true;
}

inline bool IsPointInSquare(const Coordinate& point,
                            const Coordinate& left_top_point,
                            uint32_t square_length)
{
    return IsPointInRect(point, left_top_point, square_length, square_length);
}

inline uint32_t GetQuadrant(const Coordinate& point, uint32_t size)
{
    if ((size <= 0) || ((point.GetX() > size) || (point.GetY() > size)))
        return static_cast<uint32_t>(-1);

    uint32_t half_size = size >> 1;
    return (static_cast<uint32_t>(point.GetY() > half_size) << 1) | 
        static_cast<uint32_t>(point.GetX() > half_size);
}

#endif

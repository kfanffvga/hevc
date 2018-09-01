#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include <stdint.h>

// 坐标的表示是与笛卡尔直角坐标系一致，由于二维数组的下标是[行][列]，因此在使用时候要注意
// TODO:由于历史原因，该类没有支持负坐标系，在vvc中要修正

class Coordinate
{
public:
    Coordinate(uint32_t x, uint32_t y);
    Coordinate(const Coordinate& c);
    Coordinate();

    ~Coordinate();

    bool Equal(const Coordinate& c) const;
    uint32_t GetX() const;
    uint32_t GetY() const;
    void SetX(uint32_t x);
    void SetY(uint32_t y);
    void Assign(const Coordinate& c);
    void OffsetX(int32_t len);
    void OffsetY(int32_t len);

    bool operator < (const Coordinate& c) const;
    bool operator != (const Coordinate& c) const;
	bool operator == (const Coordinate& c) const;

private:
    uint32_t x_;
    uint32_t y_;
};

#endif

#include "hevc_decoder/base/coordinate.h"

Coordinate::Coordinate(uint32_t x, uint32_t y)
    : x_(x)
    , y_(y)
{

}

Coordinate::Coordinate()
    : x_(0)
    , y_(0)
{

}

Coordinate::Coordinate(const Coordinate& c)
    : x_(c.GetX())
    , y_(c.GetY())
{

}

Coordinate::~Coordinate()
{

}

bool Coordinate::Equal(const Coordinate& c) const
{
    return (c.GetX() == x_) && (c.GetY() == y_);
}

uint32_t Coordinate::GetX() const
{
    return x_;
}

uint32_t Coordinate::GetY() const
{
    return y_;
}

void Coordinate::SetX(uint32_t x)
{
    x_ = x;
}

void Coordinate::SetY(uint32_t y)
{
    y_ = y;
}

void Coordinate::Assign(const Coordinate& c)
{
    SetX(c.GetX());
    SetY(c.GetY());
}

void Coordinate::OffsetX(int32_t len)
{
    x_ += len;
}

void Coordinate::OffsetY(int32_t len)
{
    y_ += len;
}

bool Coordinate::operator == (const Coordinate& c) const
{
	return (c.GetX() == x_) && (c.GetY() == y_);
}

bool Coordinate::operator != (const Coordinate& c) const
{
    return (c.GetX() != x_) || (c.GetY() != y_);
}

bool Coordinate::operator<(const Coordinate& c) const
{
    return c.GetY() == y_ ? x_ < c.GetX() : y_ < c.GetY();
}

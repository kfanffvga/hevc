#include "hevc_decoder/syntax/prediction_unit.h"

PredictionUnit::PredictionUnit(const Coordinate& point, uint32_t width, 
                               uint32_t height)
    : point_(point)
    , width_(width)
    , height_(height)
{

}

PredictionUnit::~PredictionUnit()
{

}

bool PredictionUnit::Parse(CABACReader* cabac_reader)
{
    return false;
}

bool PredictionUnit::IsMergeMode() const
{
    return false;
}

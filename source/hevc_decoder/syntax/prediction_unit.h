#ifndef _PREDICTION_UNIT_H_
#define _PREDICTION_UNIT_H_

#include <stdint.h>

#include "hevc_decoder/base/basic_types.h"

class CABACReader;
struct Coordinate;
class IPredictionUnitContext;

class PredictionUnit
{
public:
    PredictionUnit(const Coordinate& point, uint32_t width, uint32_t height);
    ~PredictionUnit();

    bool Parse(CABACReader* cabac_reader, IPredictionUnitContext* context);

    bool IsMergeMode() const;

private:
    Coordinate point_;
    uint32_t width_;
    uint32_t height_;
};
#endif

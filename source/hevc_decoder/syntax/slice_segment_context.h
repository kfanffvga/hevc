#ifndef _SLICE_SEGMENT_CONTEXT_H_
#define _SLICE_SEGMENT_CONTEXT_H_

#include <inttypes.h>

struct PictureOrderCount;

class ISliceSegmentContext
{
public:
    virtual void SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb) = 0;
    virtual PictureOrderCount GetPictureOrderCount() const = 0;
    virtual uint8_t GetNuhLayerIDByPOCValue(uint32_t poc_value) const = 0;
};
#endif
#ifndef _SLICE_SEGMENT_HEADER_CONTEXT_H_
#define _SLICE_SEGMENT_HEADER_CONTEXT_H_

#include <inttypes.h>

struct PictureOrderCount;

class ISliceSegmentHeaderContext
{
public:
    virtual void SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb) = 0;
    virtual PictureOrderCount GetPictureOrderCount() const = 0;
    virtual NalUnitType GetNalUnitType() const = 0;
    virtual uint8_t GetNuhLayerID() const = 0;
    virtual uint8_t GetNuhLayerIDByPOCValue(uint32_t poc_value) const = 0;
};

#endif
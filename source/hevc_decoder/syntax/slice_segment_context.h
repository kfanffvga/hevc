#ifndef _SLICE_SEGMENT_CONTEXT_H_
#define _SLICE_SEGMENT_CONTEXT_H_

#include <stdint.h>

struct PictureOrderCount;
class SliceSegmentHeader;
class CABACContextStorage;
class FramePartition;

class ISliceSegmentContext
{
public:
    virtual void SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb) = 0;
    virtual PictureOrderCount GetPictureOrderCount() const = 0;
    virtual uint8_t GetNuhLayerIDByPOCValue(uint32_t poc_value) const = 0;
    virtual bool OnSliceSegmentHeaderParsed(const SliceSegmentHeader& header) 
        = 0;

    virtual std::shared_ptr<FramePartition> GetFramePartition() = 0;
    virtual CABACContextStorage* GetCABACContextStorage() = 0;
    virtual bool GetSliceSegmentAddressByTileScanIndex(
        uint32_t tile_scan_index, uint32_t* slice_segment_address) = 0;

    virtual uint32_t GetFirstCTUIndexOfTileScan() = 0;
    virtual uint32_t GetCABACContextIndexInLastParsedSliceSegment() = 0;
};
#endif
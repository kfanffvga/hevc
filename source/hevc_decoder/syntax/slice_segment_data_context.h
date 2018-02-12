#ifndef _SLICE_SEGMENT_DATA_CONTEXT_H_
#define _SLICE_SEGMENT_DATA_CONTEXT_H_

#include <memory>

class FramePartition;
class CABACContextStorage;
class SliceSegmentHeader;

class ISliceSegmentDataContext
{
public:
    virtual void OnCodedTreeUnitParsed() = 0;
    virtual std::shared_ptr<FramePartition> GetFramePartition() = 0;
    virtual CABACContextStorage* GetCABACContextStorage() = 0;
    virtual bool GetSliceSegmentAddressByTileScanIndex(
        uint32_t tile_scan_index, uint32_t* slice_segment_address) = 0;

    virtual uint32_t GetFirstCTUIndexOfTileScan() = 0;
    virtual uint32_t GetCABACContextIndexInPriorSliceSegment() = 0;
    virtual SliceSegmentHeader* GetSliceSegmentHeader() = 0;
};
#endif

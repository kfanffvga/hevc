#ifndef _SLICE_SEGMENT_DATA_CONTEXT_H_
#define _SLICE_SEGMENT_DATA_CONTEXT_H_

#include <memory>
#include <vector>

class FramePartition;
class CABACContextStorage;
class SliceSegmentHeader;
class PaletteTable;
class Coordinate;

class ISliceSegmentDataContext
{
public:
    virtual void OnCodedTreeUnitParsed() = 0;
    virtual std::shared_ptr<FramePartition> GetFramePartition() = 0;
    virtual CABACContextStorage* GetCABACContextStorage() = 0;
    virtual uint32_t GetFirstCTUIndexOfTileScan() = 0;
    virtual uint32_t GetCABACContextIndexInPriorSliceSegment() = 0;
    virtual SliceSegmentHeader* GetSliceSegmentHeader() = 0;
    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, const Coordinate& neighbouring_block)
        const = 0;

    virtual std::shared_ptr<PaletteTable> GetPredictorPaletteTable(
        bool is_the_first_ctu_in_slice_segment, const Coordinate& point) = 0;

    virtual void SavePredictorPaletteTable(
        const Coordinate& point, 
        const std::shared_ptr<PaletteTable>& palette_table) = 0;

    virtual const std::shared_ptr<CodingTreeUnit> GetCodingTreeUnit(
        uint32_t tile_scan_index) const = 0;
};
#endif

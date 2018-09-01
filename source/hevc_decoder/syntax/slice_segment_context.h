#ifndef _SLICE_SEGMENT_CONTEXT_H_
#define _SLICE_SEGMENT_CONTEXT_H_

#include <stdint.h>

struct PictureOrderCount;
class SliceSegmentHeader;
class CABACContextStorage;
class CodingTreeUnit;
class FramePartition;
class SliceSegmentSyntax;
class Coordinate;

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
    virtual const ParametersManager& GetParametersManager() const = 0;
    virtual NalUnitType GetNalUnitType() const = 0;
    virtual uint8_t GetNuhLayerID() const = 0;
    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, const Coordinate& neighbouring_block)
        const = 0;

    virtual std::shared_ptr<PaletteTable> GetPredictorPaletteTable(
        bool is_the_first_ctu_in_slice_segment, uint32_t ctb_size_y,
        bool is_entropy_coding_sync_enabled, bool is_dependent_slice_segment,
        const Coordinate& point) = 0;

    virtual void SavePredictorPaletteTable(
        const Coordinate& point,
        const std::shared_ptr<PaletteTable>& palette_table) = 0;

    virtual const std::shared_ptr<SliceSegmentSyntax> GetSliceSegmentSyntax(
        uint32_t tile_scan_index) const = 0;
};
#endif
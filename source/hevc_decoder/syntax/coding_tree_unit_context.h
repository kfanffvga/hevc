#ifndef _CODING_TREE_UNIT_CONTEXT_H_
#define _CODING_TREE_UNIT_CONTEXT_H_

#include <memory>

class FramePartition;
enum CABACInitType;
class CodingTreeUnit;
class PaletteTable;

class ICodingTreeUnitContext
{
public:
    virtual std::shared_ptr<FramePartition> GetFramePartition() = 0;
    virtual bool IsSliceSAOLuma() const = 0;
    virtual bool IsSliceSAOChroma() const = 0;
    virtual uint32_t GetSliceSegmentAddress() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual ChromaFormatType GetChromaFormatType() const = 0;
    virtual uint32_t GetBitDepthLuma() const = 0;
    virtual uint32_t GetBitDepthChroma() const = 0;
    virtual uint32_t GetFrameHeightInLumaSamples() const = 0;
    virtual uint32_t GetFrameWidthInLumaSamples() const = 0;
    virtual uint32_t GetMinCBLog2SizeY() const = 0;
    virtual uint32_t GetMinCBSizeY() const = 0;
    virtual bool IsCUQPDeltaEnabled() const = 0;
    virtual uint32_t GetLog2MinCUQPDeltaSize() const = 0;
    virtual bool IsCUChromaQPOffsetEnabled() const = 0;
    virtual uint32_t GetLog2MinCUChromaQPOffsetSize() const = 0;
    virtual bool IsTransquantBypassEnabled() const = 0;
    virtual SliceType GetSliceType() const = 0;
    virtual bool IsPaletteModeEnabled() const = 0;
    virtual uint32_t GetMaxTransformBlockSizeY() const = 0;
    virtual uint32_t GetMinPCMCodingBlockSizeY() const = 0;
    virtual uint32_t GetMaxPCMCodingBlockSizeY() const = 0;
    virtual bool IsAsymmetricMotionPartitionsEnabled() const = 0;
    virtual bool IsPCMEnabled() const = 0;
    virtual uint32_t GetMaxTransformHierarchyDepthIntra() const = 0;
    virtual uint32_t GetMaxTransformHierarchyDepthInter() const = 0;
    virtual bool IsNeighbourBlockAvailable(
        const Coordinate& current, const Coordinate& neighbour) const = 0;

    // 同一个slice_segment和tile之内
    virtual CodingTreeUnit* GetLeftNeighbourCTU() const = 0;
    virtual CodingTreeUnit* GetUpNeighbourCTU() const = 0;
    virtual std::shared_ptr<PaletteTable> GetPredictorPaletteTable(
        const Coordinate& point) = 0;
    
    virtual void SavePredictorPaletteTable(
        const Coordinate& point, 
        const std::shared_ptr<PaletteTable>& palette_table) = 0;

    virtual uint32_t GetPaletteMaxSize() const = 0;
    virtual uint32_t GetPredictorPaletteMaxSize() const = 0;
    virtual uint32_t GetChromaQPOffsetListtLen() const = 0;
    virtual const std::vector<int32_t>& GetCbQPOffsetList() const = 0;
    virtual const std::vector<int32_t>& GetCrQPOffsetList() const = 0;
};

#endif

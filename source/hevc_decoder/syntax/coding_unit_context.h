#ifndef _CODING_UNIT_CONTEXT_H_
#define _CODING_UNIT_CONTEXT_H_

#include <memory>

enum CABACInitType;
enum SliceType;
enum ChromaFormatType;
class PaletteTable;

class ICodingUnitContext
{
public:
    virtual bool IsTransquantBypassEnabled() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual SliceType GetSliceType() const = 0;
    virtual bool IsPaletteModeEnabled() const = 0;
    virtual uint32_t GetMaxTransformBlockSizeY() const = 0;
    virtual uint32_t GetMinTransformBlockSizeY() const = 0;
    virtual uint32_t GetMinCBSizeY() const = 0;
    virtual uint32_t GetMinCBLog2SizeY() const = 0;
    virtual uint32_t GetMinPCMCodingBlockSizeY() const = 0;
    virtual uint32_t GetMaxPCMCodingBlockSizeY() const = 0;
    virtual bool IsAsymmetricMotionPartitionsEnabled() const = 0;
    virtual bool IsPCMEnabled() const = 0;
    virtual ChromaFormatType GetChromaFormatType() const = 0;
    virtual uint32_t GetMaxTransformHierarchyDepthIntra() const = 0;
    virtual uint32_t GetMaxTransformHierarchyDepthInter() const = 0;
    virtual bool IsNeighbourBlockAvailable(
        const Coordinate& current, const Coordinate& neighbour) const = 0;

    virtual uint32_t GetNearestCULayerByCoordinate(const Coordinate& point)
        const = 0;

    virtual std::shared_ptr<PaletteTable> GetPredictorPaletteTable() const = 0;
    virtual uint32_t GetPaletteMaxSize() const = 0;
    virtual uint32_t GetBitDepthOfLuma() const = 0;
    virtual uint32_t GetBitDepthOfChroma() const = 0;
    virtual uint32_t GetPredictorPaletteMaxSize() const = 0;
    virtual bool IsCUQPDeltaEnabled() const = 0;
    virtual bool IsCUQPDeltaCoded() const = 0;
    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) = 0;
    virtual bool IsCUChromaQPOffsetEnable() const = 0;
    virtual bool IsCUChromaQPOffsetCoded() const = 0;
    virtual uint32_t GetChromaQPOffsetListtLen() const = 0;
    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index) 
        = 0;

    virtual bool IsResidualAdaptiveColorTransformEnabled() const = 0;
    virtual bool IsCrossComponentPredictionEnabled() const = 0;
    virtual bool IsTransformSkipEnabled() const = 0;
    virtual uint32_t GetMaxTransformSkipSize() const = 0;
    virtual bool IsExplicitRDPCMEnabled() const = 0;

    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, 
        const Coordinate& neighbouring_block) = 0;

    virtual std::shared_ptr<CodingUnit> GetCodingUnit(const Coordinate& p) const 
        = 0;

    virtual bool IsTransformSkipContextEnabled() const = 0;
    virtual bool IsImplicitRDPCMEnabled() const = 0;
    virtual bool IsCABACBypassAlignmentEnabled() const = 0;
    virtual bool IsSignDataHidingEnabled() const = 0;
    virtual bool IsPersistentRiceAdaptationEnabled() const = 0;
    virtual bool HasExtendedPrecisionProcessing() const = 0;
};

#endif

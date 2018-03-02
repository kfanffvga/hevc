#ifndef _CODING_UNIT_CONTEXT_H_
#define _CODING_UNIT_CONTEXT_H_

enum CABACInitType;
enum SliceType;
enum ChromaFormatType;

class ICodingUnitContext
{
public:
    virtual bool IsTransquantBypassEnabled() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual SliceType GetSliceType() const = 0;
    virtual bool IsPaletteModeEnabled() const = 0;
    virtual uint32_t GetMaxTransformBlockSizeY() const = 0;
    virtual uint32_t GetMinCBSizeY() const = 0;
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
};

#endif

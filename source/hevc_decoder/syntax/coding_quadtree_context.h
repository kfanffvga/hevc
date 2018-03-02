﻿#ifndef _CODING_QUADTREE_CONTEXT_H_
#define _CODING_QUADTREE_CONTEXT_H_

#include <stdint.h>

// 此处的QP不是表示码率控制的量化值，而是把一个ctb量化成多少个cb

enum CABACInitType;

class ICodingQuadtreeContext
{
public:
    virtual uint32_t GetFrameWidthInLumaSamples() const = 0;
    virtual uint32_t GetFrameHeightInLumaSamples() const = 0;
    virtual uint32_t GetMinCBLog2SizeY() const = 0;
    virtual uint32_t GetMinCBSizeY() const = 0;
    virtual bool IsCUQPDeltaEnabled() const = 0;
    virtual uint32_t GetLog2MinCUQPDeltaSize() const = 0;
    virtual bool IsCUChromaQPOffsetEnabled() const = 0;
    virtual uint32_t GetLog2MinCUChromaQPOffsetSize() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual bool IsTransquantBypassEnabled() const = 0;
    virtual SliceType GetSliceType() const = 0;
    virtual bool IsPaletteModeEnabled() const = 0;
    virtual uint32_t GetMaxTransformBlockSizeY() const = 0;
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

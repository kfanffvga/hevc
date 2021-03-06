﻿#ifndef _FRAME_SYNTAX_CONTEXT_H_
#define _FRAME_SYNTAX_CONTEXT_H_

struct Coordinate;
class ICodingTreeBlockContext;
class ISliceSegmentContext;

class IFrameSyntaxContext
{
public:
    virtual uint32_t GetSliceAddressByRasterScanBlockIndex(uint32_t index) 
        const = 0;

    virtual bool IsTheFirstBlockInTile(const Coordinate& block) const = 0;
    virtual bool IsTheFirstBlockInRowOfTile(const Coordinate& block) const = 0;
    virtual bool IsTheFirstBlockInRowOfFrame(const Coordinate& block) const = 0;
    virtual uint32_t GetCTBHeight() const = 0;
    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, 
        const Coordinate& neighbouring_block) const = 0;

    virtual const ICodingTreeBlockContext* GetCodingTreeBlockContext(
        const Coordinate& block) const = 0;

    virtual const ISliceSegmentContext* GetIndependentSliceSegmentContext(
        uint32_t slice_segment_address) const = 0;
};

#endif
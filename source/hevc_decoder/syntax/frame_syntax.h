#ifndef _FRAME_SYNTAX_H_
#define _FRAME_SYNTAX_H_

#include <memory>
#include <vector>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/frame_syntax_context.h"

class SliceSyntax;
class SliceSegmentSyntax;
class ISliceSegmentContext;

class FrameSyntax : public IFrameSyntaxContext
{
public:
    FrameSyntax();
    virtual ~FrameSyntax();

    bool AddSliceSegment(std::unique_ptr<SliceSegmentSyntax> slice_segment);

private:
    virtual uint32 GetSliceAddressByRasterScanBlockIndex(uint32 index) const
        override;
    
    virtual bool IsTheFirstBlockInTile(const Coordinate& block) const override;
    virtual bool IsTheFirstBlockInRowOfTile(const Coordinate& block) const 
        override;

    virtual bool IsTheFirstBlockInRowOfFrame(const Coordinate& block) const 
        override;

    virtual uint32 GetCTBHeight() const override;
    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, 
        const Coordinate& neighbouring_block) const override;

    virtual const ICodingTreeBlockContext* GetCodingTreeBlockContext(
        const Coordinate& block) const override;

    virtual const ISliceSegmentContext* GetIndependentSliceSegmentContext(
        uint32 slice_segment_address) const override;

    std::vector<std::unique_ptr<SliceSyntax>> slices_;
};
#endif
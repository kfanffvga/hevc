#include "hevc_decoder/syntax/frame_syntax.h"

#include "hevc_decoder/syntax/slice_segment_syntax.h"
#include "hevc_decoder/syntax/slice_syntax.h"

using std::unique_ptr;

FrameSyntax::FrameSyntax()
{

}

FrameSyntax::~FrameSyntax()
{

}

uint32 FrameSyntax::GetSliceAddressByRasterScanBlockIndex(uint32 index) const
{
    return 0;
}

bool FrameSyntax::IsTheFirstBlockInTile(const Coordinate& block) const
{
    return false;
}

bool FrameSyntax::IsTheFirstBlockInRowOfTile(const Coordinate& block) const
{
    return false;
}

bool FrameSyntax::IsTheFirstBlockInRowOfFrame(const Coordinate& block) const
{
    return false;
}

uint32 FrameSyntax::GetCTBHeight() const
{
    return 0;
}

bool FrameSyntax::IsZScanOrderNeighbouringBlockAvailable(
    const Coordinate& current_block, const Coordinate& neighbouring_block) const
{
    return false;
}

const ICodingTreeBlockContext* FrameSyntax::GetCodingTreeBlockContext(
    const Coordinate& block) const
{
    return nullptr;
}

const ISliceSegmentContext* FrameSyntax::GetIndependentSliceSegmentContext(
    uint32 slice_segment_address) const
{
    return nullptr;
}

bool FrameSyntax::AddSliceSegment(unique_ptr<SliceSegmentSyntax> slice_segment)
{
    return false;
}

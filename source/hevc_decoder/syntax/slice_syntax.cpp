#include "hevc_decoder/syntax/slice_syntax.h"

#include <cassert>

#include "hevc_decoder/syntax/slice_segment_syntax.h"
#include "hevc_decoder/syntax/slice_segment_header.h"
#include "hevc_decoder/syntax/slice_segment_data.h"

using std::shared_ptr;

SliceSyntax::SliceSyntax(IFrameSyntaxContext* context)
{

}

SliceSyntax::~SliceSyntax()
{

}

bool SliceSyntax::AddSliceSegment(shared_ptr<SliceSegmentSyntax> slice_segment)
{
    if (!slice_segments_.empty() && 
        !slice_segment->GetSliceSegmentHeader().IsDependentSliceSegment())
        return false;

    uint32_t address = 
        slice_segment->GetSliceSegmentHeader().GetSliceSegmentAddress();
    for (auto i = slice_segments_.begin(); i != slice_segments_.end(); ++i)
    {
        if ((*i)->GetSliceSegmentHeader().GetSliceSegmentAddress() > address)
        {
            slice_segments_.insert(i, slice_segment);
            return true;
        }
    }
    slice_segments_.push_back(slice_segment);
    return true;
}

bool SliceSyntax::GetSliceSegmentAddressByCTUTileScanIndex(uint32_t index,
                                                           uint32_t* address)
{
    if (!address)
        return false;

    for (const auto& slice_segment : slice_segments_)
    {
        if (slice_segment->GetSliceSegmentData().IsInnerCTUByTileScanIndex(index))
        {
            *address = 
                slice_segment->GetSliceSegmentHeader().GetSliceSegmentAddress();
            return true;
        }
    }
    return false;
}

const shared_ptr<SliceSegmentSyntax> SliceSyntax::GetSliceSegmentSyntax(
    uint32_t tile_scan_index) const
{
    for (const auto& slice_segment : slice_segments_)
    {
        const SliceSegmentData& slice_segment_data = 
            slice_segment->GetSliceSegmentData();

        if (slice_segment_data.GetCodingTreeUnit(tile_scan_index))
            return slice_segment;
    }
    return shared_ptr<SliceSegmentSyntax>();
}

uint32_t SliceSyntax::GetCABACContextIndexInLastParsedSliceSegment()
{
    assert(!slice_segments_.empty());
    if (slice_segments_.empty())
        return 0;

    const SliceSegmentData& slice_segment_data = 
        slice_segments_.back()->GetSliceSegmentData();
    return slice_segment_data.GetCABACContextStorageIndex();
}


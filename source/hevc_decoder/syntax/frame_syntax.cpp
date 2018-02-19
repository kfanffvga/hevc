#include "hevc_decoder/syntax/frame_syntax.h"

#include <cassert>

#include "hevc_decoder/syntax/slice_segment_syntax.h"
#include "hevc_decoder/syntax/slice_segment_header.h"
#include "hevc_decoder/syntax/slice_syntax.h"
#include "hevc_decoder/syntax/coded_video_sequence.h"
#include "hevc_decoder/syntax/slice_segment_data.h"

using std::shared_ptr;
using std::unique_ptr;
using std::vector;

FrameSyntax::FrameSyntax(IFrameSyntaxContext* frame_syntax_context)
    : slices_()
    , picture_order_count_()
    , frame_syntax_context_(frame_syntax_context)
    , frame_partition_()
    , ctu_count_by_tile_scan_(0)
{

}

FrameSyntax::~FrameSyntax()
{

}

bool FrameSyntax::AddSliceSegment(shared_ptr<SliceSegmentSyntax> slice_segment)
{
    if (!slices_.empty() && slices_.back()->AddSliceSegment(slice_segment))
        return true;

    if (slice_segment->GetSliceSegmentHeader().IsDependentSliceSegment())
        return false;

    unique_ptr<SliceSyntax> slice(new SliceSyntax(frame_syntax_context_));
    if (!slice->AddSliceSegment(slice_segment))
        return false;

    ctu_count_by_tile_scan_ += 
        slice_segment->GetSliceSegmentData().GetCTUCount();
    slices_.push_back(move(slice));
    return true;
}

bool FrameSyntax::HasFramePartition() const
{
    return !!frame_partition_;
}

bool FrameSyntax::SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb)
{
    PictureOrderCount preview_poc;
    bool success = 
        frame_syntax_context_->GetPreviewPictureOrderCount(&preview_poc);
    if (!success)
        return false;

    picture_order_count_ = FrameSyntax::CalcPictureOrderCount(preview_poc.msb, 
                                                              preview_poc.lsb,
                                                              false, max_lsb, 
                                                              lsb);
    return true;
}

void FrameSyntax::SetFramePartition(
    const shared_ptr<FramePartition>& frame_partition)
{
    frame_partition_ = frame_partition;
}

const PictureOrderCount& FrameSyntax::GetPictureOrderCount() const
{
    return picture_order_count_;
}

shared_ptr<FramePartition> FrameSyntax::GetFramePartition()
{
    return frame_partition_;
}

bool FrameSyntax::GetSliceSegmentAddressByTileScanIndex(
    uint32_t tile_scan_index, uint32_t* address) const
{
    if (!address)
        return false;

    for (const auto& slice : slices_)
    {
        bool success = slice->GetSliceSegmentAddressByCTUTileScanIndex(
            tile_scan_index, address);
        if (success)
            return success;
    }
    return false;
}

uint32_t FrameSyntax::GetContainCTUCountByTileScan()
{
    return ctu_count_by_tile_scan_;
}

uint32_t FrameSyntax::GetCABACContextIndexInLastParsedSliceSegment()
{
    assert(!slices_.empty());
    if (slices_.empty())
        return 0;

    return slices_.back()->GetCABACContextIndexInLastParsedSliceSegment();
}

PictureOrderCount FrameSyntax::CalcPictureOrderCount(uint32_t previous_msb,
                                                     uint32_t previous_lsb, 
                                                     bool is_idr_frame, 
                                                     uint32_t max_lsb, 
                                                     uint32_t lsb)
{
    PictureOrderCount poc = { };
    if (is_idr_frame)
        return poc;

    poc.msb = previous_msb;
    poc.lsb = lsb;
    int delta_lsb = static_cast<int>(lsb) - static_cast<int>(previous_lsb);
    if (delta_lsb > static_cast<int>(max_lsb >> 1))
        poc.msb -= max_lsb;
    else if (delta_lsb <= -static_cast<int>(max_lsb >> 1))
        poc.msb += max_lsb;

    poc.value = poc.msb + poc.lsb;
    return poc;
}

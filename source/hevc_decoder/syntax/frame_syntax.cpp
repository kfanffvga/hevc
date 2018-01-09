#include "hevc_decoder/syntax/frame_syntax.h"

#include "hevc_decoder/syntax/slice_segment_syntax.h"
#include "hevc_decoder/syntax/slice_syntax.h"
#include "hevc_decoder/syntax/coded_video_sequence.h"

using std::unique_ptr;
using std::shared_ptr;
using std::vector;

FrameSyntax::FrameSyntax(IFrameSyntaxContext* frame_syntax_context)
    : slices_()
    , picture_order_count_()
    , frame_syntax_context_(frame_syntax_context)
    , frame_partition_()
{

}

FrameSyntax::~FrameSyntax()
{

}

uint32_t FrameSyntax::GetSliceAddressByRasterScanBlockIndex(uint32_t index) const
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

uint32_t FrameSyntax::GetCTBHeight() const
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

const ISliceSegmentInfoProviderForCABAC* 
    FrameSyntax::GetSliceSegmentInfoProviderForCABAC(
        uint32_t slice_segment_address) const
{
    return nullptr;
}

bool FrameSyntax::AddSliceSegment(unique_ptr<SliceSegmentSyntax> slice_segment)
{
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

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

bool FrameSyntax::AddSliceSegment(unique_ptr<SliceSegmentSyntax> slice_segment)
{
    return false;
}

#include "hevc_decoder/syntax/slice_segment_syntax.h"

SliceSegmentSyntax::SliceSegmentSyntax(
    IFrameSyntaxContext* frame_syntax_context)
    : frame_syntax_context_(frame_syntax_context)
{

}

SliceSegmentSyntax::~SliceSegmentSyntax()
{

}

uint32_t SliceSegmentSyntax::GetQuantizationParameter() const
{
    return 1;
}

bool SliceSegmentSyntax::IsEntropyCodingSyncEnabled() const
{
    return false;
}

bool SliceSegmentSyntax::IsDependentSliceSegment() const
{
    return false;
}

uint32_t SliceSegmentSyntax::GetSliceSegmentAddress() const
{
    return 0;
}

uint32_t SliceSegmentSyntax::GetCABACStorageIndex() const
{
    return 0;
}

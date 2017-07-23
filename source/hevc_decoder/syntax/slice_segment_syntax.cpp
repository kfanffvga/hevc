#include "hevc_decoder/syntax/slice_segment_syntax.h"

SliceSegmentSyntax::SliceSegmentSyntax()
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

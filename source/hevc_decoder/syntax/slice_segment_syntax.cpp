#include "hevc_decoder/syntax/slice_segment_syntax.h"

SliceSegmentSyntax::SliceSegmentSyntax()
{

}

SliceSegmentSyntax::~SliceSegmentSyntax()
{

}

uint32 SliceSegmentSyntax::GetQuantizationParameter() const
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

uint32 SliceSegmentSyntax::GetSliceSegmentAddress() const
{
    return 0;
}

uint32 SliceSegmentSyntax::GetCABACStorageIndex() const
{
    return 0;
}

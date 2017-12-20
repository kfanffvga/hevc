#include "hevc_decoder/syntax/slice_segment_syntax.h"

#include "hevc_decoder/syntax/slice_segment_header.h"

SliceSegmentSyntax::SliceSegmentSyntax(
    NalUnitType nal_unit_type, uint8_t nal_layer_id,
    const ParametersManager* parameters_manager,
    IFrameSyntaxContext* frame_syntax_context,
    ICodedVideoSequence* coded_video_sequence)
    : frame_syntax_context_(frame_syntax_context)
    , coded_video_sequence_(coded_video_sequence)
    , parameters_manager_(parameters_manager)
    , nal_unit_type_(nal_unit_type)
    , nal_layer_id_(nal_layer_id)
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

bool SliceSegmentSyntax::Parse(BitStream* bit_stream)
{
    SliceSegmentHeader header(nal_unit_type_, nal_layer_id_, 
                              parameters_manager_, frame_syntax_context_,
                              coded_video_sequence_);
    header.Parse(bit_stream);
    return true;
}

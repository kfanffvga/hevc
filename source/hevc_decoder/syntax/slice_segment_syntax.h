#ifndef _SLICE_SEGMENT_SYNTAX_H_
#define _SLICE_SEGMENT_SYNTAX_H_

#include "hevc_decoder/base/slice_segment_context.h"

class IFrameSyntaxContext;
class ParametersManager;
class ICodedVideoSequence;
class BitStream;
enum NalUnitType;

class SliceSegmentSyntax : public ISliceSegmentContext
{
public:
    SliceSegmentSyntax(NalUnitType nal_unit_type, uint8_t nal_layer_id,
                       const ParametersManager* parameters_manager,
                       IFrameSyntaxContext* frame_syntax_context,
                       ICodedVideoSequence* coded_video_sequence);
    ~SliceSegmentSyntax();

    bool Parse(BitStream* bit_stream);

private:
    virtual uint32_t GetQuantizationParameter() const override;
    virtual bool IsEntropyCodingSyncEnabled() const override;
    virtual bool IsDependentSliceSegment() const override;
    virtual uint32_t GetSliceSegmentAddress() const override;
    virtual uint32_t GetCABACStorageIndex() const override;

    IFrameSyntaxContext* frame_syntax_context_;
    ICodedVideoSequence* coded_video_sequence_;
    const ParametersManager* parameters_manager_;
    NalUnitType nal_unit_type_;
    uint8_t nal_layer_id_;
};

#endif
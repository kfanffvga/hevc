#ifndef _SLICE_SEGMENT_SYNTAX_H_
#define _SLICE_SEGMENT_SYNTAX_H_

#include "hevc_decoder/base/slice_segment_context.h"
#include "hevc_decoder/syntax/base_syntax.h"

class IFrameSyntaxContext;

class SliceSegmentSyntax : public ISliceSegmentContext
                         , public BaseSyntax
{
public:
    SliceSegmentSyntax(IFrameSyntaxContext* frame_syntax_context);
    virtual ~SliceSegmentSyntax();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    virtual uint32_t GetQuantizationParameter() const override;
    virtual bool IsEntropyCodingSyncEnabled() const override;
    virtual bool IsDependentSliceSegment() const override;
    virtual uint32_t GetSliceSegmentAddress() const override;
    virtual uint32_t GetCABACStorageIndex() const override;

    IFrameSyntaxContext* frame_syntax_context_;
};

#endif
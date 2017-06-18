#ifndef _SLICE_SEGMENT_SYNTAX_H_
#define _SLICE_SEGMENT_SYNTAX_H_

#include "hevc_decoder/base/slice_segment_context.h"

class SliceSegmentSyntax : public ISliceSegmentContext
{
public:
    SliceSegmentSyntax();
    virtual ~SliceSegmentSyntax();

private:
    virtual uint32 GetQuantizationParameter() const override;
    virtual bool IsEntropyCodingSyncEnabled() const override;
    virtual bool IsDependentSliceSegment() const override;
    virtual uint32 GetSliceSegmentAddress() const override;
    virtual uint32 GetCABACStorageIndex() const override;
};

#endif
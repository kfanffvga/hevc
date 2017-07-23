#ifndef _SLICE_SEGMENT_CONTEXT_H_
#define _SLICE_SEGMENT_CONTEXT_H_

#include "hevc_decoder/base/basic_types.h"

class ISliceSegmentContext
{
public:
    virtual uint32_t GetQuantizationParameter() const = 0;
    virtual bool IsEntropyCodingSyncEnabled() const = 0;
    virtual bool IsDependentSliceSegment() const = 0;
    virtual uint32_t GetSliceSegmentAddress() const = 0;
    virtual uint32_t GetCABACStorageIndex() const = 0;
};

#endif
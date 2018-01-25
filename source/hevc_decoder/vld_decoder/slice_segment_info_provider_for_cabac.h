#ifndef _SLICE_SEGMENT_INFO_PROVIDER_FOR_CABAC_H_
#define _SLICE_SEGMENT_INFO_PROVIDER_FOR_CABAC_H_

#include <inttypes.h>

class ISliceSegmentInfoProviderForCABAC
{
public:
    virtual uint32_t GetQuantizationParameter() const = 0;
    virtual bool IsEntropyCodingSyncEnabled() const = 0;
    virtual bool IsDependentSliceSegment() const = 0;
    virtual uint32_t GetSliceSegmentAddress() const = 0;
    virtual uint32_t GetCABACStorageIndex() const = 0;
};
#endif
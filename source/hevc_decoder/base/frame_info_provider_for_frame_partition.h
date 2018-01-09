#ifndef _FRAME_INFO_PROVIDER_FOR_FRAME_PARTITION_H_
#define _FRAME_INFO_PROVIDER_FOR_FRAME_PARTITION_H_

#include <inttypes.h>

class IFrameInfoProviderForFramePartition
{
public:
    virtual uint32_t GetSliceAddressByRasterScanBlockIndex(uint32_t index) 
        const = 0;
};
#endif
#ifndef _SLICE_SEGMENT_ADDRESS_PROVIDER_H_
#define _SLICE_SEGMENT_ADDRESS_PROVIDER_H_

#include <stdint.h>

struct Coordinate;

class ISliceSegmentAddressProvider
{
public:
    virtual bool GetSliceAddressByRasterScanIndex(uint32_t index, 
                                                  uint32_t* address) const = 0;

    virtual bool GetSliceAddressByTileScanIndex(uint32_t index, 
                                                uint32_t* address) const = 0;

    virtual bool GetSliceAddress(const Coordinate& coordinate, 
                                 uint32_t* address) const = 0;
};
#endif
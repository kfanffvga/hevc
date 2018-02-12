#ifndef _FRAME_INFO_PROVIDER_FOR_CABAC_H_
#define _FRAME_INFO_PROVIDER_FOR_CABAC_H_

#include <stdint.h>

struct Coordinate;
struct PictureOrderCount;

class IFrameInfoProviderForCABAC
{
public:
    virtual bool IsTheFirstBlockInTile(const Coordinate& block) const = 0;
    virtual bool IsTheFirstBlockInRowOfTile(const Coordinate& block) const = 0;
    virtual bool IsTheFirstBlockInRowOfFrame(const Coordinate& block) const = 0;
    virtual bool IsTheFirstBlockInSliceSegment(const Coordinate& block) const = 0;
    virtual uint32_t GetCTBHeight() const = 0;
    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, 
        const Coordinate& neighbouring_block) const = 0;

    virtual bool GetCABACContextIndexInCTU(
        const Coordinate& block, uint32_t* cabac_context_storage_index) const = 0;

    virtual uint32_t GetQuantizationParameter() const = 0;
    virtual bool IsEntropyCodingSyncEnabled() const = 0;
    virtual bool IsDependentSliceSegment() const = 0;
    virtual uint32_t GetCABACContextIndexInPriorSliceSegment() const = 0;
};

#endif
#ifndef _FRAME_INFO_PROVIDER_FOR_CABAC_H_
#define _FRAME_INFO_PROVIDER_FOR_CABAC_H_

#include <inttypes.h>

struct Coordinate;
class ISliceSegmentInfoProviderForCABAC;
class ICodingTreeBlockContext;
struct PictureOrderCount;

class IFrameInfoProviderForCABAC
{
public:
    virtual uint32_t GetSliceAddressByRasterScanBlockIndex(uint32_t index) 
        const = 0;

    virtual bool IsTheFirstBlockInTile(const Coordinate& block) const = 0;
    virtual bool IsTheFirstBlockInRowOfTile(const Coordinate& block) const = 0;
    virtual bool IsTheFirstBlockInRowOfFrame(const Coordinate& block) const = 0;
    virtual uint32_t GetCTBHeight() const = 0;
    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, 
        const Coordinate& neighbouring_block) const = 0;

    virtual const ICodingTreeBlockContext* GetCodingTreeBlockContext(
        const Coordinate& block) const = 0;

    virtual bool SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb) = 0;
    virtual const PictureOrderCount& GetPictureOrderCount() const = 0;

    virtual const ISliceSegmentInfoProviderForCABAC* 
        GetSliceSegmentInfoProviderForCABAC(uint32_t address) const = 0;
};

#endif
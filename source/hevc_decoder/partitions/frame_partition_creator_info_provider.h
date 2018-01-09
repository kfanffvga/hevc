#ifndef _FRAME_PARTITION_CREATOR_INFO_PROVIDER_H_
#define _FRAME_PARTITION_CREATOR_INFO_PROVIDER_H_

#include <inttypes.h>
#include <vector>

class IFramePartitionCreatorInfoProvider
{
public:
    virtual ~IFramePartitionCreatorInfoProvider()
    {

    }
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual const std::vector<uint32_t>& GetTileColsWidthInCTB() const = 0;
    virtual const std::vector<uint32_t>& GetTileRowsHeightInCTB() const = 0;
    virtual uint32_t GetCTBLog2SizeY() const = 0;
    virtual uint32_t GetMinTBLog2SizeY() const = 0;

    bool operator == (const IFramePartitionCreatorInfoProvider& provider) const
    {
        if (this->GetWidth() != provider.GetWidth())
            return false;

        if (this->GetHeight() != provider.GetHeight())
            return false;

        if (this->GetCTBLog2SizeY() != provider.GetCTBLog2SizeY())
            return false;

        if (this->GetMinTBLog2SizeY() != provider.GetMinTBLog2SizeY())
            return false;

        if (this->GetTileColsWidthInCTB() != provider.GetTileColsWidthInCTB())
            return false;

        return 
            this->GetTileRowsHeightInCTB() != provider.GetTileRowsHeightInCTB();
    }
};

#endif
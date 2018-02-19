#ifndef _CODING_TREE_UNIT_CONTEXT_H_
#define _CODING_TREE_UNIT_CONTEXT_H_

#include <memory>

class FramePartition;
enum CABACInitType;
class CodingTreeUnit;

class ICodingTreeUnitContext
{
public:
    virtual std::shared_ptr<FramePartition> GetFramePartition() = 0;
    virtual bool IsSliceSAOLuma() const = 0;
    virtual bool IsSliceSAOChroma() const = 0;
    virtual uint32_t GetSliceSegmentAddress() const = 0;
    virtual uint32_t GetCTBLog2SizeY() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual ChromaFormatType GetChromaFormatType() const = 0;
    virtual uint32_t GetBitDepthLuma() const = 0;
    virtual uint32_t GetBitDepthChroma() const = 0;
    // 同一个slice_segment和tile之内
    virtual CodingTreeUnit* GetLeftNeighbourCTU() const = 0;
    virtual CodingTreeUnit* GetUpNeighbourCTU() const = 0;
};

#endif

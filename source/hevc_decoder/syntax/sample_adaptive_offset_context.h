#ifndef _SAMPLE_ADAPTIVE_OFFSET_CONTEXT_H_
#define _SAMPLE_ADAPTIVE_OFFSET_CONTEXT_H_

#include <stdint.h>

struct Coordinate;
enum CABACInitType;
enum ChromaFormatType;
class SampleAdaptiveOffset;

class ISampleAdaptiveOffsetContext
{
public:
    virtual bool HasLeftCTBInSliceSegment() const = 0;
    virtual bool HasLeftCTBInTile() const = 0;
    virtual bool HasUpCTBInSliceSegment() const = 0;
    virtual bool HasUpCTBInTile() const = 0;
    virtual const Coordinate& GetCurrentCoordinate() const = 0;
    virtual ChromaFormatType GetChromaFormatType() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual bool IsSliceSAOLuma() const = 0;
    virtual bool IsSliceSAOChroma() const = 0;
    virtual uint32_t GetBitDepthLuma() const = 0;
    virtual uint32_t GetBitDepthChroma() const = 0;
    // 同一个slice_segment和tile之内
    virtual const SampleAdaptiveOffset* GetLeftNeighbourSAO() const = 0;
    virtual const SampleAdaptiveOffset* GetUpNeighbourSAO() const = 0;
};

#endif

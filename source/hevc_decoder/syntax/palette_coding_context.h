#ifndef _PALETTE_CODING_CONTEXT_H_
#define _PALETTE_CODING_CONTEXT_H_

#include <memory>

enum ChromaFormatType;
enum CABACInitType;
class PaletteTable;

class IPaletteCodingContext
{
public:
    virtual std::shared_ptr<PaletteTable> GetPredictorPaletteTable() const = 0;
    virtual uint32_t GetPaletteMaxSize() const = 0;
    virtual ChromaFormatType GetChromaFormatType() const = 0;
    virtual uint32_t GetBitDepthOfLuma() const = 0;
    virtual uint32_t GetBitDepthOfChroma() const = 0;
    virtual uint32_t GetPredictorPaletteMaxSize() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual bool IsCUQPDeltaEnabled() const = 0;
    virtual bool IsCUQPDeltaCoded() const = 0;
    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) = 0;
    virtual bool IsCUTransquantBypass() const = 0;
    virtual bool IsCUChromaQPOffsetEnable() const = 0;
    virtual bool IsCUChromaQPOffsetCoded() const = 0;
    virtual uint32_t GetChromaQPOffsetListtLen() const = 0;
    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index) 
        = 0;
};

#endif

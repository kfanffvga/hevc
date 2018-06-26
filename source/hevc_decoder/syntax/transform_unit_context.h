#ifndef _TRANSFORM_UNIT_CONTEXT_H_
#define _TRANSFORM_UNIT_CONTEXT_H_

struct Coordinate;
enum PredModeType;
enum CABACInitType;  

class ITransformUnitContext
{
public:
    virtual uint32_t GetTransformBlockSize() const = 0;
    virtual uint32_t GetTransformBlockDepth() const = 0;
    virtual const Coordinate& GetBaseCoordinate() const = 0;
    virtual const Coordinate& GetCurrentCoordinate() const = 0;
    virtual ChromaFormatType GetChromaFormatType() const = 0;
    virtual bool GetCBFColorBlueValue(uint32_t index, uint32_t depth) const = 0;
    virtual bool GetCBFColorRedValue(uint32_t index, uint32_t depth) const = 0;
    virtual bool GetCBFLumaValue() const = 0;
    virtual uint32_t GetMinCBLog2SizeY() const = 0;
    virtual uint32_t GetMinCBSizeY() const = 0;
    virtual bool IsResidualAdaptiveColorTransformEnabled() const = 0;
    virtual PredModeType GetCUPredMode() const = 0;
    virtual PartModeType GetPartMode() const = 0;
    virtual const std::vector<uint32_t>& GetIntraChromaPredMode() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual bool IsCUQPDeltaEnabled() const = 0;
    virtual bool IsCUQPDeltaCoded() const = 0;
    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) = 0;
    virtual bool IsCUTransquantBypass() const = 0;
    virtual bool IsCUChromaQPOffsetEnable() const = 0;
    virtual bool IsCUChromaQPOffsetCoded() const = 0;
    virtual uint32_t GetChromaQPOffsetListtLen() const = 0;
    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index) = 0;
    virtual bool IsCrossComponentPredictionEnabled() const = 0;
};

#endif

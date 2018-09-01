#ifndef _TRANSFORM_TREE_CONTEXT_H_
#define _TRANSFORM_TREE_CONTEXT_H_

#include <array>
#include <vector>

enum CABACInitType;
enum PredModeType;
enum ChromaFormatType;

class ITransformTreeContext
{
public:
    virtual uint32_t GetMaxTransformBlockSizeY() const = 0;
    virtual uint32_t GetMinTransformBlockSizeY() const = 0;
    virtual uint32_t GetMaxTransformTreeDepth() const = 0;
    virtual bool IsAllowIntraSplit() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual uint32_t GetMaxTransformHierarchyDepthInter() const = 0;
    virtual PredModeType GetCUPredMode() const = 0;
    virtual PartModeType GetPartMode() const = 0;
    virtual ChromaFormatType GetChromaFormatType() const = 0;
    virtual const std::array<bool, 2>&  
        IsPreviousCBContainedTransformCoefficientOfColorBlue() const = 0;

    virtual const std::array<bool, 2>& 
        IsPreviousCBContainedTransformCoefficientOfColorRed() const = 0;

    virtual uint32_t GetMinCBLog2SizeY() const = 0;
    virtual uint32_t GetMinCBSizeY() const = 0;
    virtual bool IsResidualAdaptiveColorTransformEnabled() const = 0;
    virtual const std::vector<uint32_t>& GetIntraChromaPredModeIdentification() 
        const = 0;

    virtual bool IsCUQPDeltaEnabled() const = 0;
    virtual bool IsCUQPDeltaCoded() const = 0;
    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) = 0;
    virtual bool IsCUTransquantBypass() const = 0;
    virtual bool IsCUChromaQPOffsetEnable() const = 0;
    virtual bool IsCUChromaQPOffsetCoded() const = 0;
    virtual uint32_t GetChromaQPOffsetListtLen() const = 0;
    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index) = 0;
    virtual bool IsCrossComponentPredictionEnabled() const = 0;
    virtual bool IsTransformSkipEnabled() const = 0;
    virtual uint32_t GetMaxTransformSkipSize() const = 0;
    virtual bool IsExplicitRDPCMEnabled() const = 0;
    virtual IntraPredModeType GetIntraLumaPredMode(const Coordinate& c) const
        = 0;

    virtual IntraPredModeType GetIntraChromaPredMode(const Coordinate& c) const
        = 0;

    virtual bool IsTransformSkipContextEnabled() const = 0;
    virtual bool IsImplicitRDPCMEnabled() const = 0;
    virtual bool IsCABACBypassAlignmentEnabled() const = 0;
    virtual bool IsSignDataHidingEnabled() const = 0;
    virtual bool IsPersistentRiceAdaptationEnabled() const = 0;
    virtual uint32_t GetBitDepthOfLuma() const = 0;
    virtual uint32_t GetBitDepthOfChroma() const = 0;
    virtual bool HasExtendedPrecisionProcessing() const = 0;
};

#endif

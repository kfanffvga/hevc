#ifndef _RESIDUAL_CODING_CONTEXT_H_
#define _RESIDUAL_CODING_CONTEXT_H_

enum CABACInitType;

class IResidualCodingContext
{
public:
    virtual bool IsTransformSkipEnabled() const = 0;
    virtual bool IsCUTransquantBypass() const = 0;
    virtual uint32_t GetMaxTransformSkipSize() const = 0;
    virtual CABACInitType GetCABACInitType() const = 0;
    virtual PredModeType GetCUPredMode() const = 0;
    virtual bool IsExplicitRDPCMEnabled() const = 0;
    virtual ChromaFormatType GetChromaFormatType() const = 0;
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

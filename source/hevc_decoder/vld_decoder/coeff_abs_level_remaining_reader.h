#ifndef _COEFF_ABS_LEVEL_REMAINING_READER_H_
#define _COEFF_ABS_LEVEL_REMAINING_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class ICoeffAbsLevelRemainingReaderContext
{
public:
    virtual bool IsPersistentRiceAdaptationEnabled() const = 0;
    virtual bool IsTransformSkip() const = 0;
    virtual bool IsCUTransquantBypass() const = 0;
    virtual uint32_t GetColorIndex() const = 0;
    virtual uint32_t GetRiceParam() const = 0;
    virtual void UpdateRiceParamAndAbsLevel(uint32_t rice_param, 
                                            uint32_t abs_level) = 0;

    virtual bool IsUpdatedRiceParamAndAbsLevel() const = 0;
    virtual uint32_t GetAbsLevel() const = 0;
    virtual uint32_t GetBitDepthOfLuma() const = 0;
    virtual uint32_t GetBitDepthOfChroma() const = 0;
    virtual bool HasExtendedPrecisionProcessing() const = 0;
};

class CoeffAbsLevelRemainingReader : public CommonCABACSyntaxReader
{
public:
    CoeffAbsLevelRemainingReader(CABACReader* cabac_reader, 
                                 ICoeffAbsLevelRemainingReaderContext* context);

    virtual ~CoeffAbsLevelRemainingReader();

    uint32_t Read(uint32_t base_level);

private:
    uint32_t GetInitialzationRiceParamAndAbsLevel(uint32_t* rice_param, 
                                                  uint32_t* abs_level);

    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    ICoeffAbsLevelRemainingReaderContext* context_;
};
#endif

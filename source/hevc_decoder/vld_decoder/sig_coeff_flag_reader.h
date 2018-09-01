#ifndef _SIG_COEFF_FLAG_READER_H_
#define _SIG_COEFF_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"
#include "hevc_decoder/base/coordinate.h"

class ISigCoeffFlagReaderContext
{
public:
    virtual uint32_t GetColorIndex() const = 0;
    virtual bool IsTransformSkipContextEnabled() const = 0;
    virtual bool IsTransformSkip() const = 0;
    virtual bool IsCUTransquantBypass() const = 0;
    virtual uint32_t GetLog2TransformSizeY() const = 0;
    virtual const Coordinate& GetCurrentRelativeCoordinate() const = 0;
    virtual bool HasCodedSubBlockOnRight() const = 0;
    virtual bool HasCodedSubBlockOnBottom() const = 0;
    virtual uint32_t GetScanIndex() const = 0;

};

class SigCoeffFlagReader : public CommonCABACSyntaxReader
{
public:
    SigCoeffFlagReader(CABACReader* cabac_reader, CABACInitType init_type, 
                       const ISigCoeffFlagReaderContext* context);
    virtual ~SigCoeffFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    const ISigCoeffFlagReaderContext* context_;
};

#endif

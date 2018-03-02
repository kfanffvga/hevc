#ifndef _PART_MODE_READER_H_
#define _PART_MODE_READER_H_

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class IPartModeReaderContext
{
public:
    virtual uint32_t GetCBSizeY() const = 0;
    virtual bool IsAsymmetricMotionPartitionsEnabled() const = 0;
    virtual uint32_t GetMinCBSizeY() const = 0;
    virtual PredModeType GetCUPredMode() const = 0;
};

class PartModeReader : public CommonCABACSyntaxReader
{
public:
    PartModeReader(CABACReader* cabac_reader, CABACInitType init_type,
                   IPartModeReaderContext* context);
    virtual ~PartModeReader();

    PartModeType Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    IPartModeReaderContext* context_;
};

#endif

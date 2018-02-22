#ifndef _SPLIT_CU_FLAG_READER_H_
#define _SPLIT_CU_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

class ISplitCUFlagReaderContext
{
public:
    virtual bool IsLeftBlockAvailable() const = 0;
    virtual bool IsUpBlockAvailable() const = 0;
    virtual uint32_t GetCurrentBlockLayer() const = 0;
    virtual uint32_t GetLeftBlockLayer() const = 0;
    virtual uint32_t GetUpBlockLayer() const = 0;
};

class SplitCUFlagReader : public CommonCABACSyntaxReader
{
public:
    SplitCUFlagReader(CABACReader* cabac_reader, CABACInitType init_type,
                      ISplitCUFlagReaderContext* context);
    virtual ~SplitCUFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;

    uint32_t lowest_context_index_;
    ISplitCUFlagReaderContext* context_;
};

#endif

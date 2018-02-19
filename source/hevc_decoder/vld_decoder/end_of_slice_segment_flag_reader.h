#ifndef _END_OF_SLICE_SEGMENT_FLAG_READER_H_
#define _END_OF_SLICE_SEGMENT_FLAG_READER_H_

#include "hevc_decoder/vld_decoder/common_cabac_syntax_reader.h"

enum CABACInitType;

class EndOfSliceSegmentFlagReader : public CommonCABACSyntaxReader
{
public:
    EndOfSliceSegmentFlagReader(CABACReader* reader);
    virtual ~EndOfSliceSegmentFlagReader();

    bool Read();

private:
    virtual uint32_t GetArithmeticContextIndex(uint16_t bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16_t bin_idx) override;
};

typedef EndOfSliceSegmentFlagReader EndOfSubSetOneBitReader;
#endif
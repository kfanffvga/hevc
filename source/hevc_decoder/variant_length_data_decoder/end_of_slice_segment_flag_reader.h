#ifndef _END_OF_SLICE_SEGMENT_FLAG_READER_H_
#define _END_OF_SLICE_SEGMENT_FLAG_READER_H_

#include "hevc_decoder/variant_length_data_decoder/common_cabac_syntax_reader.h"

class EndOfSliceSegmentFlagReader : public CommonCABACSyntaxReader
{
public:
    EndOfSliceSegmentFlagReader(CABACReader* reader);
    virtual ~EndOfSliceSegmentFlagReader();

    bool Read();

private:
    virtual int GetArithmeticContextIndex(uint16 bin_idx) override;
    virtual SyntaxElementName GetSyntaxElementName() override;
    virtual ReadFunctionIndex GetFunctionIndex(uint16 bin_idx) override;
};
#endif
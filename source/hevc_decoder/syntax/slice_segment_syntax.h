#ifndef _SLICE_SEGMENT_SYNTAX_H_
#define _SLICE_SEGMENT_SYNTAX_H_

#include <stdint.h>
#include <memory>

class ISliceSegmentContext;
class BitStream;
class SliceSegmentHeader;
class SliceSegmentData;

class SliceSegmentSyntax
{
public:
    SliceSegmentSyntax();
    ~SliceSegmentSyntax();

    bool Parse(BitStream* bit_stream, ISliceSegmentContext* context);

    const SliceSegmentHeader& GetSliceSegmentHeader() const;
    const SliceSegmentData& GetSliceSegmentData() const;

private:    
    std::unique_ptr<SliceSegmentHeader> header_;
    std::unique_ptr<SliceSegmentData> data_;
};

#endif
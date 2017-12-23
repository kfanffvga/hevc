#ifndef _PPS_RANGE_EXTENSION_H_
#define _PPS_RANGE_EXTENSION_H_

class BitStream;

class PPSRangeExtension 
{
public:
    PPSRangeExtension();
    ~PPSRangeExtension();

    bool Parse(BitStream* bit_stream, bool is_transform_skip_enabled);
    bool IsChromaQPOffsetListEnabled() const;

private:
    bool is_chroma_qp_offset_list_enabled_;
};

#endif
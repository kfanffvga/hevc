#ifndef _PPS_RANGE_EXTENSION_H_
#define _PPS_RANGE_EXTENSION_H_

class BitStream;

class PPSRangeExtension 
{
public:
    PPSRangeExtension(bool is_transform_skip_enabled);
    ~PPSRangeExtension();

    bool Parse(BitStream* bit_stream);
    bool IsChromaQPOffsetListEnabled() const;

private:
    bool is_transform_skip_enabled_;
    bool is_chroma_qp_offset_list_enabled_;
};

#endif
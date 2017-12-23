#ifndef _SPS_RANGE_EXTENSION_H_
#define _SPS_RANGE_EXTENSION_H_

class BitStream;

class SPSRangeExtension
{
public:
    SPSRangeExtension();
    virtual ~SPSRangeExtension();

    bool Parse(BitStream* bit_stream);

    bool IsHighPrecisionOffsetsEnabled() const;

private:
    bool is_high_precision_offsets_enabled_;
};

#endif
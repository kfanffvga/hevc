#ifndef _SLICE_SEGMENT_DATA_H_
#define _SLICE_SEGMENT_DATA_H_

class BitStream;

class SliceSegmentData
{
public:
    SliceSegmentData();
    ~SliceSegmentData();

    bool Parse(BitStream bit_stream);
};

#endif
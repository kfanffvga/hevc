#ifndef _VIDEO_PARAMETER_SET_H_
#define _VIDEO_PARAMETER_SET_H_

#include <vector>
#include <memory>

class BitStream;
class GolombReader;
class ProfileTierLevel;
class HrdParmeters;

class VideoParameterSet
{
public:
    VideoParameterSet();
    ~VideoParameterSet();
    
    bool Parse(BitStream* bit_stream);

    uint8_t GetVideoParameterSetID();

private:
    bool ParasHRDInfo(BitStream* bit_stream, uint32_t vps_max_sub_layers);

    uint8_t video_parameter_set_id_;
};

#endif
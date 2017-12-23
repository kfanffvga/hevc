#ifndef _VIDEO_USABILITY_INFOMATION_PARAMETERS_H_
#define _VIDEO_USABILITY_INFOMATION_PARAMETERS_H_

#include <stdint.h>

class BitStream;

class VideoUsabilityInfomationParameter
{
public:
    VideoUsabilityInfomationParameter();
    ~VideoUsabilityInfomationParameter();

    bool Parse(BitStream* bit_stream, uint32_t sps_max_sub_layers);

};

#endif
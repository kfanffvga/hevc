#ifndef _VIDEO_USABILITY_INFOMATION_PARAMETERS_H_
#define _VIDEO_USABILITY_INFOMATION_PARAMETERS_H_

#include <stdint.h>

#include "hevc_decoder/syntax/base_syntax.h"

class VideoUsabilityInfomationParameter : public BaseSyntax
{
public:
    VideoUsabilityInfomationParameter(uint32_t sps_max_sub_layers);
    virtual ~VideoUsabilityInfomationParameter();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    uint32_t sps_max_sub_layers_;

};

#endif
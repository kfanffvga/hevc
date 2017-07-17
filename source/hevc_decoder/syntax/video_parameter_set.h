#ifndef _VIDEO_PARAMETER_SET_H_
#define _VIDEO_PARAMETER_SET_H_

#include <memory>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;
class ProfileTierLevel;

class VideoParameterSet : public BaseSyntax
{
public:
    VideoParameterSet();
    ~VideoParameterSet();
    
    virtual bool Parse(BitStream* nal_unit) override;

private:
    std::unique_ptr<ProfileTierLevel> profile_tier_level_;
};

#endif
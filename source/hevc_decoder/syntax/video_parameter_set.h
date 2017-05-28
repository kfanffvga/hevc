#ifndef _VIDEO_PARAMETER_SET_H_
#define _VIDEO_PARAMETER_SET_H_

#include <memory>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;
class NalUnit;
class ProfileTierLevel;

using std::unique_ptr;

class VideoParameterSet : public BaseSyntax
{
public:
    VideoParameterSet(unique_ptr<NalUnit> nal_unit);
    ~VideoParameterSet();
    
    virtual bool Parser() override;

private:
    unique_ptr<NalUnit> nal_unit_;
    unique_ptr<ProfileTierLevel> profile_tier_level_;
};

#endif
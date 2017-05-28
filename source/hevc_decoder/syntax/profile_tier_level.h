#ifndef _PROFILE_TIER_LEVEL_H_
#define _PROFILE_TIER_LEVEL_H_

#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;

class ProfileTierLevel : BaseSyntax
{
public:
    ProfileTierLevel(BitStream* bit_stream, bool profile_present_flag,
                     int max_num_sub_layers);
    ~ProfileTierLevel();

    virtual bool Parser() override;

private:
    BitStream* bit_stream_;
    bool profile_present_flag_;
    int max_num_sub_layers_;
};

#endif
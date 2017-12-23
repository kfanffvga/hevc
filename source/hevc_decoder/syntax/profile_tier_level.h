#ifndef _PROFILE_TIER_LEVEL_H_
#define _PROFILE_TIER_LEVEL_H_

class BitStream;

class ProfileTierLevel
{
public:
    ProfileTierLevel(bool profile_present_flag, int max_num_sub_layers);
    virtual ~ProfileTierLevel();

    bool Parse(BitStream* bit_stream);

private:
    void ParseGeneralProfileInfo(BitStream* bit_stream);
    void ParseSubLayerInfo(BitStream* bit_stream);

    bool profile_present_flag_;
    int max_num_sub_layers_;
};

#endif
#ifndef _PROFILE_TIER_LEVEL_H_
#define _PROFILE_TIER_LEVEL_H_

class BitStream;

class ProfileTierLevel
{
public:
    ProfileTierLevel();
    virtual ~ProfileTierLevel();

    bool Parse(BitStream* bit_stream, bool has_profile_present, 
               int max_num_sub_layers);

private:
    void ParseGeneralProfileInfo(BitStream* bit_stream);
    void ParseSubLayerInfo(BitStream* bit_stream, int max_num_sub_layers);

};

#endif
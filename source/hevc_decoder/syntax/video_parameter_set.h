#ifndef _VIDEO_PARAMETER_SET_H_
#define _VIDEO_PARAMETER_SET_H_

#include <vector>
#include <memory>

#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;
class GolombReader;
class ProfileTierLevel;
class HrdParmeters;

class VideoParameterSet : public BaseSyntax
{
public:
    VideoParameterSet();
    ~VideoParameterSet();
    
    virtual bool Parse(BitStream* nal_unit) override;

private:
    void ParasVPSInfo(BitStream* bit_stream, GolombReader* golomb_reader);
    void ParasHRDInfo(BitStream* bit_stream, GolombReader* golomb_reader,
                      uint32 vps_max_sub_layers);

    std::unique_ptr<ProfileTierLevel> profile_tier_level_;
    std::vector<std::unique_ptr<HrdParmeters>> hrd_parameters_;
};

#endif
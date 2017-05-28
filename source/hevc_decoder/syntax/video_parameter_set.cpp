#include "hevc_decoder/syntax/video_parameter_set.h"

#include <vector>

#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/profile_tier_level.h"

using std::vector;

VideoParameterSet::VideoParameterSet(std::unique_ptr<NalUnit> nal_unit)
    : nal_unit_(std::move(nal_unit))
    , profile_tier_level_(nullptr)
{

}

VideoParameterSet::~VideoParameterSet()
{

}

bool VideoParameterSet::Parser()
{
    BitStream* bit_stream = nal_unit_->GetBitSteam();
    if (!bit_stream)
        return false;

    uint8 vps_video_parameter_set_id = bit_stream->Read<uint8>(4);
    bool vps_base_layer_internal_flag = bit_stream->ReadBool();
    bool vps_base_layer_available_flag = bit_stream->ReadBool();
    uint8 vps_max_layers = bit_stream->Read<uint8>(6);
    uint8 vps_max_sub_layers = bit_stream->Read<uint8>(3) + 1;
    bool vsp_temporal_id_nesting_flag = bit_stream->ReadBool();
    uint16 vps_reserved = bit_stream->Read<uint16>(16);
    if (!profile_tier_level_)
        profile_tier_level_.reset(new ProfileTierLevel(bit_stream, true, 
                                                       (vps_max_sub_layers - 1)));

    profile_tier_level_->Parser();
    bool vps_sub_layer_ordering_info_present = bit_stream->ReadBool();
    vector<uint64> vps_max_dec_pic_buffering;
    vector<uint64> vps_max_num_reorder_pics;
    vector<uint64> vps_max_latency_increase;
    GolombReadr golom_reader(bit_stream);
    uint64 read_result = 0;
    for (int i = vps_sub_layer_ordering_info_present ? 0 : vps_max_sub_layers; 
         i <= vps_max_sub_layers; ++i)
    {
        golom_reader.Read(&read_result);        
        vps_max_dec_pic_buffering.push_back(read_result + 1);
        golom_reader.Read(&read_result);
        vps_max_num_reorder_pics.push_back(read_result);
        golom_reader.Read(&read_result);
        vps_max_latency_increase.push_back(read_result - 1);
    }

    uint8 vps_max_layer_id = bit_stream->Read<uint8>(6);

    return true;
}

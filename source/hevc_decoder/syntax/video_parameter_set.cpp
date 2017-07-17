#include "hevc_decoder/syntax/video_parameter_set.h"

#include <boost/multi_array.hpp>
#include <vector>

#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/profile_tier_level.h"

using std::vector;
using std::unique_ptr;
using std::move;
using boost::multi_array;

VideoParameterSet::VideoParameterSet()
    : profile_tier_level_(nullptr)
{

}

VideoParameterSet::~VideoParameterSet()
{

}

bool VideoParameterSet::Parse(BitStream* bit_stream)
{
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
    {
        profile_tier_level_.reset(
            new ProfileTierLevel(true, vps_max_sub_layers));
    }

    profile_tier_level_->Parse(bit_stream);
    bool is_vps_sub_layer_ordering_info_present = bit_stream->ReadBool();
    vector<uint64> vps_max_dec_pic_buffering;
    vector<uint64> vps_max_num_reorder_pics;
    vector<uint64> vps_max_latency_increase;

    GolombReadr golomb_reader(bit_stream);
    int sub_layer_start_index = 
        is_vps_sub_layer_ordering_info_present ? 0 : vps_max_sub_layers;
    for (int i = sub_layer_start_index; i <= vps_max_sub_layers; ++i)
    {
        vps_max_dec_pic_buffering.push_back(
            golomb_reader.ReadUnsignedValue() + 1);
        vps_max_num_reorder_pics.push_back(golomb_reader.ReadUnsignedValue());
        vps_max_latency_increase.push_back(
            golomb_reader.ReadUnsignedValue() - 1);
    }

    uint8 vps_max_layer_id = bit_stream->Read<uint8>(6);
    uint32 vps_num_layer_sets = golomb_reader.ReadUnsignedValue() + 1;
    multi_array<bool, 2> layer_id_included(
        boost::extents[vps_num_layer_sets][vps_max_layer_id]);
    for (uint32 i = 1; i <= vps_num_layer_sets; ++i)
        for (uint8 j = 0; j <= vps_max_layer_id; ++j)
            layer_id_included[i][j] = bit_stream->ReadBool();

    bool vps_timing_info_present = bit_stream->ReadBool();
    if (vps_timing_info_present)
    {
        uint32 vps_num_units_in_tick = bit_stream->Read<uint32>(32);
        uint32 vps_time_scale = bit_stream->Read<uint32>(32);
        bool vps_poc_proportional_to_timing = bit_stream->ReadBool();
        if (vps_poc_proportional_to_timing)
        {
            uint32 vps_num_ticks_poc_diff_one = 
                golomb_reader.ReadUnsignedValue() + 1;
        }

        uint32 vps_num_hrd_parameters = golomb_reader.ReadUnsignedValue();
        std::vector<uint32> hrd_layer_set_idx;
        multi_array<bool, 1> cprms_present(boost::extents[vps_num_hrd_parameters]);
        for (uint32 i = 0; i < vps_num_hrd_parameters; ++i)
        {
            hrd_layer_set_idx.push_back(golomb_reader.ReadUnsignedValue());
            if (i > 0)
                cprms_present[i] = bit_stream->ReadBool();
            else
                cprms_present[i] = true;
        }
    }

    return true;
}

#include "hevc_decoder/syntax/video_parameter_set.h"

#include <boost/multi_array.hpp>
#include <vector>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/profile_tier_level.h"
#include "hevc_decoder/syntax/hrd_parameters.h"

using std::vector;
using std::unique_ptr;
using std::move;
using boost::multi_array;
using boost::extents;

VideoParameterSet::VideoParameterSet()
    : video_parameter_set_id_(0)
    , profile_tier_level_()
    , hrd_parameters_()
{

}

VideoParameterSet::~VideoParameterSet()
{

}

bool VideoParameterSet::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    ParasVPSInfo(bit_stream, &golomb_reader);
    return true;
}

void VideoParameterSet::ParasVPSInfo(BitStream* bit_stream,
                                     GolombReader* golomb_reader)
{   
    video_parameter_set_id_ = bit_stream->Read<uint8_t>(4);
    bool is_vps_base_layer_internal = bit_stream->ReadBool();
    bool is_vps_base_layer_available = bit_stream->ReadBool();
    uint8_t vps_max_layers = bit_stream->Read<uint8_t>(6) + 1;
    assert((vps_max_layers > 0) && (vps_max_layers < 63));
    uint8_t vps_max_sub_layers = bit_stream->Read<uint8_t>(3) + 1;
    assert((vps_max_sub_layers >= 0) && (vps_max_sub_layers <= 6));
    bool is_vps_temporal_id_nesting = bit_stream->ReadBool();

    bit_stream->SkipBits(16);
    profile_tier_level_.reset(new ProfileTierLevel(true, vps_max_sub_layers));

    profile_tier_level_->Parse(bit_stream);
    bool is_vps_sub_layer_ordering_info_present = bit_stream->ReadBool();
    vector<uint32_t> vps_max_dec_pic_buffering;
    vector<uint64_t> vps_max_num_reorder_pics;
    vector<uint64_t> vps_max_latency_increase;
    
    int sub_layer_start_index =
        is_vps_sub_layer_ordering_info_present ? 0 : (vps_max_sub_layers - 1);
    for (int i = sub_layer_start_index; i < vps_max_sub_layers; ++i)
    {
        vps_max_dec_pic_buffering.push_back(
            golomb_reader->ReadUnsignedValue() + 1);
        vps_max_num_reorder_pics.push_back(golomb_reader->ReadUnsignedValue());
        vps_max_latency_increase.push_back(
            golomb_reader->ReadUnsignedValue() - 1);
    }

    uint8_t vps_max_layer_id = bit_stream->Read<uint8_t>(6);
    assert(vps_max_layer_id < 63);
    uint32_t vps_num_layer_sets = golomb_reader->ReadUnsignedValue() + 1;
    multi_array<bool, 2> layer_id_included(
        boost::extents[vps_num_layer_sets][vps_max_layer_id + 1]);
    uint32_t vps_num_layer_sets_minus1 = vps_num_layer_sets - 1;
    for (uint32_t i = 0; i < vps_num_layer_sets_minus1; ++i)
        for (uint8_t j = 0; j <= vps_max_layer_id; ++j)
            layer_id_included[i][j] = bit_stream->ReadBool();

    bool vps_timing_info_present = bit_stream->ReadBool();
    if (vps_timing_info_present)
    {
        uint32_t vps_num_units_in_tick = bit_stream->Read<uint32_t>(32);
        uint32_t vps_time_scale = bit_stream->Read<uint32_t>(32);
        bool vps_poc_proportional_to_timing = bit_stream->ReadBool();
        if (vps_poc_proportional_to_timing)
        {
            uint32_t vps_num_ticks_poc_diff_one =
                golomb_reader->ReadUnsignedValue() + 1;
        }

        ParasHRDInfo(bit_stream, golomb_reader, vps_max_sub_layers);
    }

    bool is_vps_extension = bit_stream->ReadBool();
}

void VideoParameterSet::ParasHRDInfo(BitStream* bit_stream,
                                     GolombReader* golomb_reader,
                                     uint32_t vps_max_sub_layers)
{
    uint32_t vps_num_hrd_parameters = golomb_reader->ReadUnsignedValue();
    vector<uint32_t> hrd_layer_set_idx;
    multi_array<bool, 1> cprms_present(extents[vps_num_hrd_parameters]);
    for (uint32_t i = 0; i < vps_num_hrd_parameters; ++i)
    {
        hrd_layer_set_idx.push_back(golomb_reader->ReadUnsignedValue());
        if (i > 0)
            cprms_present[i] = bit_stream->ReadBool();
        else
            cprms_present[i] = true;

        hrd_parameters_.push_back(unique_ptr<HrdParmeters>(
            new HrdParmeters(cprms_present[i], vps_max_sub_layers)));
    }
}

uint8_t VideoParameterSet::GetVideoParameterSetID()
{
    return video_parameter_set_id_;
}

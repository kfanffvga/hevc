#include "hevc_decoder/syntax/video_usability_infomation_parameters.h"

#include <map>
#include <utility>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/hrd_parameters.h"

using std::map;
using std::make_pair;

struct SampleAspectRatio
{
    uint16_t width;
    uint16_t height;
};

namespace 
{ 
const uint8_t extended_sar = 255;
const map<uint8_t, SampleAspectRatio> sar_info = 
{  
    make_pair<uint8_t, SampleAspectRatio>(1, {1, 1}), 
    make_pair<uint8_t, SampleAspectRatio>(2, {12, 11}), 
    make_pair<uint8_t, SampleAspectRatio>(3, {10, 11}), 
    make_pair<uint8_t, SampleAspectRatio>(4, {16, 111}), 
    make_pair<uint8_t, SampleAspectRatio>(5, {40, 33}), 
    make_pair<uint8_t, SampleAspectRatio>(6, {24, 11}), 
    make_pair<uint8_t, SampleAspectRatio>(7, {20, 11}), 
    make_pair<uint8_t, SampleAspectRatio>(8, {32, 11}), 
    make_pair<uint8_t, SampleAspectRatio>(9, {80, 33}), 
    make_pair<uint8_t, SampleAspectRatio>(10, {18, 11}), 
    make_pair<uint8_t, SampleAspectRatio>(11, {15, 11}), 
    make_pair<uint8_t, SampleAspectRatio>(12, {64, 33}), 
    make_pair<uint8_t, SampleAspectRatio>(13, {160, 99}), 
    make_pair<uint8_t, SampleAspectRatio>(14, {4, 3}), 
    make_pair<uint8_t, SampleAspectRatio>(15, {3, 2}), 
    make_pair<uint8_t, SampleAspectRatio>(16, {2, 1}), 
    make_pair<uint8_t, SampleAspectRatio>(255, {0, 0})
};
}

VideoUsabilityInfomationParameter::VideoUsabilityInfomationParameter(
    uint32_t sps_max_sub_layers)
    : sps_max_sub_layers_(sps_max_sub_layers)
{

}

VideoUsabilityInfomationParameter::~VideoUsabilityInfomationParameter()
{

}

bool VideoUsabilityInfomationParameter::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool has_aspect_ratio_info_present = bit_stream->ReadBool();
    SampleAspectRatio sar = { };
    if (has_aspect_ratio_info_present)
    {
        uint8_t aspect_ratio_idc = bit_stream->Read<uint8_t>(8);
        if (extended_sar == aspect_ratio_idc)
        {
            sar.width = bit_stream->Read<uint16_t>(16);
            sar.height = bit_stream->Read<uint16_t>(16);
        }
        else
        {
            auto r = sar_info.find(aspect_ratio_idc);
            if (r != sar_info.end())
                sar = r->second;
        }
    }

    bool has_overscan_info_present = bit_stream->ReadBool();
    if (has_overscan_info_present)
        bool is_overscan_appropriate = bit_stream->ReadBool();

    bool has_video_signal_type_present = bit_stream->ReadBool();
    if (has_video_signal_type_present)
    {
        uint8_t video_format = bit_stream->Read<uint8_t>(3);
        bool is_video_full_range = bit_stream->ReadBool();
        bool has_colour_description_present = bit_stream->ReadBool();
        if (has_colour_description_present)
        {
            uint8_t colour_primaries = bit_stream->Read<uint8_t>(8);
            uint8_t transfer_characteristics = bit_stream->Read<uint8_t>(8);
            uint8_t matrix_coeffs = bit_stream->Read<uint8_t>(8);
        }
    }

    bool has_chroma_loc_info_present = bit_stream->ReadBool();
    GolombReader golomb_reader(bit_stream);
    if (has_chroma_loc_info_present)
    {
        uint32_t chroma_sample_loc_type_top_field = 
            golomb_reader.ReadUnsignedValue();

        uint32_t chroma_sample_loc_type_bottom_field = 
            golomb_reader.ReadUnsignedValue();
    }

    bool is_neutral_chroma_indication = bit_stream->ReadBool();
    bool is_field_seq = bit_stream->ReadBool();
    bool has_frame_field_info_present = bit_stream->ReadBool();
    bool is_default_display_window = bit_stream->ReadBool();
    if (is_default_display_window)
    {
        uint32_t def_disp_win_left_offset = golomb_reader.ReadUnsignedValue();
        uint32_t def_disp_win_right_offset = golomb_reader.ReadUnsignedValue();
        uint32_t def_disp_win_top_offset = golomb_reader.ReadUnsignedValue();
        uint32_t def_disp_win_bottom_offset = golomb_reader.ReadUnsignedValue();
    }

    bool has_vui_timing_info_present = bit_stream->ReadBool();
    if (has_vui_timing_info_present)
    {
        uint32_t vui_num_units_in_tick = bit_stream->Read<uint32_t>(32);
        uint32_t vui_time_scale = bit_stream->Read<uint32_t>(32);
        bool has_vui_poc_proportional_to_timing = bit_stream->ReadBool();
        if (has_vui_poc_proportional_to_timing)
        {
            uint32_t vui_num_ticks_poc_diff_one = 
                golomb_reader.ReadUnsignedValue() + 1;
        }
        bool has_vui_hrd_parameters_present = bit_stream->ReadBool();
        if (has_vui_hrd_parameters_present)
        {
            HrdParmeters hrd_parameters(true, sps_max_sub_layers_);
            bool success = hrd_parameters.Parse(bit_stream);
            if (!success)
                return false;
        }
    }

    bool is_bitstream_restriction = bit_stream->ReadBool();
    if (is_bitstream_restriction)
    {
        bool is_tiles_fixed_structure = bit_stream->ReadBool();
        bool is_motion_vectors_over_pic_boundaries = bit_stream->ReadBool();
        bool has_restricted_ref_pic_lists = bit_stream->ReadBool();
        uint32_t min_spatial_segmentation_idc = 
            golomb_reader.ReadUnsignedValue();

        uint32_t max_bytes_per_pic_denom = golomb_reader.ReadUnsignedValue();
        uint32_t max_bits_per_min_cu_denom = golomb_reader.ReadUnsignedValue();
        uint32_t log2_max_mv_length_horizontal = 
            golomb_reader.ReadUnsignedValue();

        uint32_t log2_max_mv_length_vertical = golomb_reader.ReadUnsignedValue();
    }
    return true;
}

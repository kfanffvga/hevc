﻿#include "hevc_decoder/syntax/sequence_parameter_set.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/profile_tier_level.h"
#include "hevc_decoder/syntax/scaling_list_data.h"
#include "hevc_decoder/syntax/short_term_reference_picture_set.h"
#include "hevc_decoder/syntax/video_usability_infomation_parameters.h"
#include "hevc_decoder/syntax/sps_range_extension.h"
#include "hevc_decoder/syntax/sps_multilayer_extension.h"
#include "hevc_decoder/syntax/sps_3d_extension.h"
#include "hevc_decoder/syntax/sps_screen_content_coding_extension.h"

using std::vector;
using std::unique_ptr;

SequenceParameterSet::SequenceParameterSet()
{

}

SequenceParameterSet::~SequenceParameterSet()
{

}

bool SequenceParameterSet::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    uint32_t sps_video_parameter_set_id = bit_stream->Read<uint32_t>(4);
    uint8_t sps_max_sub_layers = bit_stream->Read<uint32_t>(3) + 1;
    bool is_sps_temporal_id_nesting = bit_stream->ReadBool();
    ProfileTierLevel profile_tier_level(true, sps_max_sub_layers);
    if (!profile_tier_level.Parse(bit_stream))
        return false;

    GolombReader golomb_reader(bit_stream);
    sps_seq_parameter_set_id_ = golomb_reader.ReadUnsignedValue();
    uint32_t chroma_format_idc = golomb_reader.ReadUnsignedValue();
    if (3 == chroma_format_idc)
        bool is_separate_colour_plane = bit_stream->ReadBool();

    uint32_t pic_width_in_luma_samples = golomb_reader.ReadUnsignedValue();
    uint32_t pic_height_in_luma_samples = golomb_reader.ReadUnsignedValue();

    bool is_conformance_window = bit_stream->ReadBool();
    if (is_conformance_window)
    {
        uint32_t conf_win_left_offset = golomb_reader.ReadUnsignedValue();
        uint32_t conf_win_right_offset = golomb_reader.ReadUnsignedValue();
        uint32_t conf_win_top_offset = golomb_reader.ReadUnsignedValue();
        uint32_t conf_win_bottom_offset = golomb_reader.ReadUnsignedValue();
    }
    uint32_t bit_depth_luma = golomb_reader.ReadUnsignedValue() + 8;
    uint32_t bit_depth_chroma = golomb_reader.ReadUnsignedValue() + 8;

    uint32_t log2_max_pic_order_cnt_lsb = golomb_reader.ReadUnsignedValue() + 4;

    bool has_sps_sub_layer_ordering_info_present = bit_stream->ReadBool();
    ParseSubLayerOrderingInfo(&golomb_reader, sps_max_sub_layers, 
                              has_sps_sub_layer_ordering_info_present);

    uint32_t log2_min_luma_coding_block_size = 
        golomb_reader.ReadUnsignedValue() + 3;
    uint32_t log2_diff_max_min_luma_coding_block_size = 
        golomb_reader.ReadUnsignedValue();
    uint32_t ctb_log2_size_y = log2_min_luma_coding_block_size + 
        log2_diff_max_min_luma_coding_block_size;

    uint32_t log2_min_luma_transform_block_size = 
        golomb_reader.ReadUnsignedValue() + 2;
    uint32_t log2_diff_max_min_luma_transform_block_size = 
        golomb_reader.ReadUnsignedValue();

    uint32_t max_transform_hierarchy_depth_inter = 
        golomb_reader.ReadUnsignedValue();
    uint32_t max_transform_hierarchy_depth_intra = 
        golomb_reader.ReadUnsignedValue();

    bool is_scaling_list_enabled = bit_stream->ReadBool();
    if (is_scaling_list_enabled)
    {
        bool has_sps_scaling_list_data_present = bit_stream->ReadBool();
        if (has_sps_scaling_list_data_present)
        {
            ScalingListData scaling_list_data;
            if (!scaling_list_data.Parse(bit_stream))
                return false;
        }
    }
    bool is_amp_enabled = bit_stream->ReadBool();
    bool is_sample_adaptive_offset_enabled = bit_stream->ReadBool();
    bool is_pcm_enabled = bit_stream->ReadBool();
    if (is_pcm_enabled)
    {
        uint8_t pcm_sample_bit_depth_luma = bit_stream->Read<uint8_t>(4) + 1;
        uint8_t pcm_sample_bit_depth_chroma_ = bit_stream->Read<uint8_t>(4) + 1;
        uint32_t log2_min_pcm_luma_coding_block_ = 
            golomb_reader.ReadUnsignedValue() + 3;
        uint32_t log2_diff_max_min_pcm_luma_coding_block_size = 
            golomb_reader.ReadUnsignedValue();
        bool is_pcm_loop_filter_disabled = bit_stream->ReadBool();
    }
    bool success = ParseReferencePicturesInfo(bit_stream, 
                                              log2_max_pic_order_cnt_lsb);
    if (!success)
        return false;

    bool is_sps_temporal_mvp_enabled = bit_stream->ReadBool();
    bool is_strong_intra_smoothing_enabled = bit_stream->ReadBool();
    bool has_vui_parameters_present = bit_stream->ReadBool();
    if (has_vui_parameters_present)
    {
        VideoUsabilityInfomationParameter vui_parameters(sps_max_sub_layers);
        bool success = vui_parameters.Parse(bit_stream);
        if (!success)
            return false;
    }
    return ParseExtensionInfo(bit_stream, ctb_log2_size_y, chroma_format_idc);
}

uint32_t SequenceParameterSet::GetSequenceParameterSetID()
{
    return sps_seq_parameter_set_id_;
}

void SequenceParameterSet::ParseSubLayerOrderingInfo(
    GolombReader* golomb_reader, uint32_t sps_max_sub_layers, 
    bool has_sps_sub_layer_ordering_info_present)
{
    vector<SubLayerOrderingInfo> sub_layer_ordering_info;
    if (has_sps_sub_layer_ordering_info_present)
    {
        for (uint32_t i = 0; i < sps_max_sub_layers; ++i)
        {
            SubLayerOrderingInfo info = { };
            info.sps_max_dec_pic_buffering =
                golomb_reader->ReadUnsignedValue() + 1;

            info.sps_max_num_reorder_pics = golomb_reader->ReadUnsignedValue();
            info.sps_max_latency_increase = static_cast<int>(
                golomb_reader->ReadUnsignedValue()) - 1;

            sub_layer_ordering_info.push_back(info);
        }
    }
    else
    {
        SubLayerOrderingInfo info = { };
        info.sps_max_dec_pic_buffering = golomb_reader->ReadUnsignedValue() + 1;

        info.sps_max_num_reorder_pics = golomb_reader->ReadUnsignedValue();
        info.sps_max_latency_increase = golomb_reader->ReadUnsignedValue() - 1;
        for (uint32_t i = 0; i < sps_max_sub_layers; ++i)
            sub_layer_ordering_info.push_back(info);
    }
}

bool SequenceParameterSet::ParseReferencePicturesInfo(
    BitStream* bit_stream, uint32_t log2_max_pic_order_cnt_lsb)
{
    GolombReader golomb_reader(bit_stream);
    uint32_t num_short_term_ref_pic_sets = golomb_reader.ReadUnsignedValue();
    for (uint32_t i = 0; i < num_short_term_ref_pic_sets; ++i)
    {
        unique_ptr<ShortTermReferencePictureSet> st_ref_pic_set(
            new ShortTermReferencePictureSet(this, i));
        bool success = st_ref_pic_set->Parse(bit_stream);
        if (!success)
            return false;

        short_term_reference_picture_sets_.push_back(move(st_ref_pic_set));
    }

    bool has_long_term_ref_pics_present = bit_stream->ReadBool();

    if (has_long_term_ref_pics_present)
    {
        uint32_t num_long_term_ref_pics_sps = golomb_reader.ReadUnsignedValue();
        vector<LongTermReferencePictureOrderCountInfo> lt_ref_poc_info;
        for (uint32_t i = 0; i < num_long_term_ref_pics_sps; ++i)
        {
            LongTermReferencePictureOrderCountInfo info = { };
            info.lt_ref_pic_poc_lsb_sps = 
                bit_stream->Read<uint32_t>(log2_max_pic_order_cnt_lsb);
            info.is_used_by_curr_pic_lt_sps_flag = bit_stream->ReadBool();
            lt_ref_poc_info.push_back(info);
        }
    }
    return true;
}

uint32_t SequenceParameterSet::GetShortTermReferencePictureSetCount()
{
    return short_term_reference_picture_sets_.size();
}

const ShortTermReferencePictureSet*
    SequenceParameterSet::GetShortTermReferencePictureSet(uint32_t index)
{
    if (index >= short_term_reference_picture_sets_.size())
        return nullptr;

    return short_term_reference_picture_sets_[index].get();
}

bool SequenceParameterSet::ParseExtensionInfo(BitStream* bit_stream, 
                                              uint32_t ctb_log2_size_y,
                                              uint32_t chroma_format_idc)
{
    bool has_sps_extension_present = bit_stream->ReadBool();

    bool has_sps_range_extension = false;
    bool has_sps_multilayer_extension = false;
    bool has_sps_3d_extension = false;
    bool has_sps_scc_extension = false;
    bool has_sps_extension = false;
    if (has_sps_extension_present)
    {
        has_sps_range_extension = bit_stream->ReadBool();
        has_sps_multilayer_extension = bit_stream->ReadBool();
        has_sps_3d_extension = bit_stream->ReadBool();
        has_sps_scc_extension = bit_stream->ReadBool();
        has_sps_extension = bit_stream->ReadBool();
    }
    if (has_sps_range_extension)
    {
        SPSRangeExtension sps_range_extension;
        bool success = sps_range_extension.Parse(bit_stream);
        if (!success)
            return false;
    }
    if (has_sps_multilayer_extension)
    {
        SPSMultilayerExtension sps_multilayer_extension;
        bool success = sps_multilayer_extension.Parse(bit_stream);
        if (!success)
            return false;
    }
    if (has_sps_3d_extension)
    {
        SPS3DExtension sps_3d_extension(ctb_log2_size_y);
        bool success = sps_3d_extension.Parse(bit_stream);
        if (!success)
            return false;
    }
    if (has_sps_scc_extension)
    {
        uint32_t num_of_color_compoments = 0 == chroma_format_idc ? 1 : 3;
        SPSScreenContentCodingExtension sps_scc_extension(chroma_format_idc);
        bool success = sps_scc_extension.Parse(bit_stream);
        if (!success)
            return false;
    }
    return true;
}

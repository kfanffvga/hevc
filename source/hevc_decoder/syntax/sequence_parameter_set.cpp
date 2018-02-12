#include "hevc_decoder/syntax/sequence_parameter_set.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/base/math.h"
#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/profile_tier_level.h"
#include "hevc_decoder/syntax/scaling_list_data.h"
#include "hevc_decoder/syntax/short_term_reference_picture_set.h"
#include "hevc_decoder/syntax/video_usability_infomation_parameters.h"
#include "hevc_decoder/syntax/sps_range_extension.h"
#include "hevc_decoder/syntax/sps_multilayer_extension.h"
#include "hevc_decoder/syntax/sps_3d_extension.h"
#include "hevc_decoder/syntax/sps_screen_content_coding_extension.h"
#include "hevc_decoder/syntax/short_term_reference_picture_set_context_impl.h"

using std::vector;
using std::unique_ptr;

SequenceParameterSet::SequenceParameterSet()
    : sps_seq_parameter_set_id_(0)
    , short_term_reference_picture_sets_()
    , slice_segment_address_bit_length_(0)
    , log2_max_pic_order_cnt_lsb_(0)
    , has_long_term_ref_pics_present_(false)
    , lt_ref_poc_infos_()
    , is_sps_temporal_mvp_enabled_(false)
    , is_sample_adaptive_offset_enabled_(false)
    , chroma_array_type_(MONO_CHROME)
    , max_lsb_of_pic_order_count_(1 << 4)
    , pic_width_in_luma_samples_(0)
    , pic_height_in_luma_samples_(0)
    , sps_range_extension_(new SPSRangeExtension())
    , sps_scc_extension_(new SPSScreenContentCodingExtension())
    , bit_depth_chroma_(0)
    , ctb_log2_size_y_(0)
    , ctb_height_(0)
    , log2_min_luma_transform_block_size_(0)
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
    ProfileTierLevel profile_tier_level;
    if (!profile_tier_level.Parse(bit_stream, true, sps_max_sub_layers))
        return false;

    GolombReader golomb_reader(bit_stream);
    sps_seq_parameter_set_id_ = golomb_reader.ReadUnsignedValue();
    uint32_t chroma_format_idc = golomb_reader.ReadUnsignedValue();
    chroma_array_type_ = static_cast<ChromaFormatType>(chroma_format_idc);
    if (3 == chroma_format_idc)
    {
        bool is_separate_colour_plane = bit_stream->ReadBool();
        if (is_separate_colour_plane)
            chroma_array_type_ = YUV_MONO_CHROME;
    }

    pic_width_in_luma_samples_ = golomb_reader.ReadUnsignedValue();
    pic_height_in_luma_samples_ = golomb_reader.ReadUnsignedValue();

    bool is_conformance_window = bit_stream->ReadBool();
    if (is_conformance_window)
    {
        uint32_t conf_win_left_offset = golomb_reader.ReadUnsignedValue();
        uint32_t conf_win_right_offset = golomb_reader.ReadUnsignedValue();
        uint32_t conf_win_top_offset = golomb_reader.ReadUnsignedValue();
        uint32_t conf_win_bottom_offset = golomb_reader.ReadUnsignedValue();
    }
    uint32_t bit_depth_luma = golomb_reader.ReadUnsignedValue() + 8;
    bit_depth_chroma_ = golomb_reader.ReadUnsignedValue() + 8;

    log2_max_pic_order_cnt_lsb_ = golomb_reader.ReadUnsignedValue() + 4;
    max_lsb_of_pic_order_count_ = 1 << log2_max_pic_order_cnt_lsb_;

    bool has_sps_sub_layer_ordering_info_present = bit_stream->ReadBool();
    ParseSubLayerOrderingInfo(&golomb_reader, sps_max_sub_layers, 
                              has_sps_sub_layer_ordering_info_present);

    uint32_t log2_min_luma_coding_block_size = 
        golomb_reader.ReadUnsignedValue() + 3;
    uint32_t log2_diff_max_min_luma_coding_block_size = 
        golomb_reader.ReadUnsignedValue();

    ctb_log2_size_y_ = log2_min_luma_coding_block_size + 
        log2_diff_max_min_luma_coding_block_size;
    ctb_height_ = 1 << ctb_log2_size_y_;
    uint32_t pic_width_in_ctb_y =
        UpAlignRightShift(pic_width_in_luma_samples_, ctb_log2_size_y_);
    uint32_t pic_height_in_ctb_y =
        UpAlignRightShift(pic_height_in_luma_samples_, ctb_log2_size_y_);
    uint32_t pic_size_in_ctb = pic_width_in_ctb_y * pic_height_in_ctb_y;
    slice_segment_address_bit_length_ = CeilLog2(pic_size_in_ctb);

    log2_min_luma_transform_block_size_ = golomb_reader.ReadUnsignedValue() + 2;
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
    is_sample_adaptive_offset_enabled_ = bit_stream->ReadBool();
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
                                              log2_max_pic_order_cnt_lsb_);
    if (!success)
        return false;

    is_sps_temporal_mvp_enabled_ = bit_stream->ReadBool();
    bool is_strong_intra_smoothing_enabled = bit_stream->ReadBool();
    bool has_vui_parameters_present = bit_stream->ReadBool();
    if (has_vui_parameters_present)
    {
        VideoUsabilityInfomationParameter vui_parameters;
        bool success = vui_parameters.Parse(bit_stream, sps_max_sub_layers);
        if (!success)
            return false;
    }
    return ParseExtensionInfo(bit_stream, ctb_log2_size_y_, chroma_format_idc);
}

uint32_t SequenceParameterSet::GetPicWidthInLumaSamples() const
{
    return pic_width_in_luma_samples_;
}

uint32_t SequenceParameterSet::GetPicHeightInLumaSamples() const
{
    return pic_height_in_luma_samples_;
}

uint32_t SequenceParameterSet::GetCTBLog2SizeY() const
{
    return ctb_log2_size_y_;
}

uint32_t SequenceParameterSet::GetLog2MinLumaTransformBlockSize() const
{
    return log2_min_luma_transform_block_size_;
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

    ShortTermReferencePictureSetContext short_term_rps_context(
        shared_from_this());
    for (uint32_t i = 0; i < num_short_term_ref_pic_sets; ++i)
    {
        unique_ptr<ShortTermReferencePictureSet> st_ref_pic_set(
            new ShortTermReferencePictureSet(i));

        bool success = 
            st_ref_pic_set->Parse(bit_stream, &short_term_rps_context);

        if (!success)
            return false;

        short_term_reference_picture_sets_.push_back(move(st_ref_pic_set));
    }

    has_long_term_ref_pics_present_ = bit_stream->ReadBool();

    if (has_long_term_ref_pics_present_)
    {
        uint32_t num_long_term_ref_pics_sps = golomb_reader.ReadUnsignedValue();
        for (uint32_t i = 0; i < num_long_term_ref_pics_sps; ++i)
        {
            LongTermReferenceLSBPictureOrderCountInfo info = { };
            info.lt_ref_pic_poc_lsb_sps = 
                bit_stream->Read<uint32_t>(log2_max_pic_order_cnt_lsb);
            info.is_used_by_curr_pic_lt_sps_flag = bit_stream->ReadBool();
            lt_ref_poc_infos_.push_back(info);
        }
    }
    return true;
}

uint32_t SequenceParameterSet::GetShortTermReferencePictureSetCount() const
{
    return short_term_reference_picture_sets_.size();
}

const ShortTermReferencePictureSet*
    SequenceParameterSet::GetShortTermReferencePictureSet(uint32_t index) const
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
        bool success = sps_range_extension_->Parse(bit_stream);
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
        SPS3DExtension sps_3d_extension;
        bool success = sps_3d_extension.Parse(bit_stream, ctb_log2_size_y);
        if (!success)
            return false;
    }
    if (has_sps_scc_extension)
    {
        bool success = sps_scc_extension_->Parse(bit_stream, chroma_format_idc);
        if (!success)
            return false;
    }
    return true;
}

uint32_t SequenceParameterSet::GetSliceSegmentAddressBitLength() const
{
    return slice_segment_address_bit_length_;
}

uint32_t SequenceParameterSet::GetPicOrderCountLSBBitLength() const
{
    return log2_max_pic_order_cnt_lsb_;
}

uint32_t SequenceParameterSet::GetMaxLSBOfPicOrderCount() const
{
    return max_lsb_of_pic_order_count_;
}

bool SequenceParameterSet::HasLongTermReferencePicturesPresent() const
{
    return has_long_term_ref_pics_present_;
}

const vector<LongTermReferenceLSBPictureOrderCountInfo>& 
    SequenceParameterSet::GetLongTermReferencePictureOrderCountInfos() const
{
    return lt_ref_poc_infos_;
}

bool SequenceParameterSet::IsTemporalMVPEnabled() const
{
    return is_sps_temporal_mvp_enabled_;
}

bool SequenceParameterSet::IsSampleAdaptiveOffsetEnabled() const
{
    return is_sample_adaptive_offset_enabled_;
}

ChromaFormatType SequenceParameterSet::GetChromaFormatType() const
{
    return chroma_array_type_;
}

const SPSRangeExtension& SequenceParameterSet::GetSPSRangeExtension() const
{
    return *sps_range_extension_;
}

uint32_t SequenceParameterSet::GetBitDepthChroma() const
{
    return bit_depth_chroma_;
}

const SPSScreenContentCodingExtension& 
    SequenceParameterSet::GetSPSScreenContentCodingExtension() const
{
    return *sps_scc_extension_;
}

uint32_t SequenceParameterSet::GetCTBHeight() const
{
    return ctb_height_;
}

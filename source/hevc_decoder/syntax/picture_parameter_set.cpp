#include "hevc_decoder/syntax/picture_parameter_set.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/scaling_list_data.h"
#include "hevc_decoder/syntax/pps_range_extension.h"
#include "hevc_decoder/syntax/pps_multilayer_extension.h"
#include "hevc_decoder/syntax/pps_3d_extension.h"
#include "hevc_decoder/syntax/pps_screen_content_coding_extension.h"

PictureParameterSet::PictureParameterSet()
    : pps_pic_parameter_set_id_(0)
    , pps_seq_parameter_set_id_(0)
    , is_dependent_slice_segments_enabled_(false)
    , num_extra_slice_header_bits_(0)
    , has_output_flag_present_(false)
    , has_lists_modification_present_(false)
    , pps_scc_extension_(new PPSScreenContentCodingExtension())
    , has_cabac_init_present_(false)
    , has_weighted_pred_(false)
    , has_weighted_bipred_(false)
    , has_pps_slice_chroma_qp_offsets_present_(false)
    , pps_range_extension_(new PPSRangeExtension())
    , deblocking_filter_control_info_()
    , is_pps_loop_filter_across_slices_enabled_(false)
    , is_tiles_enabled_(false)
    , is_entropy_coding_sync_enabled_(false)
{
    memset(&deblocking_filter_control_info_, 0, 
           sizeof(deblocking_filter_control_info_));
}

PictureParameterSet::~PictureParameterSet()
{

}

bool PictureParameterSet::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    pps_pic_parameter_set_id_ = golomb_reader.ReadUnsignedValue();
    pps_seq_parameter_set_id_ = golomb_reader.ReadUnsignedValue();
    is_dependent_slice_segments_enabled_ = bit_stream->ReadBool();
    has_output_flag_present_ = bit_stream->ReadBool();
    num_extra_slice_header_bits_ = bit_stream->Read<uint8_t>(3);
    bool is_sign_data_hiding_enabled = bit_stream->ReadBool();
    has_cabac_init_present_ = bit_stream->ReadBool();

    uint32_t num_ref_idx_l0_default_active = 
        golomb_reader.ReadUnsignedValue() + 1;
    uint32_t num_ref_idx_l1_default_active = 
        golomb_reader.ReadUnsignedValue() + 1;

    int init_qp = golomb_reader.ReadSignedValue() + 26;
    bool is_constrained_intra_pred = bit_stream->ReadBool();
    bool is_transform_skip_enabled = bit_stream->ReadBool();
    bool is_cu_qp_delta_enabled = bit_stream->ReadBool();
    if (is_cu_qp_delta_enabled)
        uint32_t diff_cu_qp_delta_depth = golomb_reader.ReadUnsignedValue();

    int pps_cb_qp_offset = golomb_reader.ReadSignedValue();
    int pps_cr_qp_offset = golomb_reader.ReadSignedValue();
    has_pps_slice_chroma_qp_offsets_present_ = bit_stream->ReadBool();
    has_weighted_pred_ = bit_stream->ReadBool();
    has_weighted_bipred_ = bit_stream->ReadBool();
    bool is_transquant_bypass_enabled = bit_stream->ReadBool();
    is_tiles_enabled_ = bit_stream->ReadBool();
    is_entropy_coding_sync_enabled_ = bit_stream->ReadBool();
    if (is_tiles_enabled_)
        ParseTileInfo(bit_stream);

    is_pps_loop_filter_across_slices_enabled_ = bit_stream->ReadBool();
    bool is_deblocking_filter_control_present = bit_stream->ReadBool();
    if (is_deblocking_filter_control_present)
        ParseDeblockingFilterControlInfo(bit_stream);

    bool is_pps_scaling_list_data_present = bit_stream->ReadBool();
    if (is_pps_scaling_list_data_present)
    {
        ScalingListData scaling_list_data;
        if (!scaling_list_data.Parse(bit_stream))
            return false;
    }

    has_lists_modification_present_ = bit_stream->ReadBool();
    uint32_t log2_parallel_merge_level = golomb_reader.ReadUnsignedValue() + 2;
    bool has_slice_segment_header_extension_present = bit_stream->ReadBool();
    bool has_pps_extension_present = bit_stream->ReadBool();
    if (has_pps_extension_present)
        if (!ParsePPSExtensionInfo(is_transform_skip_enabled, bit_stream))
            return false;

    return true;
}

void PictureParameterSet::ParseTileInfo(BitStream* bit_stream)
{
    GolombReader golomb_reader(bit_stream);
    uint32_t num_tile_columns_minus1 = golomb_reader.ReadUnsignedValue();
    uint32_t num_tile_rows_minus1 = golomb_reader.ReadUnsignedValue();
    TileInfo tile_info = {};
    tile_info.is_uniform_spacing = bit_stream->ReadBool();
    if (!tile_info.is_uniform_spacing)
    {
        for (uint32_t i = 0; i < num_tile_columns_minus1; ++i)
        {
            uint32_t single_width = golomb_reader.ReadUnsignedValue() + 1;
            tile_info.column_width.push_back(single_width);
        }

        for (uint32_t i = 0; i < num_tile_rows_minus1; ++i)
        {
            uint32_t single_height = golomb_reader.ReadUnsignedValue() + 1;
            tile_info.row_height.push_back(single_height);
        }
        tile_info.loop_filter_across_tiles_enabled = bit_stream->ReadBool();
    }
}

void PictureParameterSet::ParseDeblockingFilterControlInfo(BitStream* bit_stream)
{
    
    deblocking_filter_control_info_.is_deblocking_filter_override_enabled = 
        bit_stream->ReadBool();

    deblocking_filter_control_info_.is_pps_deblocking_filter_disabled = 
        bit_stream->ReadBool();

    GolombReader golomb_reader(bit_stream);
    deblocking_filter_control_info_.beta_offset = 
        golomb_reader.ReadSignedValue() * 2;

    deblocking_filter_control_info_.tc_offset = 
        golomb_reader.ReadSignedValue() * 2;
}

bool PictureParameterSet::ParsePPSExtensionInfo(bool is_transform_skip_enabled, 
                                                BitStream* bit_stream)
{
    bool has_pps_range_extension = bit_stream->ReadBool();
    bool has_pps_multilayer_extension = bit_stream->ReadBool();
    bool has_pps_3d_extension = bit_stream->ReadBool();
    bool has_pps_scc_extension = bit_stream->ReadBool();
    bit_stream->SkipBits(4);
    if (has_pps_range_extension)
    {
        if (!pps_range_extension_->Parse(bit_stream, is_transform_skip_enabled))
            return false;
    }
    if (has_pps_multilayer_extension)
    {
        PPSMultilayerExtension pps_multilayer_extension;
        if (!pps_multilayer_extension.Parse(bit_stream))
            return false;
    }
    if (has_pps_3d_extension)
    {
        PPS3DExtension pps_3d_extension;
        if (!pps_3d_extension.Parse(bit_stream))
            return false;
    }
    if (has_pps_scc_extension)
    {
        if (!pps_scc_extension_->Parse(bit_stream))
            return false;
    }
    return true;
}

uint32_t PictureParameterSet::GetPictureParameterSetID()
{
    return pps_pic_parameter_set_id_;
}

bool PictureParameterSet::IsDependentSliceSegmentEnabled() const
{
    return is_dependent_slice_segments_enabled_;
}

uint32_t PictureParameterSet::GetAssociateSequenceParameterSetID() const
{
    return pps_seq_parameter_set_id_;
}

uint8_t PictureParameterSet::GetExtraSliceHeaderBitLength() const
{
    return num_extra_slice_header_bits_;
}

bool PictureParameterSet::HasOutputFlagPresent() const
{
    return has_output_flag_present_;
}

bool PictureParameterSet::HasListsModificationPresent() const
{
    return has_lists_modification_present_;
}

const PPSScreenContentCodingExtension* PictureParameterSet::GetPPSSccExtension()
    const
{
    return pps_scc_extension_.get();
}

bool PictureParameterSet::HasCABACInitPresent() const
{
    return has_cabac_init_present_;
}

bool PictureParameterSet::HasWeightedPred() const
{
    return has_weighted_pred_;
}

bool PictureParameterSet::HasWeightedBipred() const
{
    return has_weighted_bipred_;
}

bool PictureParameterSet::HasPPSSliceChromaQPOffsetPresent() const
{
    return has_pps_slice_chroma_qp_offsets_present_;
}

const PPSRangeExtension* PictureParameterSet::GetPPSRangeExtension() const
{
    return pps_range_extension_.get();
}

bool PictureParameterSet::IsDeblockingFilterOverrideEnabled() const
{
    return deblocking_filter_control_info_.is_deblocking_filter_override_enabled;
}

bool PictureParameterSet::IsPPSLoopFilterAcrossSliceEnabled() const
{
    return is_pps_loop_filter_across_slices_enabled_;
}

bool PictureParameterSet::IsDeblockingFilterDisabled() const
{
    return deblocking_filter_control_info_.is_pps_deblocking_filter_disabled;
}

bool PictureParameterSet::IsTilesEnabled() const
{
    return is_tiles_enabled_;
}

bool PictureParameterSet::IsEntropyCodingSyncEnabled() const
{
    return is_entropy_coding_sync_enabled_;
}

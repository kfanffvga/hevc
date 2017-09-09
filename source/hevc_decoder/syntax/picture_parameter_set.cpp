#include "hevc_decoder/syntax/picture_parameter_set.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/scaling_list_data.h"
#include "hevc_decoder/syntax/pps_range_extension.h"
#include "hevc_decoder/syntax/pps_multilayer_extension.h"
#include "hevc_decoder/syntax/pps_3d_extension.h"
#include "hevc_decoder/syntax/pps_screen_content_coding_extension.h"


PictureParameterSet::PictureParameterSet()
{

}

PictureParameterSet::~PictureParameterSet()
{

}

bool PictureParameterSet::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    uint32_t pps_pic_parameter_set_id = golomb_reader.ReadUnsignedValue();
    uint32_t pps_seq_parameter_set_id = golomb_reader.ReadUnsignedValue();
    bool is_dependent_slice_segments_enabled = bit_stream->ReadBool();
    bool has_output_flag_present = bit_stream->ReadBool();
    uint8_t num_extra_slice_header_bits = bit_stream->Read<uint8_t>(3);
    bool is_sign_data_hiding_enabled = bit_stream->ReadBool();
    bool has_cabac_init_present = bit_stream->ReadBool();

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
    bool has_pps_slice_chroma_qp_offsets_present = bit_stream->ReadBool();
    bool has_weighted_pred = bit_stream->ReadBool();
    bool has_weighted_bipred = bit_stream->ReadBool();
    bool is_transquant_bypass_enabled = bit_stream->ReadBool();
    bool is_tiles_enabled = bit_stream->ReadBool();
    bool is_entropy_coding_sync_enabled = bit_stream->ReadBool();
    if (is_tiles_enabled)
        ParseTileInfo(bit_stream);

    bool pps_loop_filter_across_slices_enabled = bit_stream->ReadBool();
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

    bool has_lists_modification_present = bit_stream->ReadBool();
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
    DeblockingFilterControlInfo control_info = { };
    control_info.deblocking_filter_override_enabled = bit_stream->ReadBool();
    control_info.pps_deblocking_filter_disabled = bit_stream->ReadBool();
    GolombReader golomb_reader(bit_stream);
    control_info.beta_offset = golomb_reader.ReadSignedValue() * 2;
    control_info.tc_offset = golomb_reader.ReadSignedValue() * 2;
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
        PPSRangeExtension pps_range_extension(is_transform_skip_enabled);
        if (!pps_range_extension.Parse(bit_stream))
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
        PPSScreenContentCodingExtension pps_scc_extension;
        if (!pps_scc_extension.Parse(bit_stream))
            return false;
    }
    return true;
}
#include "hevc_decoder/syntax/sps_3d_extension.h"

#include <vector>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

using std::vector;

SPS3DExtension::SPS3DExtension()
{

}

SPS3DExtension::~SPS3DExtension()
{

}

bool SPS3DExtension::Parse(BitStream* bit_stream, uint32_t ctb_log2_size_y)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    vector<SPS3DExtensionItem> sps_3d_extension_items;
    for (int i = 0; i < 2; ++i)
    {
        SPS3DExtensionItem item = { };
        item.is_iv_di_mc_enabled = bit_stream->ReadBool();
        item.is_iv_mv_scal_enabled = bit_stream->ReadBool();
        if (0 == i)
        {
            item.log2_ivmc_sub_pb_size = golomb_reader.ReadUnsignedValue() + 3;
            item.is_iv_res_pred_enabled = bit_stream->ReadBool();
            item.is_depth_ref_enabled = bit_stream->ReadBool();
            item.is_vsp_mc_enabled = bit_stream->ReadBool();
            item.is_dbbp_enabled = bit_stream->ReadBool();
            item.is_tex_mc_enabled = false;
            item.log2_texmc_sub_pb_size = 0; // 文档中没有写明白初始值
            item.is_intra_contour_enabled = false;
            item.is_intra_dc_only_wedge_enabled = false; 
            item.is_cqt_cu_part_pred_enabled = false;
            item.is_inter_dc_only_enabled = false;
            item.is_skip_intra_enabled = false;
        }
        else
        {
            item.log2_ivmc_sub_pb_size = ctb_log2_size_y;
            item.is_iv_res_pred_enabled = false;
            item.is_depth_ref_enabled = false;
            item.is_vsp_mc_enabled = false;
            item.is_dbbp_enabled = false;
            item.is_tex_mc_enabled = bit_stream->ReadBool();
            item.log2_texmc_sub_pb_size = golomb_reader.ReadUnsignedValue() + 3;
            item.is_intra_contour_enabled = bit_stream->ReadBool();
            item.is_intra_dc_only_wedge_enabled = bit_stream->ReadBool();
            item.is_cqt_cu_part_pred_enabled = bit_stream->ReadBool();
            item.is_inter_dc_only_enabled = bit_stream->ReadBool();
            item.is_skip_intra_enabled = bit_stream->ReadBool();
        }
        sps_3d_extension_items.push_back(item);
    }
    return true;
}

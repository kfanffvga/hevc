#include "hevc_decoder/syntax/pps_multilayer_extension.h"

#include <memory>
#include <utility>
#include <map>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/colour_mapping_table.h"

using std::map;
using std::make_pair;

PPSMultilayerExtension::PPSMultilayerExtension()
    : BaseSyntax()
{

}

PPSMultilayerExtension::~PPSMultilayerExtension()
{

}

bool PPSMultilayerExtension::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool has_poc_reset_info_present = bit_stream->ReadBool();
    bool has_pps_infer_scaling_list = bit_stream->ReadBool();
    if (has_pps_infer_scaling_list)
        uint8_t pps_scaling_list_ref_layer_id = bit_stream->Read<uint8_t>(6);

    GolombReadr golomb_reader(bit_stream);
    uint32_t num_ref_loc_offsets = golomb_reader.ReadUnsignedValue();

    map<uint32_t, ScaledRefLayerInfo> scaled_ref_layer_infos;
    map<uint32_t, RefRegionInfo> ref_region_infos;
    map<uint32_t, PhaseInfo> phase_infos;
    for (uint32_t i = 0; i < num_ref_loc_offsets; ++i)
    {
        uint32_t ref_loc_offset_layer_id = bit_stream->Read<uint32_t>(6);
        bool is_caled_ref_layer_offset_present = bit_stream->ReadBool();
        if (is_caled_ref_layer_offset_present)
        {
            ScaledRefLayerInfo info = { };
            info.left_offset = golomb_reader.ReadSignedValue();
            info.top_offset = golomb_reader.ReadSignedValue();
            info.right_offset = golomb_reader.ReadSignedValue();
            info.bottom_offset = golomb_reader.ReadSignedValue();
            scaled_ref_layer_infos.insert(
                make_pair(ref_loc_offset_layer_id, info));
        }

        bool is_ref_region_offset_present = bit_stream->ReadBool();
        if (is_ref_region_offset_present)
        {
            RefRegionInfo info = { };
            info.left_offset = golomb_reader.ReadSignedValue();
            info.top_offset = golomb_reader.ReadSignedValue();
            info.right_offset = golomb_reader.ReadSignedValue();
            info.bottom_offset = golomb_reader.ReadSignedValue();
            ref_region_infos.insert(make_pair(ref_loc_offset_layer_id, info));
        }

        bool is_resample_phase_set_present = bit_stream->ReadBool();
        if (is_resample_phase_set_present)
        {
            PhaseInfo info = { };
            info.hor_luma = golomb_reader.ReadUnsignedValue();
            info.ver_luma = golomb_reader.ReadUnsignedValue();
            info.hor_chroma = golomb_reader.ReadUnsignedValue() - 8;
            info.ver_chroma = golomb_reader.ReadUnsignedValue() - 8;
            phase_infos.insert(make_pair(ref_loc_offset_layer_id, info));
        }
    }

    bool is_colour_mapping_enabled = bit_stream->ReadBool();
    if (is_colour_mapping_enabled)
    {
        ColourMappingTable colour_mapping_table;
        return colour_mapping_table.Parse(bit_stream);
    }
    return true;
}

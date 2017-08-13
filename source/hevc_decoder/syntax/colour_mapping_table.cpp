#include "hevc_decoder/syntax/colour_mapping_table.h"

#include <vector>
#include <algorithm>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/colour_mapping_octants.h"

using std::vector;
using std::unique_ptr;
using std::max;

ColourMappingTable::ColourMappingTable()
{

}

ColourMappingTable::~ColourMappingTable()
{

}

bool ColourMappingTable::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    GolombReadr golomb_reader(bit_stream);
    uint32_t num_cm_ref_layers = golomb_reader.ReadUnsignedValue();
    vector<uint8_t> cm_ref_layer_ids;
    for (uint32_t i = 0; i < num_cm_ref_layers; ++i)
        cm_ref_layer_ids.push_back(bit_stream->Read<uint8_t>(6));

    uint8_t cm_octant_depth = bit_stream->Read<uint8_t>(2);
    uint8_t cm_y_part_num_log2 = bit_stream->Read<uint8_t>(2);

    uint32_t luma_bit_depth_cm_input = golomb_reader.ReadUnsignedValue() + 8;
    uint32_t chroma_bit_depth_cm_input = golomb_reader.ReadUnsignedValue() + 8;
    uint32_t luma_bit_depth_cm_output = golomb_reader.ReadUnsignedValue() + 8;
    uint32_t chroma_bit_depth_cm_output = golomb_reader.ReadUnsignedValue() + 8;

    uint8_t cm_res_quant_bits = bit_stream->Read<uint8_t>(2);
    uint8_t cm_delta_flc_bits = bit_stream->Read<uint8_t>(2) + 1;

    if (1 == cm_octant_depth)
    {
        int32_t cm_adapt_threshold_u_delta = golomb_reader.ReadSignedValue();
        int32_t cm_adapt_threshold_v_delta = golomb_reader.ReadSignedValue();
    }
    uint8_t cm_res_coeff_r_bits = 10 + luma_bit_depth_cm_input - 
        luma_bit_depth_cm_output - cm_res_quant_bits - cm_delta_flc_bits;
    cm_res_coeff_r_bits = max(0ui8, cm_res_coeff_r_bits);

    unique_ptr<ColourMappingOctants> colour_mapping_octants(
        new ColourMappingOctants(cm_y_part_num_log2, cm_octant_depth,
                                 cm_res_coeff_r_bits));
    return colour_mapping_octants->Parse(bit_stream);;
}

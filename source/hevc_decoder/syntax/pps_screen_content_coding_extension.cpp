#include "hevc_decoder/syntax/pps_screen_content_coding_extension.h"

#include <boost/multi_array.hpp>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

using boost::multi_array;
using boost::extents;

PPSScreenContentCodingExtension::PPSScreenContentCodingExtension()
{

}

PPSScreenContentCodingExtension::~PPSScreenContentCodingExtension()
{

}

bool PPSScreenContentCodingExtension::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool is_pps_curr_pic_ref_enabled = bit_stream->ReadBool();
    bool is_residual_adaptive_colour_transform_enabled = bit_stream->ReadBool();

    GolombReadr golomb_reader(bit_stream);
    if (is_residual_adaptive_colour_transform_enabled)
    {
        bool has_pps_slice_act_qp_offsets_present = bit_stream->ReadBool();
        int32_t pps_act_y_qp_offset = golomb_reader.ReadSignedValue() - 5;
        int32_t pps_act_cb_qp_offset = golomb_reader.ReadSignedValue() - 5;
        int32_t pps_act_cr_qp_offset = golomb_reader.ReadSignedValue() - 3;
    }

    bool has_pps_palette_predictor_initializer_present = bit_stream->ReadBool();
    if (has_pps_palette_predictor_initializer_present)
    {
        uint32_t pps_num_palette_predictor_initializer = 
            golomb_reader.ReadUnsignedValue();

        if (pps_num_palette_predictor_initializer > 0)
        {
            bool has_monochrome_palette = bit_stream->ReadBool();
            uint32_t luma_bit_depth_entry = 
                golomb_reader.ReadUnsignedValue() + 8;

            uint32_t chroma_bit_depth_entry = 0;
            if (!has_monochrome_palette)
                chroma_bit_depth_entry = golomb_reader.ReadUnsignedValue() + 8;

            uint32_t num_comps = has_monochrome_palette ? 1 : 3;

            multi_array<uint32_t, 2> pps_palette_predictor_initializers(
                extents[num_comps][pps_num_palette_predictor_initializer]);
            for (uint32_t i = 0; i < num_comps; ++i)
            {
                uint32_t initializer_length = 
                    0 == i ? luma_bit_depth_entry : chroma_bit_depth_entry;
                uint32_t j = 0;
                for (; j < pps_num_palette_predictor_initializer; ++j)
                {
                    pps_palette_predictor_initializers[i][j] = 
                        bit_stream->Read<uint32_t>(initializer_length);
                }
            }
        }
    }
    return true;
}

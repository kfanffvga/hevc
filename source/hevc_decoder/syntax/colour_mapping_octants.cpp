#include "hevc_decoder/syntax/colour_mapping_octants.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

ColourMappingOctants::ColourMappingOctants(uint8_t cm_y_part_num_log2, 
                                           uint8_t cm_octant_depth,
                                           uint8_t cm_res_coeff_r_bits)
    : y_part_num_(1 << cm_y_part_num_log2)
    , cm_octant_depth_(cm_octant_depth)
    , cm_res_coeff_r_bits_(cm_res_coeff_r_bits)
{

}

ColourMappingOctants::~ColourMappingOctants()
{

}

bool ColourMappingOctants::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    ResidualCoefficient res_coeff_q;
    ResidualCoefficient res_coeff_r;
    ResidualCoefficient res_coeff_s;
    ColourIndex colour_index = {0, 0, 0};
    ReadColourMappingOctantsElement(bit_stream, &res_coeff_q, &res_coeff_r, 
                                    &res_coeff_s, 0, colour_index, 
                                    1 << cm_octant_depth_);
    return true;
}

void ColourMappingOctants::ReadColourMappingOctantsElement(
    BitStream* bit_stream, ResidualCoefficient* res_coeff_q, 
    ResidualCoefficient* res_coeff_r, ResidualCoefficient* res_coeff_s, 
    uint32_t current_depth, ColourIndex colour_index, uint32_t inp_length)
{
    bool is_split_octant = false;
    if (current_depth < cm_octant_depth_)
        is_split_octant = bit_stream->ReadBool();

    if (is_split_octant)
    {
        for (uint32_t y = 0; y < 2; ++y)
        {
            for (uint32_t cb = 0; cb < 2; ++cb)
            {
                for (uint32_t cr = 0; cr < 2; ++cr)
                {
                    uint32_t inp_length_div_2 = inp_length >> 1;
                    colour_index.y += y_part_num_ * y * inp_length_div_2;
                    colour_index.cb += cb * inp_length_div_2;
                    colour_index.cr += cr * inp_length_div_2;
                    ReadColourMappingOctantsElement(bit_stream, res_coeff_q, 
                                                    res_coeff_r, res_coeff_s, 
                                                    current_depth + 1, 
                                                    colour_index, 
                                                    inp_length_div_2);
                }
            }
        }
    }
    else
    {
        ColourIndex current_colour_index = colour_index;
        uint32_t delta_depth = cm_octant_depth_ - current_depth;
        GolombReadr golomb_reader(bit_stream);
        for (uint32_t i = 0; i < y_part_num_; ++i)
        {
            current_colour_index.y = colour_index.y + (i << delta_depth);
            for (uint32_t j = 0; j < 4; ++j)
            {
                bool has_coded_res = bit_stream->ReadBool();
                if (has_coded_res)
                {
                    for (uint32_t c = 0; c < 3; ++c)
                    {
                        uint32_t q = golomb_reader.ReadUnsignedValue();
                        (*res_coeff_q)[current_colour_index][j][c] = q;

                        uint32_t r = 
                            bit_stream->Read<uint32_t>(cm_res_coeff_r_bits_);
                        (*res_coeff_r)[current_colour_index][j][c] = 
                            0 == cm_res_coeff_r_bits_ ? 0 : r;

                        if (q || r)
                        {
                            (*res_coeff_s)[current_colour_index][j][c] =
                                bit_stream->Read<uint32_t>(1);
                        }   
                    }
                }
            }
        }
    }
}

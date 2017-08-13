#ifndef _COLOUR_MAPPING_OCTANTS_H_
#define _COLOUR_MAPPING_OCTANTS_H_

#include <stdint.h>
#include <map>
#include <array>

#include "hevc_decoder/syntax/base_syntax.h"

// TODO: 此处并不理解8叉树的分割方法,特别是句法结构中的
// colour_mapping_octants( inpDepth, idxY, idxCb, idxCr, inpLength )
// inpLength的意义,由于此处是在multilayer中使用,暂时性不考虑语义,留到将来再解决

class ColourMappingOctants : public BaseSyntax
{
public:
    ColourMappingOctants(uint8_t cm_y_part_num_log2, uint8_t cm_octant_depth,
                         uint8_t cm_res_coeff_r_bits);
    virtual ~ColourMappingOctants();

    virtual bool Parse(BitStream* bit_stream) override;

private:
    struct ColourIndex
    {
        uint32_t y;
        uint32_t cb;
        uint32_t cr;

        bool operator < (const ColourIndex& a)const
        {
            uint64_t this_value = 
                (static_cast<uint64_t>(this->y) << 32) | 
                static_cast<uint64_t>(this->cb);

            uint64_t a_value = 
                (static_cast<uint64_t>(a.y) << 32) | static_cast<uint64_t>(a.cb);
            if (this_value < a_value)
                return true;
            else if (this_value > a_value)
                return false;

            return this->cr < a.cr;
        }
    };
    typedef std::array<std::array<uint32_t, 3>, 4> CoefficientTable;
    typedef std::map<ColourIndex, CoefficientTable> ResidualCoefficient;

    void ReadColourMappingOctantsElement(BitStream* bit_stream, 
                                         ResidualCoefficient* res_coeff_q,
                                         ResidualCoefficient* res_coeff_r,
                                         ResidualCoefficient* res_coeff_s,
                                         uint32_t current_depth, 
                                         ColourIndex colour_index, 
                                         uint32_t inp_length);

    uint32_t y_part_num_;
    uint32_t cm_octant_depth_;
    uint32_t cm_res_coeff_r_bits_;
};

#endif
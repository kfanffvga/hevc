#include "hevc_decoder/syntax/hrd_parameters.h"

#include <boost/multi_array.hpp>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/sub_layer_hrd_parameters.h"

using boost::multi_array;
using boost::extents;
using boost::detail::multi_array::extent_gen;
using std::vector;

namespace
{
const uint32 numerals_twenty_three = 23;
}

HrdParmeters::HrdParmeters(bool commin_inf_present, int max_num_sub_layers)
                           : BaseSyntax()
                           , commin_inf_present_(commin_inf_present)
                           , max_num_sub_layers_(max_num_sub_layers)
                           , nal_sub_layer_hrd_parameters_(max_num_sub_layers_)
                           , vcl_sub_layer_hrd_parameters_(max_num_sub_layers_)
{

}

HrdParmeters::~HrdParmeters()
{

}

bool HrdParmeters::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool has_nal_hrd_parmeters_present = false;
    bool has_vcl_hrd_parameters_present = false;
    bool has_sub_pic_hrd_params_present = false;
    if (commin_inf_present_)
    {
        has_nal_hrd_parmeters_present = bit_stream->ReadBool();
        has_vcl_hrd_parameters_present = bit_stream->ReadBool();
        uint8 initial_cpb_removal_delay_length = numerals_twenty_three;
        uint8 au_cpb_removal_delay_length = numerals_twenty_three;
        uint8 dpb_output_delay_length = numerals_twenty_three;
        if (has_nal_hrd_parmeters_present || has_vcl_hrd_parameters_present)
        {
            has_sub_pic_hrd_params_present = bit_stream->ReadBool();
            if (has_sub_pic_hrd_params_present)
            {
                uint8 tick_divisor = bit_stream->Read<uint8>(8) + 2;
                uint8 du_cpb_removal_delay_increment_length =
                    bit_stream->Read<uint8>(5) + 1;
                bool is_sub_pic_cpb_params_in_pic_timing_sei =
                    bit_stream->ReadBool();
                uint8 dpb_output_delay_du_length =
                    bit_stream->Read<uint8>(5) + 1;
            }

            uint8 bit_rate_scale = bit_stream->Read<uint8>(4);
            uint8 cpb_size_scale = bit_stream->Read<uint8>(4);
            uint8 cpb_size_du_scale = 0;
            if (has_sub_pic_hrd_params_present)
                cpb_size_du_scale = bit_stream->Read<uint8>(4);

            initial_cpb_removal_delay_length = bit_stream->Read<uint8>(5) + 1;
            au_cpb_removal_delay_length = bit_stream->Read<uint8>(5) + 1;
            dpb_output_delay_length = bit_stream->Read<uint8>(5) + 1;
        }
    }

    extent_gen<1> alloc_size = extents[max_num_sub_layers_];
    multi_array<bool, 1> is_fixed_pic_rate_general(alloc_size);
    multi_array<bool, 1> is_fixed_pic_rate_within_cvs(alloc_size);    
    multi_array<bool, 1> is_low_delay_hrd(alloc_size);
    vector<uint32> elemental_duration_in_tc(max_num_sub_layers_);
    vector<uint32> cpb_cnt(max_num_sub_layers_);
    GolombReader golomb_reader(bit_stream);
    for (int i = 0; i <= max_num_sub_layers_; ++i)
    {
        is_fixed_pic_rate_general[i] = bit_stream->ReadBool();
        is_fixed_pic_rate_within_cvs[i] = is_fixed_pic_rate_general[i] ?
            is_fixed_pic_rate_general[i] : bit_stream->ReadBool();

        if (is_fixed_pic_rate_within_cvs[i])
        {
            elemental_duration_in_tc[i] =
                golomb_reader.ReadUnsignedValue() + 1;
            assert(elemental_duration_in_tc[i] >= 0 &&
                   elemental_duration_in_tc[i] <= 2047);
            is_low_delay_hrd[i] = false;
        }
        else
        {
            is_low_delay_hrd[i] = bit_stream->ReadBool();
        }

        cpb_cnt[i] = is_low_delay_hrd[i] ? 
            0 : (golomb_reader.ReadUnsignedValue() + 1);
        assert(cpb_cnt[i] >= 0 && cpb_cnt[i] <= 31);

        if (has_nal_hrd_parmeters_present)
        {
            nal_sub_layer_hrd_parameters_[i].reset(new SubLayerHrdParameters(
                cpb_cnt[i], has_sub_pic_hrd_params_present));
        }

        if (has_vcl_hrd_parameters_present)
        {
            vcl_sub_layer_hrd_parameters_[i].reset(new SubLayerHrdParameters(
                cpb_cnt[i], has_sub_pic_hrd_params_present));
        }
    }

    return true;
}
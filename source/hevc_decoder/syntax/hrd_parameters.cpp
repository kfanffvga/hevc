#include "hevc_decoder/syntax/hrd_parameters.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/sub_layer_hrd_parameters.h"

using std::vector;
using std::unique_ptr;

HrdParmeters::HrdParmeters()
{

}

HrdParmeters::~HrdParmeters()
{

}

bool HrdParmeters::Parse(BitStream* bit_stream, bool has_commin_inf_present,
                         uint32_t max_num_sub_layers)
{
    if (!bit_stream)
        return false;

    bool has_nal_hrd_parmeters_present = false;
    bool has_vcl_hrd_parameters_present = false;
    bool has_sub_pic_hrd_params_present = false;
    if (has_commin_inf_present)
    {
        has_nal_hrd_parmeters_present = bit_stream->ReadBool();
        has_vcl_hrd_parameters_present = bit_stream->ReadBool();
        uint8_t initial_cpb_removal_delay_length = 23;
        uint8_t au_cpb_removal_delay_length = 23;
        uint8_t dpb_output_delay_length = 23;
        if (has_nal_hrd_parmeters_present || has_vcl_hrd_parameters_present)
        {
            has_sub_pic_hrd_params_present = bit_stream->ReadBool();
            if (has_sub_pic_hrd_params_present)
            {
                uint8_t tick_divisor = bit_stream->Read<uint8_t>(8) + 2;
                uint8_t du_cpb_removal_delay_increment_length =
                    bit_stream->Read<uint8_t>(5) + 1;
                bool is_sub_pic_cpb_params_in_pic_timing_sei =
                    bit_stream->ReadBool();
                uint8_t dpb_output_delay_du_length =
                    bit_stream->Read<uint8_t>(5) + 1;
            }

            uint8_t bit_rate_scale = bit_stream->Read<uint8_t>(4);
            uint8_t cpb_size_scale = bit_stream->Read<uint8_t>(4);
            uint8_t cpb_size_du_scale = 0;
            if (has_sub_pic_hrd_params_present)
                cpb_size_du_scale = bit_stream->Read<uint8_t>(4);

            initial_cpb_removal_delay_length = bit_stream->Read<uint8_t>(5) + 1;
            au_cpb_removal_delay_length = bit_stream->Read<uint8_t>(5) + 1;
            dpb_output_delay_length = bit_stream->Read<uint8_t>(5) + 1;
        }
    }

    GolombReader golomb_reader(bit_stream);
    for (uint32_t i = 0; i <= max_num_sub_layers; ++i)
    {
        bool is_fixed_pic_rate_general = bit_stream->ReadBool();
        bool is_fixed_pic_rate_within_cvs = is_fixed_pic_rate_general;
        if (!is_fixed_pic_rate_general)
            is_fixed_pic_rate_within_cvs = bit_stream->ReadBool();

        bool is_low_delay_hrd = false;
        if (is_fixed_pic_rate_within_cvs)
        {
            uint32_t elemental_duration_in_tc =
                golomb_reader.ReadUnsignedValue() + 1;
        }
        else
        {
            is_low_delay_hrd = bit_stream->ReadBool();
        }

        uint32_t cpb_cnt = 
            is_low_delay_hrd ? 0 : (golomb_reader.ReadUnsignedValue() + 1);
        
        vector<unique_ptr<SubLayerHrdParameters>> nal_sub_layer_hrd_parameters;
        vector<unique_ptr<SubLayerHrdParameters>> vcl_sub_layer_hrd_parameters;
        unique_ptr<SubLayerHrdParameters> parameter(new SubLayerHrdParameters());
        bool success = parameter->Parse(bit_stream, cpb_cnt, 
                                        has_sub_pic_hrd_params_present);
        if (!success)
            return false;

        if (has_nal_hrd_parmeters_present)
            nal_sub_layer_hrd_parameters.push_back(move(parameter));

        if (has_vcl_hrd_parameters_present)
            vcl_sub_layer_hrd_parameters.push_back(move(parameter));
    }

    return true;
}
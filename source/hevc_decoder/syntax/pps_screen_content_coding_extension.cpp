#include "hevc_decoder/syntax/pps_screen_content_coding_extension.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/palette_table.h"

PPSScreenContentCodingExtension::PPSScreenContentCodingExtension()
    : is_pps_curr_pic_ref_enabled_(false)
    , has_pps_slice_act_qp_offsets_present_(false)
    , has_pps_palette_predictor_initializer_present_(false)
    , palette_predictor_initializer_(new PaletteTable())
    , is_residual_adaptive_colour_transform_enabled_(false)
{

}

PPSScreenContentCodingExtension::~PPSScreenContentCodingExtension()
{

}

bool PPSScreenContentCodingExtension::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    is_pps_curr_pic_ref_enabled_ = bit_stream->ReadBool();
    is_residual_adaptive_colour_transform_enabled_ = bit_stream->ReadBool();

    GolombReader golomb_reader(bit_stream);
    if (is_residual_adaptive_colour_transform_enabled_)
    {
        has_pps_slice_act_qp_offsets_present_ = bit_stream->ReadBool();
        int32_t pps_act_y_qp_offset = golomb_reader.ReadSignedValue() - 5;
        int32_t pps_act_cb_qp_offset = golomb_reader.ReadSignedValue() - 5;
        int32_t pps_act_cr_qp_offset = golomb_reader.ReadSignedValue() - 3;
    }

    has_pps_palette_predictor_initializer_present_ = bit_stream->ReadBool();
    if (has_pps_palette_predictor_initializer_present_)
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
            palette_predictor_initializer_->Init(num_comps);
            for (uint32_t i = 0; i < num_comps; ++i)
            {
                uint32_t initializer_length = 
                    0 == i ? luma_bit_depth_entry : chroma_bit_depth_entry;
                for (uint32_t j = 0; j < pps_num_palette_predictor_initializer; 
                     ++j)
                {
                    uint32_t palette_value = 
                        bit_stream->Read<uint32_t>(initializer_length);
                    palette_predictor_initializer_->SetValue(j, i, 
                                                             palette_value);
                }
            }
        }
    }
    return true;
}

bool PPSScreenContentCodingExtension::IsPPSCurrentPictureReferenceEnabled() 
    const
{
    return is_pps_curr_pic_ref_enabled_;
}

bool PPSScreenContentCodingExtension::HasPPSSliceActQPOffsetsPresent() const
{
    return has_pps_slice_act_qp_offsets_present_;
}

bool PPSScreenContentCodingExtension::HasPPSPalettePredictorInitializerPresent() const
{
    return has_pps_palette_predictor_initializer_present_;
}

const PaletteTable& 
    PPSScreenContentCodingExtension::GetPalettePredictorInitializer() const
{
    return *palette_predictor_initializer_;
}

bool PPSScreenContentCodingExtension::IsResidualAdaptiveColorTransformEnabled() const
{
    return is_residual_adaptive_colour_transform_enabled_;
}

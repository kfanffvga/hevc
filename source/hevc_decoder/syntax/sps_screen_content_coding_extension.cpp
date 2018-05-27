#include "hevc_decoder/syntax/sps_screen_content_coding_extension.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/syntax/palette_table.h"

SPSScreenContentCodingExtension::SPSScreenContentCodingExtension()
    : motion_vector_resolution_control_idc_(0)
    , is_palette_mode_enabled_(false)
    , has_sps_palette_predictor_initializer_present_(false)
    , palette_predictor_initializer_(new PaletteTable())
    , palette_max_size_(0)
    , predictor_palette_max_size_(0)
{

}

SPSScreenContentCodingExtension::~SPSScreenContentCodingExtension()
{

}

bool SPSScreenContentCodingExtension::Parse(BitStream* bit_stream,
                                            uint32_t chroma_format_idc,
                                            uint32_t bit_depth_luma, 
                                            uint32_t bit_depth_chroma)
{
    if (!bit_stream)
        return false;

    bool is_sps_curr_pic_ref_enabled = bit_stream->ReadBool();
    is_palette_mode_enabled_ = bit_stream->ReadBool();
    if (is_palette_mode_enabled_)
    {
        GolombReader golomb_reader(bit_stream);
        palette_max_size_ = golomb_reader.ReadUnsignedValue();
        uint32_t delta_palette_max_predictor_size = 
            golomb_reader.ReadUnsignedValue();

        predictor_palette_max_size_ = 
            palette_max_size_ + delta_palette_max_predictor_size;

        has_sps_palette_predictor_initializer_present_ = bit_stream->ReadBool();
        if (has_sps_palette_predictor_initializer_present_)
        {
            uint32_t sps_num_palette_predictor_initializer = 
                golomb_reader.ReadUnsignedValue() + 1;

            uint32_t num_of_color_compoments = (0 == chroma_format_idc) ? 1 : 3;
            palette_predictor_initializer_->Init(num_of_color_compoments);
            // 读取的大小纯属猜测,文档中没有提及,FFMPEG中也没有读取
            for (uint32_t i = 0; i < num_of_color_compoments; ++i)
            {
                uint32_t bits = 0 == i ? bit_depth_luma : bit_depth_chroma;
                for (uint32_t j = 0; j < sps_num_palette_predictor_initializer;
                     ++j)
                {
                    uint32_t palette_value = bit_stream->Read<uint32_t>(bits);
                    palette_predictor_initializer_->SetValue(j, i, 
                                                             palette_value);
                }
            }
        }
    }
    motion_vector_resolution_control_idc_ = bit_stream->Read<uint8_t>(2);
    bool is_intra_boundary_filtering_disabled = bit_stream->ReadBool();
    return true;
}

uint8_t SPSScreenContentCodingExtension::GetMotionVectorResolutionControlIDC() 
    const
{
    return motion_vector_resolution_control_idc_;
}

bool SPSScreenContentCodingExtension::IsPaletteModeEnabled() const
{
    return is_palette_mode_enabled_;
}

bool SPSScreenContentCodingExtension::HasSPSPalettePredictorInitializerPresent() 
    const
{
    return has_sps_palette_predictor_initializer_present_;
}

const PaletteTable&
    SPSScreenContentCodingExtension::GetPalettePredictorInitializer() const
{
    return *palette_predictor_initializer_;
}

uint32_t SPSScreenContentCodingExtension::GetPaletteMaxSize() const
{
    return palette_max_size_;
}

uint32_t SPSScreenContentCodingExtension::GetPredictorPaletteMaxSize() const
{
    return predictor_palette_max_size_;
}

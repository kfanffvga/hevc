#include "hevc_decoder/syntax/sps_screen_content_coding_extension.h"

#include <boost/multi_array.hpp>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

using boost::multi_array;

SPSScreenContentCodingExtension::SPSScreenContentCodingExtension()
{

}

SPSScreenContentCodingExtension::~SPSScreenContentCodingExtension()
{

}

bool SPSScreenContentCodingExtension::Parse(BitStream* bit_stream,
                                            uint32_t chroma_format_idc)
{
    if (!bit_stream)
        return false;

    bool is_sps_curr_pic_ref_enabled = bit_stream->ReadBool();
    bool is_palette_mode_enabled = bit_stream->ReadBool();
    if (is_palette_mode_enabled)
    {
        GolombReader golomb_reader(bit_stream);
        uint32_t palette_max_size = golomb_reader.ReadUnsignedValue();
        uint32_t delta_palette_max_predictor_size = 
            golomb_reader.ReadUnsignedValue();

        bool has_sps_palette_predictor_initializer_present = 
            bit_stream->ReadBool();

        if (has_sps_palette_predictor_initializer_present)
        {
            uint32_t sps_num_palette_predictor_initializer = 
                golomb_reader.ReadUnsignedValue() + 1;

            uint32_t num_of_color_compoments = (0 == chroma_format_idc) ? 1 : 3;
            multi_array<uint32_t, 2> sps_palette_predictor_initializers(
                boost::extents[num_of_color_compoments]
                              [sps_num_palette_predictor_initializer]);

            // 读取的大小纯属猜测,文档中没有提及,FFMPEG中也没有读取
            uint32_t predictor_bit_length = 
                palette_max_size + delta_palette_max_predictor_size;

            for (uint32_t i = 0; i < num_of_color_compoments; ++i)
            {
                for (uint32_t j = 0; j < sps_num_palette_predictor_initializer;
                     ++j)
                {
                    sps_palette_predictor_initializers[i][j] = 
                        bit_stream->Read<uint32_t>(predictor_bit_length);
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

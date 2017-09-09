#include "hevc_decoder/syntax/pps_range_extension.h"

#include <vector>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

using std::vector;

PPSRangeExtension::PPSRangeExtension(bool is_transform_skip_enabled)
    : BaseSyntax()
    , is_transform_skip_enabled_(is_transform_skip_enabled)
{

}

PPSRangeExtension::~PPSRangeExtension()
{

}

bool PPSRangeExtension::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    if (is_transform_skip_enabled_)
    {
        uint32_t log2_max_transform_skip_block_size = 
            golomb_reader.ReadUnsignedValue();
    }
    bool is_cross_component_prediction_enabled = bit_stream->ReadBool();
    bool is_chroma_qp_offset_list_enabled = bit_stream->ReadBool();
    if (is_chroma_qp_offset_list_enabled)
    {
        uint32_t diff_cu_chroma_qp_offset_depth = 
            golomb_reader.ReadUnsignedValue();

        uint32_t chroma_qp_offset_list_len = golomb_reader.ReadUnsignedValue();
        vector<int32_t> cb_qp_offset_list(chroma_qp_offset_list_len);
        vector<int32_t> cr_qp_offset_list(chroma_qp_offset_list_len);
        for (uint32_t i = 0; i < chroma_qp_offset_list_len; ++i)
        {
            cb_qp_offset_list[i] = golomb_reader.ReadSignedValue();
            cr_qp_offset_list[i] = golomb_reader.ReadSignedValue();
        }
        uint32_t sao_offset_scale_luma = golomb_reader.ReadUnsignedValue();
        uint32_t sao_offset_scale_chroma = golomb_reader.ReadUnsignedValue();
    }
    return true;
}

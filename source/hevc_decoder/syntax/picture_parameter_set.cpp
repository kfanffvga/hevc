#include "hevc_decoder/syntax/picture_parameter_set.h"

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"

PictureParameterSet::PictureParameterSet()
{

}

PictureParameterSet::~PictureParameterSet()
{

}

bool PictureParameterSet::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    uint32_t pps_pic_parameter_set_id = golomb_reader.ReadUnsignedValue();
    uint32_t pps_seq_parameter_set_id = golomb_reader.ReadUnsignedValue();
    bool is_dependent_slice_segments_enabled = bit_stream->ReadBool();
    bool has_output_flag_present = bit_stream->ReadBool();
    uint8_t num_extra_slice_header_bits = bit_stream->Read<uint8_t>(3);
    bool is_sign_data_hiding_enabled = bit_stream->ReadBool();
    bool has_cabac_init_present = bit_stream->ReadBool();
    uint32_t num_ref_idx_l0_default_active = golomb_reader.ReadUnsignedValue() + 1;
    uint32_t num_ref_idx_l1_default_active = golomb_reader.ReadUnsignedValue() + 1;
    int init_qp = golomb_reader.ReadSignedValue() + 26;
    bool is_constrained_intra_pred = bit_stream->ReadBool();
    bool is_transform_skip_enabled = bit_stream->ReadBool();
    bool is_cu_qp_delta_enabled = bit_stream->ReadBool();
    if (is_cu_qp_delta_enabled)
        uint32_t diff_cu_qp_delta_depth = golomb_reader.ReadUnsignedValue();

    int pps_cb_qp_offset = golomb_reader.ReadSignedValue();
    int pps_cr_qp_offset = golomb_reader.ReadSignedValue();
    bool has_pps_slice_chroma_qp_offsets_present = bit_stream->ReadBool();
    bool has_weighted_pred = bit_stream->ReadBool();
    bool has_weighted_bipred = bit_stream->ReadBool();
    bool is_transquant_bypass_enabled = bit_stream->ReadBool();
    bool is_tiles_enabled = bit_stream->ReadBool();
    bool is_entropy_coding_sync_enabled = bit_stream->ReadBool();
    if (is_tiles_enabled)
        ParseTileInfo(bit_stream);

    return true;

}

void PictureParameterSet::ParseTileInfo(BitStream* bit_stream)
{

}

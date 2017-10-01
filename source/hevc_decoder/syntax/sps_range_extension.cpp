#include "hevc_decoder/syntax/sps_range_extension.h"

#include "hevc_decoder/base/stream/bit_stream.h"

SPSRangeExtension::SPSRangeExtension()
{

}

SPSRangeExtension::~SPSRangeExtension()
{

}

bool SPSRangeExtension::Parse(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool is_transform_skip_rotation_enabled = bit_stream->ReadBool();
    bool is_transform_skip_context_enabled = bit_stream->ReadBool();
    bool is_implicit_rdpcm_enabled = bit_stream->ReadBool();
    bool is_explicit_rdpcm_enabled = bit_stream->ReadBool();
    bool is_extended_precision_processing = bit_stream->ReadBool();
    bool is_intra_smoothing_disabled = bit_stream->ReadBool();
    bool is_high_precision_offsets_enabled = bit_stream->ReadBool();
    bool is_persistent_rice_adaptation_enabled = bit_stream->ReadBool();
    bool is_cabac_bypass_alignment_enabled = bit_stream->ReadBool();
    return true;
}

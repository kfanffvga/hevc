#include "hevc_decoder/syntax/sps_range_extension.h"

#include "hevc_decoder/base/stream/bit_stream.h"

SPSRangeExtension::SPSRangeExtension()
    : is_high_precision_offsets_enabled_(false)
    , is_explicit_rdpcm_enabled_(false)
    , is_transform_skip_context_enabled_(false)
    , is_implicit_rdpcm_enabled_(false)
    , is_cabac_bypass_alignment_enabled_(false)
    , is_persistent_rice_adaptation_enabled_(false)
    , has_extended_precision_processing_(false)
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
    is_transform_skip_context_enabled_ = bit_stream->ReadBool();
    is_implicit_rdpcm_enabled_ = bit_stream->ReadBool();
    is_explicit_rdpcm_enabled_ = bit_stream->ReadBool();
    has_extended_precision_processing_ = bit_stream->ReadBool();
    bool is_intra_smoothing_disabled = bit_stream->ReadBool();
    is_high_precision_offsets_enabled_ = bit_stream->ReadBool();
    is_persistent_rice_adaptation_enabled_ = bit_stream->ReadBool();
    is_cabac_bypass_alignment_enabled_ = bit_stream->ReadBool();
    return true;
}

bool SPSRangeExtension::IsHighPrecisionOffsetsEnabled() const
{
    return is_high_precision_offsets_enabled_;
}

bool SPSRangeExtension::IsExplicitRDPCMEnabled() const
{
    return is_explicit_rdpcm_enabled_;
}

bool SPSRangeExtension::IsTransformSkipContextEnabled() const
{
    return is_transform_skip_context_enabled_;
}

bool SPSRangeExtension::IsImplicitRDPCMEnabled() const
{
    return is_implicit_rdpcm_enabled_;
}

bool SPSRangeExtension::IsCABACBypassAlignmentEnabled() const
{
    return is_cabac_bypass_alignment_enabled_;
}

bool SPSRangeExtension::IsPersistentRiceAdaptationEnabled() const
{
    return is_persistent_rice_adaptation_enabled_;
}

bool SPSRangeExtension::HasExtendedPrecisionProcessing() const
{
    return has_extended_precision_processing_;
}

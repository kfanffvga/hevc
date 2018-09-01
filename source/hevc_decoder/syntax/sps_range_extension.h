#ifndef _SPS_RANGE_EXTENSION_H_
#define _SPS_RANGE_EXTENSION_H_

class BitStream;

class SPSRangeExtension
{
public:
    SPSRangeExtension();
    virtual ~SPSRangeExtension();

    bool Parse(BitStream* bit_stream);

    bool IsHighPrecisionOffsetsEnabled() const;
    bool IsExplicitRDPCMEnabled() const;
    bool IsTransformSkipContextEnabled() const;
    bool IsImplicitRDPCMEnabled() const;
    bool IsCABACBypassAlignmentEnabled() const;
    bool IsPersistentRiceAdaptationEnabled() const;
    bool HasExtendedPrecisionProcessing() const;

private:
    bool is_high_precision_offsets_enabled_;
    bool is_explicit_rdpcm_enabled_;
    bool is_transform_skip_context_enabled_;
    bool is_implicit_rdpcm_enabled_;
    bool is_cabac_bypass_alignment_enabled_;
    bool is_persistent_rice_adaptation_enabled_;
    bool has_extended_precision_processing_;
};

#endif
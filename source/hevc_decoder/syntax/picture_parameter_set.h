#ifndef _PICTURE_PARAMETER_SET_H_
#define _PICTURE_PARAMETER_SET_H_

#include <stdint.h>
#include <vector>
#include <memory>

#include "hevc_decoder/syntax/base_syntax.h"

class BitStream;
class PPSScreenContentCodingExtension;
class PPSRangeExtension;

class PictureParameterSet : public BaseSyntax
{
public:
    PictureParameterSet();
    virtual ~PictureParameterSet();

    virtual bool Parse(BitStream* bit_stream) override;

    uint32_t GetPictureParameterSetID();
    uint32_t GetAssociateSequenceParameterSetID() const;
    bool IsDependentSliceSegmentEnabled() const;
    uint8_t GetExtraSliceHeaderBitLength() const;
    bool HasOutputFlagPresent() const;
    bool HasListsModificationPresent() const;
    const PPSScreenContentCodingExtension* GetPPSSccExtension() const;
    bool HasCABACInitPresent() const;
    bool HasWeightedPred() const;
    bool HasWeightedBipred() const;
    bool HasPPSSliceChromaQPOffsetPresent() const;
    const PPSRangeExtension* GetPPSRangeExtension() const;
    bool IsDeblockingFilterOverrideEnabled() const;
    bool IsPPSLoopFilterAcrossSliceEnabled() const;
    bool IsDeblockingFilterDisabled() const;
    bool IsTilesEnabled() const;
    bool IsEntropyCodingSyncEnabled() const;

private:
    struct TileInfo
    {
        bool is_uniform_spacing;
        std::vector<uint32_t> column_width;
        std::vector<uint32_t> row_height;
        bool loop_filter_across_tiles_enabled;
    };

    struct DeblockingFilterControlInfo
    {
        bool is_deblocking_filter_override_enabled;
        bool is_pps_deblocking_filter_disabled;
        int32_t beta_offset;
        int32_t tc_offset;
    };

    void ParseTileInfo(BitStream* bit_stream);
    void ParseDeblockingFilterControlInfo(BitStream* bit_stream);
    bool ParsePPSExtensionInfo(bool is_transform_skip_enabled, 
                               BitStream* bit_stream);

    uint32_t pps_pic_parameter_set_id_;
    uint32_t pps_seq_parameter_set_id_;
    bool is_dependent_slice_segments_enabled_;
    uint8_t num_extra_slice_header_bits_;
    bool has_output_flag_present_;
    bool has_lists_modification_present_;
    std::unique_ptr<PPSScreenContentCodingExtension> pps_scc_extension_;
    bool has_cabac_init_present_;
    bool has_weighted_pred_;
    bool has_weighted_bipred_;
    bool has_pps_slice_chroma_qp_offsets_present_;
    std::unique_ptr<PPSRangeExtension> pps_range_extension_;
    DeblockingFilterControlInfo deblocking_filter_control_info_;
    bool is_pps_loop_filter_across_slices_enabled_;
    bool is_tiles_enabled_;
    bool is_entropy_coding_sync_enabled_;

};
#endif
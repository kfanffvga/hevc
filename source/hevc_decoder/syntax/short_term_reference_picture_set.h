#ifndef _SHORT_TERM_REFERENCE_PICTURE_SET_H_
#define _SHORT_TERM_REFERENCE_PICTURE_SET_H_

#include <vector>
#include <stdint.h>

class ShortTermReferencePictureSetContext;
class BitStream;

class ShortTermReferencePictureSet
{
public:
    struct ShortTermReferenceDeltaPictureOrderCountItem
    {
        int delta_poc_of_current_decode_picture;
        bool can_use_for_current_decode_picture;
    };

    typedef std::vector<ShortTermReferenceDeltaPictureOrderCountItem> 
        ReferenceDeltaPOCs;

    ShortTermReferencePictureSet(
        const ShortTermReferencePictureSetContext* context, 
        uint32_t current_rps_index);

    ~ShortTermReferencePictureSet();

    bool Parse(BitStream* bit_stream);

    const ReferenceDeltaPOCs& GetPositiveDeltaPOCs() const;

    const ReferenceDeltaPOCs& GetNegativeDeltaPOCs() const;

private: 
    void ParsePredictionReferencePOCs(BitStream* bit_stream);

    void DeriveDeltaPOCs(const ReferenceDeltaPOCs& ref_positive_delta_pocs,
                         const ReferenceDeltaPOCs& ref_negative_delta_pocs,
                         int delta_rps_of_item, 
                         const std::vector<bool>& can_use_delta, 
                         const std::vector<bool>& is_used_by_curr_pic);

    void DeriveNegativeDeltaPOCs(
        const ReferenceDeltaPOCs& ref_positive_delta_pocs,
        const ReferenceDeltaPOCs& ref_negative_delta_pocs, 
        int delta_rps_of_item, const std::vector<bool>& can_use_delta,
        const std::vector<bool>& is_used_by_curr_pic);

    void DerivePositiveDeltaPOCs(
        const ReferenceDeltaPOCs& ref_positive_delta_pocs,
        const ReferenceDeltaPOCs& ref_negative_delta_pocs,
        int delta_rps_of_item, const std::vector<bool>& can_use_delta,
        const std::vector<bool>& is_used_by_curr_pic);

    const ShortTermReferencePictureSetContext* context_;
    ReferenceDeltaPOCs positive_delta_pocs_;
    ReferenceDeltaPOCs negative_delta_pocs_;
    uint32_t current_rps_index_;
};
#endif
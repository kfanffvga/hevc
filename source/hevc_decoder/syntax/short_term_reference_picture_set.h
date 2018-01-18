#ifndef _SHORT_TERM_REFERENCE_PICTURE_SET_H_
#define _SHORT_TERM_REFERENCE_PICTURE_SET_H_

#include <vector>
#include <stdint.h>
#include <boost/multi_array.hpp>

class IShortTermReferencePictureSetContext;
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

    ShortTermReferencePictureSet(uint32_t current_rps_index);

    ~ShortTermReferencePictureSet();

    bool Parse(BitStream* bit_stream, 
               const IShortTermReferencePictureSetContext* context);

    void operator = (const ShortTermReferencePictureSet& st_ref_pic_set);

    const ReferenceDeltaPOCs& GetPositiveDeltaPOCs() const;

    const ReferenceDeltaPOCs& GetNegativeDeltaPOCs() const;

private: 
    void ParsePredictionReferencePOCs(
        BitStream* bit_stream, 
        const IShortTermReferencePictureSetContext* context);

    void DeriveDeltaPOCs(const ReferenceDeltaPOCs& ref_positive_delta_pocs,
                         const ReferenceDeltaPOCs& ref_negative_delta_pocs,
                         int delta_rps_of_item, 
                         const boost::multi_array<bool, 1>& can_use_delta, 
                         const boost::multi_array<bool, 1>& is_used_by_curr_pic);

    void DeriveNegativeDeltaPOCs(
        const ReferenceDeltaPOCs& ref_positive_delta_pocs,
        const ReferenceDeltaPOCs& ref_negative_delta_pocs, 
        int delta_rps_of_item, const boost::multi_array<bool, 1>& can_use_delta,
        const boost::multi_array<bool, 1>& is_used_by_curr_pic);

    void DerivePositiveDeltaPOCs(
        const ReferenceDeltaPOCs& ref_positive_delta_pocs,
        const ReferenceDeltaPOCs& ref_negative_delta_pocs,
        int delta_rps_of_item, const boost::multi_array<bool, 1>& can_use_delta,
        const boost::multi_array<bool, 1>& is_used_by_curr_pic);

    ReferenceDeltaPOCs positive_delta_pocs_;
    ReferenceDeltaPOCs negative_delta_pocs_;
    uint32_t current_rps_index_;
};
#endif
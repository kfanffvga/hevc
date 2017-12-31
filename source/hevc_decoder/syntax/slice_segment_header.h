#ifndef _SLICE_SEGMENT_HEADER_H_
#define _SLICE_SEGMENT_HEADER_H_

#include <stdint.h>
#include <vector>
#include <memory>

class ParametersManager;
class PictureParameterSet;
class SequenceParameterSet;
class ReferencePictureListsModification;
class ShortTermReferencePictureSet;
class ISliceSegmentHeaderContext;
enum NalUnitType;
enum SliceType;
class BitStream;

class SliceSegmentHeader
{
public:
    SliceSegmentHeader(const ParametersManager* parameters_manager);
    virtual ~SliceSegmentHeader();
    
    bool Parse(BitStream* bit_stream, ISliceSegmentHeaderContext* context);

    SliceType GetSliceType() const;
    const std::vector<int32_t>& GetNegativeRefPOCList() const;
    const std::vector<int32_t>& GetPositiveRefPOCList() const;

private:
    struct LongTermReferencePictureOrderCountInfo
    {
        uint32_t picture_order_count_value; // 当前cvs里的长期参考帧的poc的值
        bool is_used_by_curr_pic_lt;
    };

    typedef std::vector<LongTermReferencePictureOrderCountInfo> 
        LongTermRefPOCInfoSet;

    bool ParseIndependentSyntax(const PictureParameterSet* pps, 
                                const SequenceParameterSet* sps,
                                BitStream* bit_stream,
                                ISliceSegmentHeaderContext* context);

    bool ParseReferencePictureSet(
        const PictureParameterSet* pps, const SequenceParameterSet* sps, 
        BitStream* bit_stream, ISliceSegmentHeaderContext* context,
        LongTermRefPOCInfoSet* lt_ref_poc_infos);

    bool ParseReferenceDetailInfo(
        const PictureParameterSet* pps, const SequenceParameterSet* sps,
        bool is_slice_temporal_mvp_enabled, 
        const LongTermRefPOCInfoSet& current_lt_ref_poc_infos,
        BitStream* bit_stream, ISliceSegmentHeaderContext* context);

    bool ParseQuantizationParameterInfo(
        const PictureParameterSet* pps, const SequenceParameterSet* sps,
        BitStream* bit_stream);

    bool ParseReconstructPictureInfo(const PictureParameterSet* pps,
                                     bool is_slice_sao_luma, 
                                     bool is_slice_sao_chroma,
                                     BitStream* bit_stream);

    bool ParseTileInfo(const PictureParameterSet* pps, BitStream* bit_stream);

    uint32_t GetCurrentAvailableReferencePictureCount(
        const PictureParameterSet* pps, const SequenceParameterSet* sps,
        const LongTermRefPOCInfoSet& current_lt_ref_poc_infos);

    bool ConstructReferencePOCList(
        const SequenceParameterSet* sps, ISliceSegmentHeaderContext* context,
        int short_term_ref_pic_set_idx, bool is_current_picture_ref_enabled,
        const LongTermRefPOCInfoSet& current_lt_ref_poc_infos,
        const ReferencePictureListsModification* ref_pic_list_modification,
        std::vector<int32_t>* negative_ref_pic_list,
        std::vector<int32_t>* positive_ref_pic_list);

    const ParametersManager* parameters_manager_;

    std::unique_ptr<ShortTermReferencePictureSet> st_ref_pic_set_of_self_;     
    int short_term_ref_pic_set_idx_; // fix me: 不需要的成员变量

    std::vector<int32_t> negative_ref_poc_list_;
    std::vector<int32_t> positive_ref_poc_list_;
    SliceType slice_type_;
};

#endif
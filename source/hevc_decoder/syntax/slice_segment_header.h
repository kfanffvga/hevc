#ifndef _SLICE_SEGMENT_HEADER_H_
#define _SLICE_SEGMENT_HEADER_H_

#include <stdint.h>
#include <vector>
#include <memory>

#include "hevc_decoder/base/tile_info.h"

class ParametersManager;
class PictureParameterSet;
class SequenceParameterSet;
class ReferencePictureListsModification;
class ShortTermReferencePictureSet;
class ISliceSegmentHeaderContext;
enum NalUnitType;
enum SliceType;
class BitStream;
class TileInfo;

// 由于sps与pps在此处是parse的时候得到，如得不到，整个解码过程就结束了，
// 并且parse函数均是在一开始时候调用，因此，在以后函数的使用的时候，
// 为了代码的速度与阅读的流畅性，不进行相关的sps与pps的指针的判断，
// 如调用者在调用parse之前就去调用相关的有访问到sps与pps的函数产生的问题，
// 由调用者进行保证

class SliceSegmentHeader
{
public:
    SliceSegmentHeader(const ParametersManager* parameters_manager);
    virtual ~SliceSegmentHeader();
    
    bool Parse(BitStream* bit_stream, ISliceSegmentHeaderContext* context);

    bool IsFirstSliceSegmentInPic() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    const TileInfo& GetTileInfo() const;
    uint32_t GetCTBLog2SizeY() const;
    uint32_t GetMinTBLog2SizeY() const;
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

    bool ParseIndependentSyntax(BitStream* bit_stream,
                                ISliceSegmentHeaderContext* context);

    bool ParseReferencePictureSet(
        BitStream* bit_stream, ISliceSegmentHeaderContext* context,
        LongTermRefPOCInfoSet* long_term_ref_poc_infos, 
        ShortTermReferencePictureSet* short_term_ref_poc_infos);

    bool ParseReferenceDetailInfo(
        bool is_slice_temporal_mvp_enabled, 
        const ShortTermReferencePictureSet& short_term_ref_poc_infos,
        const LongTermRefPOCInfoSet& long_term_ref_poc_infos,
        BitStream* bit_stream, ISliceSegmentHeaderContext* context);

    bool ParseQuantizationParameterInfo(BitStream* bit_stream);

    bool ParseReconstructPictureInfo(bool is_slice_sao_luma, 
                                     bool is_slice_sao_chroma,
                                     BitStream* bit_stream);

    bool ParseTileInfo(BitStream* bit_stream);

    uint32_t GetCurrentAvailableReferencePictureCount(
        const ShortTermReferencePictureSet& short_term_ref_poc_infos,
        const LongTermRefPOCInfoSet& long_term_ref_poc_infos);

    bool ConstructReferencePOCList(
        ISliceSegmentHeaderContext* context, bool is_current_picture_ref_enabled,
        const ShortTermReferencePictureSet& short_term_ref_poc_infos,
        const LongTermRefPOCInfoSet& long_term_ref_poc_infos,
        const ReferencePictureListsModification& ref_pic_list_modification,
        std::vector<int32_t>* negative_ref_pic_list,
        std::vector<int32_t>* positive_ref_pic_list);

    const ParametersManager* parameters_manager_;
    std::shared_ptr<PictureParameterSet> pps_;
    std::shared_ptr<SequenceParameterSet> sps_;

    bool is_first_slice_segment_in_pic_;
    std::vector<int32_t> negative_ref_poc_list_;
    std::vector<int32_t> positive_ref_poc_list_;
    SliceType slice_type_;
};

#endif
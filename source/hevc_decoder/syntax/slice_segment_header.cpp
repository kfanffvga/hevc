#include "hevc_decoder/syntax/slice_segment_header.h"

#include <cassert>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/base/math.h"
#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/nal_unit_types.h"
#include "hevc_decoder/syntax/picture_parameter_set.h"
#include "hevc_decoder/syntax/frame_syntax.h"
#include "hevc_decoder/syntax/parameters_manager.h"
#include "hevc_decoder/syntax/prediction_weight_table.h"
#include "hevc_decoder/syntax/sequence_parameter_set.h"
#include "hevc_decoder/syntax/short_term_reference_picture_set.h"
#include "hevc_decoder/syntax/pps_screen_content_coding_extension.h"
#include "hevc_decoder/syntax/reference_picture_lists_modification.h"
#include "hevc_decoder/syntax/sps_screen_content_coding_extension.h"
#include "hevc_decoder/syntax/sps_range_extension.h"
#include "hevc_decoder/syntax/pps_range_extension.h"
#include "hevc_decoder/syntax/slice_segment_header_context.h"
#include "hevc_decoder/syntax/short_term_reference_picture_set_context_impl.h"
#include "hevc_decoder/syntax/prediction_weight_table_context.h"

using std::vector;
using std::unique_ptr;

class PredictionWeightTableContext : public IPredictionWeightTableContext
{
public:
    PredictionWeightTableContext(
        const SliceSegmentHeader* slice_segemnt_header, 
        const SequenceParameterSet* sps,
        const ISliceSegmentHeaderContext* slice_segment_header_context)
        : slice_segment_header_(slice_segemnt_header)
        , slice_segment_header_context_(slice_segment_header_context)
        , sps_(sps)
    {
        assert(slice_segemnt_header);
        assert(slice_segment_header_context);
        assert(sps);
    }

    virtual~PredictionWeightTableContext()
    {

    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return sps_->GetChromaFormatType();
    }

    virtual PictureOrderCount GetCurrentPictureOrderCount() const override
    {
        return slice_segment_header_context_->GetPictureOrderCount();
    }

    virtual uint8_t GetCurrentNuhLayerID() const override
    {
        return slice_segment_header_context_->GetNuhLayerID();
    }

    virtual uint8_t GetNuhLayerIDByPOCValue(uint32_t poc_value) const override
    {
        return slice_segment_header_context_->GetNuhLayerIDByPOCValue(poc_value);
    }

    virtual SliceType GetSliceType() const override
    {
        return slice_segment_header_->GetSliceType();
    }

    virtual const std::vector<int32_t>& GetNegativeRefPOCList() const override
    {
        return slice_segment_header_->GetNegativeRefPOCList();
    }

    virtual const std::vector<int32_t>& GetPositiveRefPOCList() const override
    {
        return slice_segment_header_->GetPositiveRefPOCList();
    }

    virtual uint32_t GetWPOffsetHalfRange() const override
    {
        return sps_->GetSPSRangeExtension().IsHighPrecisionOffsetsEnabled() ?
            (1 << (sps_->GetBitDepthChroma() - 1)) : (1 << 7);
    }

private:
    const SliceSegmentHeader* slice_segment_header_;
    const ISliceSegmentHeaderContext* slice_segment_header_context_;
    const SequenceParameterSet* sps_;
};

SliceSegmentHeader::SliceSegmentHeader(
    const ParametersManager* parameters_manager)
    : parameters_manager_(parameters_manager)
    , st_ref_pic_set_of_self_()
    , short_term_ref_pic_set_idx_(-1)
    , negative_ref_poc_list_()
    , positive_ref_poc_list_()
    , slice_type_(I_SLICE)
{

}

SliceSegmentHeader::~SliceSegmentHeader()
{

}

bool SliceSegmentHeader::Parse(BitStream* bit_stream, 
                               ISliceSegmentHeaderContext* context)
{
    if (!bit_stream || !context)
        return false;

    bool is_first_slice_segment_in_pic = bit_stream->ReadBool();
    if ((context->GetNalUnitType() >= BLA_W_LP) && 
        (context->GetNalUnitType() <= RSV_IRAP_VCL23))
        bool is_no_output_of_prior_pics = bit_stream->ReadBool();

    GolombReader golomb_reader(bit_stream);
    uint32_t slice_pic_parameter_set_id = golomb_reader.ReadUnsignedValue();
    const PictureParameterSet* pps = 
        parameters_manager_->GetPictureParameterSet(slice_pic_parameter_set_id);
    if (!pps)
        return false;

    const SequenceParameterSet* sps = 
        parameters_manager_->GetSequenceParameterSet(
            pps->GetAssociateSequenceParameterSetID());
    if (!sps)
        return false;

    bool is_dependent_slice_segment = false;
    if (!is_first_slice_segment_in_pic)
    {
        if (pps->IsDependentSliceSegmentEnabled())
            is_dependent_slice_segment = bit_stream->ReadBool();

        uint32_t slice_segment_address = 
            bit_stream->Read<uint32_t>(sps->GetSliceSegmentAddressBitLength());
    }
    if (!is_dependent_slice_segment)
    {
        bool success = ParseIndependentSyntax(pps, sps, bit_stream, context);
        if (!success)
            return false;
    }

    return ParseTileInfo(pps, bit_stream);
}

SliceType SliceSegmentHeader::GetSliceType() const
{
    return slice_type_;
}

const vector<int32_t>& SliceSegmentHeader::GetNegativeRefPOCList() const
{
    return negative_ref_poc_list_;
}

const vector<int32_t>& SliceSegmentHeader::GetPositiveRefPOCList() const
{
    return positive_ref_poc_list_;
}

bool SliceSegmentHeader::ParseIndependentSyntax(
    const PictureParameterSet* pps, const SequenceParameterSet* sps,
    BitStream* bit_stream, ISliceSegmentHeaderContext* context)
{
    assert(bit_stream);

    bit_stream->SkipBits(pps->GetExtraSliceHeaderBitLength());

    GolombReader golomb_reader(bit_stream);
    slice_type_ = static_cast<SliceType>(golomb_reader.ReadUnsignedValue());

    if (pps->HasOutputFlagPresent())
        bool is_pic_output = bit_stream->ReadBool();

    // 如果是单一色彩的话,则需要知道究竟是何种颜色
    if (sps->GetChromaFormatType() == YUV_MONO_CHROME)
        uint8_t colour_plane_id = bit_stream->Read<uint8_t>(2);

    bool is_slice_temporal_mvp_enabled = false;
    LongTermRefPOCInfoSet lt_ref_poc_infos;
    if ((context->GetNalUnitType() != IDR_N_LP) && 
        (context->GetNalUnitType() != IDR_W_RADL))
    {
        uint8_t slice_pic_order_cnt_lsb =
            bit_stream->Read<uint8_t>(sps->GetPicOrderCountLSBBitLength());
        context->SetPictureOrderCountByLSB(slice_pic_order_cnt_lsb, 
                                           sps->GetMaxLSBOfPicOrderCount());

        bool success = ParseReferencePictureSet(pps, sps, bit_stream, context,
                                                &lt_ref_poc_infos);
        if (!success)
            return false;

        if (sps->IsTemporalMVPEnabled())
            is_slice_temporal_mvp_enabled = bit_stream->ReadBool();
    }

    bool is_slice_sao_luma = false;
    bool is_slice_sao_chroma = false;
    if (sps->IsSampleAdaptiveOffsetEnabled())
    {
        is_slice_sao_luma = bit_stream->ReadBool();
        if ((sps->GetChromaFormatType() != MONO_CHROME) && 
            (sps->GetChromaFormatType() != YUV_MONO_CHROME))
            is_slice_sao_chroma = bit_stream->ReadBool();
    }
    if (slice_type_ != I_SLICE)
    {
        bool success = ParseReferenceDetailInfo(pps, sps, 
                                                is_slice_temporal_mvp_enabled, 
                                                lt_ref_poc_infos,
                                                bit_stream, context);
        if (!success)
            return false;
    }
    bool success = ParseQuantizationParameterInfo(pps, sps, bit_stream);
    if (!success)
        return false;

    success = ParseReconstructPictureInfo(pps, is_slice_sao_luma, 
                                          is_slice_sao_chroma, bit_stream);
    return success;
}

bool SliceSegmentHeader::ParseReferencePictureSet(
    const PictureParameterSet* pps, const SequenceParameterSet* sps, 
    BitStream* bit_stream, ISliceSegmentHeaderContext* context,
    LongTermRefPOCInfoSet* lt_ref_poc_infos)
{
    assert(lt_ref_poc_infos);
    if (!lt_ref_poc_infos)
        return false;

    bool has_short_term_ref_pic_set_sps = bit_stream->ReadBool();
    uint32_t short_term_rps_count =sps->GetShortTermReferencePictureSetCount();
    if (!has_short_term_ref_pic_set_sps)
    {
        st_ref_pic_set_of_self_.reset(
            new ShortTermReferencePictureSet(short_term_rps_count));
        ShortTermReferencePictureSetContext short_term_rps_context(sps);
        bool success = 
            st_ref_pic_set_of_self_->Parse(bit_stream, &short_term_rps_context);
        if (!success)
            return false;
    }
    else 
    {
        // -1代表使用当前slice_segment_header指定的参考poc
        // 假如short_term_reference_picture_set_count == 1 也就是说当前的st_ref没有
        // 选择,因此,不需要指定short_term_ref_pic_set_idx_, 因为默认值为0
        short_term_ref_pic_set_idx_ = 0;
        if (short_term_rps_count > 1)
        {
            // TODO: 能够把此处的log2放到context里做
            uint32_t bits = CeilLog2(short_term_rps_count);
            short_term_ref_pic_set_idx_ = bit_stream->Read<int>(bits);
        }
    }

    if (sps->HasLongTermReferencePicturesPresent())
    {
        const vector<LongTermReferenceLSBPictureOrderCountInfo>& 
            lt_ref_lsb_poc_infos = 
                sps->GetLongTermReferencePictureOrderCountInfos();

        uint32_t num_long_term_sps = 0;
        GolombReader golomb_reader(bit_stream);
        if (lt_ref_lsb_poc_infos.size() > 0)
            num_long_term_sps = golomb_reader.ReadUnsignedValue();

        uint32_t num_long_term_pics = golomb_reader.ReadUnsignedValue();
        // TODO: 可以只算一次,而不是每次读取slice_segment时候都要算
        uint32_t lt_idx_sps_bit_length = CeilLog2(lt_ref_lsb_poc_infos.size());
        
        PictureOrderCount current_poc = context->GetPictureOrderCount();
        auto get_lt_poc_value = 
            [current_poc, sps](uint32_t delta_poc_msb_cycle_lt, uint32_t lsb)
        {
            uint32_t lt_msb = 
                delta_poc_msb_cycle_lt * sps->GetMaxLSBOfPicOrderCount();
            // 当前图片的msb的开始加上指定的lsb
            return current_poc.msb - lt_msb + lsb;
        };
        // lt_ref_lsb_poc_infos.size() == 0 则num_long_term_sps 也为0
        uint32_t preview_delta_poc_msb_cycle_lt = 0;
        for (uint32_t i = 0; i < num_long_term_sps; ++i)
        {
            uint32_t lt_idx_sps = 0;
            if (lt_ref_lsb_poc_infos.size() > 1)
            {
                lt_idx_sps = bit_stream->Read<uint32_t>(lt_idx_sps_bit_length);
                assert(lt_idx_sps < lt_ref_lsb_poc_infos.size());
            }

            bool has_delta_poc_msb_present = bit_stream->ReadBool();
            uint32_t delta_poc_msb_cycle_lt = 0;
            if (has_delta_poc_msb_present)
                delta_poc_msb_cycle_lt = golomb_reader.ReadUnsignedValue();

            preview_delta_poc_msb_cycle_lt +=  delta_poc_msb_cycle_lt;

            LongTermReferencePictureOrderCountInfo info = { };
            info.picture_order_count_value = get_lt_poc_value(
                preview_delta_poc_msb_cycle_lt,
                lt_ref_lsb_poc_infos[lt_idx_sps].lt_ref_pic_poc_lsb_sps);

            info.is_used_by_curr_pic_lt = 
                lt_ref_lsb_poc_infos[lt_idx_sps].is_used_by_curr_pic_lt_sps_flag;
            lt_ref_poc_infos->push_back(info);
        }

        preview_delta_poc_msb_cycle_lt = 0;
        for (uint32_t i = 0; i < num_long_term_pics; ++i)
        {
            uint32_t poc_lsb_lt = 
                bit_stream->Read<uint32_t>(sps->GetPicOrderCountLSBBitLength());
            bool is_used_by_curr_pic_lt = bit_stream->ReadBool();
            bool has_delta_poc_msb_present = bit_stream->ReadBool();
            uint32_t delta_poc_msb_cycle_lt = 0;
            if (has_delta_poc_msb_present)
                delta_poc_msb_cycle_lt = golomb_reader.ReadUnsignedValue();

            preview_delta_poc_msb_cycle_lt += delta_poc_msb_cycle_lt;
            LongTermReferencePictureOrderCountInfo info = { };
            info.picture_order_count_value = get_lt_poc_value(
                preview_delta_poc_msb_cycle_lt, poc_lsb_lt);
            info.is_used_by_curr_pic_lt = is_used_by_curr_pic_lt;
            lt_ref_poc_infos->push_back(info);
        }
    }
    return true;
}

bool SliceSegmentHeader::ParseReferenceDetailInfo(
    const PictureParameterSet* pps, const SequenceParameterSet* sps, 
    bool is_slice_temporal_mvp_enabled, 
    const LongTermRefPOCInfoSet& current_lt_ref_poc_infos,
    BitStream* bit_stream, ISliceSegmentHeaderContext* context)
{
    bool is_num_ref_idx_active_override = bit_stream->ReadBool();
    GolombReader golomb_reader(bit_stream);
    uint32_t num_ref_idx_negative_active = 0;
    uint32_t num_ref_idx_positive_active = 0;
    if (is_num_ref_idx_active_override)
    {
        num_ref_idx_negative_active = golomb_reader.ReadUnsignedValue();
        if (B_SLICE == slice_type_)
            num_ref_idx_positive_active = golomb_reader.ReadUnsignedValue();
    }
    uint32_t reference_picture_count = GetCurrentAvailableReferencePictureCount(
        pps, sps, current_lt_ref_poc_infos);

    ReferencePictureListsModification ref_pic_list_modification;
    if (pps->HasListsModificationPresent() && (reference_picture_count > 1))
    {
        bool success = ref_pic_list_modification.Parse(
            bit_stream, slice_type_, num_ref_idx_negative_active,
            num_ref_idx_positive_active, CeilLog2(reference_picture_count));
        if (!success)
            return false;
    }
    const PPSScreenContentCodingExtension* pps_scc_extension =
        pps->GetPPSSccExtension();
    bool is_current_picture_ref_enabled = pps_scc_extension ? 
        pps_scc_extension->IsPPSCurrentPictureReferenceEnabled() : false;

    bool success = ConstructReferencePOCList(sps, context,
                                             short_term_ref_pic_set_idx_,
                                             is_current_picture_ref_enabled,
                                             current_lt_ref_poc_infos,
                                             ref_pic_list_modification,
                                             &negative_ref_poc_list_, 
                                             &positive_ref_poc_list_);
    if (!success)
        return false;

    if (B_SLICE == slice_type_)
        bool is_mvd_positive_zero = bit_stream->ReadBool();

    if (pps->HasCABACInitPresent())
        bool is_need_cabac_init = bit_stream->ReadBool();

    if (is_slice_temporal_mvp_enabled)
    {
        bool is_collocated_from_negative = true;
        if (B_SLICE == slice_type_)
            is_collocated_from_negative = bit_stream->ReadBool();

        if ((is_collocated_from_negative && (num_ref_idx_negative_active > 0)) ||
            (!is_collocated_from_negative && (num_ref_idx_positive_active > 0)))
            uint32_t collocated_ref_idx = golomb_reader.ReadUnsignedValue();
    }
    if ((pps->HasWeightedPred() && (P_SLICE == slice_type_)) ||
        (pps->HasWeightedBipred() && (B_SLICE == slice_type_)))
    {
        PredictionWeightTable prediction_weight_table;
        PredictionWeightTableContext prediction_weight_table_context(this, sps, 
                                                                     context);
        success = prediction_weight_table.Parse(
            bit_stream, &prediction_weight_table_context);
        if (!success)
            return false;
    }
    int32_t max_num_merge_cand = 5 - golomb_reader.ReadUnsignedValue();
    const SPSScreenContentCodingExtension& sps_scc_extension =
        sps->GetSPSScreenContentCodingExtension();
    if (sps_scc_extension.GetMotionVectorResolutionControlIDC() == 2)
        bool is_use_integer_mv = bit_stream->ReadBool();

    return true;
}

uint32_t SliceSegmentHeader::GetCurrentAvailableReferencePictureCount(
    const PictureParameterSet* pps, const SequenceParameterSet* sps,
    const LongTermRefPOCInfoSet& current_lt_ref_poc_infos)
{
    assert(sps && pps);
    uint32_t reference_picture_count = 0;
    auto get_short_term_reference_picture_count = 
        [](const ShortTermReferencePictureSet* st_ref_pic_set) -> uint32_t
    {
        uint32_t count = 0;
        for (const auto& i : st_ref_pic_set->GetNegativeDeltaPOCs())
        {
            if (i.can_use_for_current_decode_picture)
                ++count;
        }   

        for (const auto& i : st_ref_pic_set->GetPositiveDeltaPOCs())
        {
            if (i.can_use_for_current_decode_picture)
                ++count;
        }
        return count;
    };
    if (st_ref_pic_set_of_self_)
    {
        reference_picture_count = get_short_term_reference_picture_count(
            st_ref_pic_set_of_self_.get());
    }
    else if (short_term_ref_pic_set_idx_ >= 0)
    {
        const ShortTermReferencePictureSet* st_ref_pic_set = 
            sps->GetShortTermReferencePictureSet(short_term_ref_pic_set_idx_);
        if (st_ref_pic_set)
        {
            reference_picture_count = 
                get_short_term_reference_picture_count(st_ref_pic_set);
        }
    }
    for (const auto& i : current_lt_ref_poc_infos)
    {
        if (i.is_used_by_curr_pic_lt)
            ++reference_picture_count;
    }
    const PPSScreenContentCodingExtension* pps_scc_extension = 
        pps->GetPPSSccExtension();
    if (!pps_scc_extension)
        return reference_picture_count;

    return pps_scc_extension->IsPPSCurrentPictureReferenceEnabled() ?
        reference_picture_count + 1 : reference_picture_count;
}

bool SliceSegmentHeader::ConstructReferencePOCList(
    const SequenceParameterSet* sps, ISliceSegmentHeaderContext* context, 
    int short_term_ref_pic_set_idx, bool is_current_picture_ref_enabled,
    const LongTermRefPOCInfoSet& current_lt_ref_poc_infos,
    const ReferencePictureListsModification& ref_pic_list_modification,
    vector<int32_t>* negative_ref_pic_list,
    vector<int32_t>* positive_ref_pic_list)
{
    const ShortTermReferencePictureSet* st_ref_pic_set = nullptr;
    if (short_term_ref_pic_set_idx < 0)
    {
        st_ref_pic_set = st_ref_pic_set_of_self_.get();
    }
    else
    {
        st_ref_pic_set = 
            sps->GetShortTermReferencePictureSet(short_term_ref_pic_set_idx);
    }
    assert(st_ref_pic_set);
    if (!st_ref_pic_set)
        return false;

    uint32_t current_poc_value = context->GetPictureOrderCount().value;

    // 暂时性还不清楚会不会产生负数,但由于poc都是正数,按照文档定义一致性的原则,认为这里也是
    // 正数
    vector<uint32_t> poc_st_curr_before;
    vector<uint32_t> poc_st_foll;
    for (const auto& i : st_ref_pic_set->GetNegativeDeltaPOCs())
    {
        // 相对位置转绝对位置
        uint32_t ref_poc_value = static_cast<uint32_t>(
            i.delta_poc_of_current_decode_picture + current_poc_value);

        if (i.can_use_for_current_decode_picture)
            poc_st_curr_before.push_back(ref_poc_value);
        else
            poc_st_foll.push_back(ref_poc_value);
    }
    vector<uint32_t> poc_st_curr_after;
    for (const auto& i : st_ref_pic_set->GetPositiveDeltaPOCs())
    {
        uint32_t ref_poc_value = static_cast<uint32_t>(
            i.delta_poc_of_current_decode_picture + current_poc_value);

        if (i.can_use_for_current_decode_picture)
            poc_st_curr_after.push_back(ref_poc_value);
        else
            poc_st_foll.push_back(ref_poc_value);
    }
    vector<uint32_t> poc_lt_curr;
    vector<uint32_t> poc_lt_foll;
    for (const auto& i : current_lt_ref_poc_infos)
    {
        if (i.is_used_by_curr_pic_lt)
            poc_lt_curr.push_back(i.picture_order_count_value);
        else
            poc_lt_foll.push_back(i.picture_order_count_value);
    }

    //fix me: 假如以后证实是正整数,则修改此处
    auto general_reference_list = [](
        const vector<uint32_t>& st_ref1, const vector<uint32_t>& st_ref2,
        const vector<uint32_t>& lt_ref, bool is_allow_self_ref, 
        uint32_t self_poc, const vector<uint32_t>& ref_pic_list_resort_indices,
        vector<int32_t>* ref_list)
    {
        assert(ref_list);
        vector<int32_t> merge_cache_ref_list;
        for (const auto& i : st_ref1)
            merge_cache_ref_list.push_back(static_cast<int32_t>(i));

        for (const auto& i : st_ref2)
            merge_cache_ref_list.push_back(static_cast<int32_t>(i));

        for (const auto& i : lt_ref)
            merge_cache_ref_list.push_back(static_cast<int32_t>(i));

        if (is_allow_self_ref)
            merge_cache_ref_list.push_back(static_cast<int32_t>(self_poc));

        if (!ref_pic_list_resort_indices.empty())
        {
            for (const auto& i : ref_pic_list_resort_indices)
                ref_list->push_back(merge_cache_ref_list[i]);
        }
        else
        {
            *ref_list = merge_cache_ref_list;
        }
    };
    
    general_reference_list(poc_st_curr_before, poc_st_curr_after, poc_lt_curr, 
                           is_current_picture_ref_enabled, current_poc_value,
                           ref_pic_list_modification.GetListEntryOfNegative(),
                           negative_ref_pic_list);
    if (B_SLICE == slice_type_)
    {
        general_reference_list(
            poc_st_curr_after, poc_st_curr_before, poc_lt_curr, 
            is_current_picture_ref_enabled, current_poc_value,
            ref_pic_list_modification.GetListEntryOfPositive(),
            positive_ref_pic_list);
    }
    return true;
}

bool SliceSegmentHeader::ParseQuantizationParameterInfo(
    const PictureParameterSet* pps, const SequenceParameterSet* sps,
    BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    int32_t slice_qp_delta = golomb_reader.ReadSignedValue();
    if (pps->HasPPSSliceChromaQPOffsetPresent())
    {
        int32_t slice_cb_qp_offset = golomb_reader.ReadSignedValue();
        int32_t slice_cr_qp_offset = golomb_reader.ReadSignedValue();
    }
    const PPSScreenContentCodingExtension* pps_scc_extension = 
        pps->GetPPSSccExtension();
    if (pps_scc_extension && pps_scc_extension->HasPPSSliceActQPOffsetsPresent())
    {
        int32_t slice_act_y_qp_offset = golomb_reader.ReadSignedValue();
        int32_t slice_act_cb_qp_offset = golomb_reader.ReadSignedValue();
        int32_t slice_act_cr_qp_offset = golomb_reader.ReadSignedValue();
    }
    const PPSRangeExtension* pps_range_extension = pps->GetPPSRangeExtension();
    if (pps_range_extension && 
        pps_range_extension->IsChromaQPOffsetListEnabled())
        bool cu_chroma_qp_offset_enabled = bit_stream->ReadBool();

    return true;
}

bool SliceSegmentHeader::ParseReconstructPictureInfo(
    const PictureParameterSet* pps, bool is_slice_sao_luma, 
    bool is_slice_sao_chroma, BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool is_deblocking_filter_override = false;
    if (pps->IsDeblockingFilterOverrideEnabled())
        is_deblocking_filter_override = bit_stream->ReadBool();

    bool is_slice_deblocking_filter_disabled = pps->IsDeblockingFilterDisabled();
    if (is_deblocking_filter_override)
    {
        is_slice_deblocking_filter_disabled = bit_stream->ReadBool();
        if (!is_slice_deblocking_filter_disabled)
        {
            GolombReader golomb_reader(bit_stream);
            int32_t slice_beta_offset = golomb_reader.ReadSignedValue() * 2;
            int32_t slice_tc_offset = golomb_reader.ReadSignedValue() * 2;
        }
    }
    if (pps->IsPPSLoopFilterAcrossSliceEnabled() && 
        (is_slice_sao_luma || is_slice_sao_chroma || 
        !is_slice_deblocking_filter_disabled))
        bool is_slice_loop_filter_across_slice_enabled = bit_stream->ReadBool();

    return true;
}

bool SliceSegmentHeader::ParseTileInfo(const PictureParameterSet* pps, 
                                       BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    if (pps->IsTilesEnabled() || pps->IsEntropyCodingSyncEnabled())
    {
        GolombReader golomb_reader(bit_stream);
        uint32_t num_entry_point_offsets = golomb_reader.ReadUnsignedValue();
        if (num_entry_point_offsets > 0)
        {
            uint32_t offset_len = golomb_reader.ReadUnsignedValue() + 1;
            vector<uint32_t> entry_point_offset;
            for (uint32_t i = 0; i < num_entry_point_offsets; ++i)
            {
                uint32_t v = bit_stream->Read<uint32_t>(offset_len) + 1;
                entry_point_offset.push_back(v);
            }   
        }
    }
    return true;
}

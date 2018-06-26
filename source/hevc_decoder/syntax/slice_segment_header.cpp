#include "hevc_decoder/syntax/slice_segment_header.h"

#include <cassert>

#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/stream/golomb_reader.h"
#include "hevc_decoder/base/math.h"
#include "hevc_decoder/base/tile_info.h"
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
#include "hevc_decoder/syntax/palette_table.h"

using std::vector;
using std::shared_ptr;
using std::unique_ptr;

class PredictionWeightTableContext : public IPredictionWeightTableContext
{
public:
    PredictionWeightTableContext(
        const SliceSegmentHeader* slice_segemnt_header, 
        const shared_ptr<SequenceParameterSet> sps,
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
    const shared_ptr<SequenceParameterSet> sps_;
};

SliceSegmentHeader::SliceSegmentHeader()
    : pps_()
    , sps_()
    , is_first_slice_segment_in_pic_(false)
    , negative_ref_poc_list_()
    , positive_ref_poc_list_()
    , slice_type_(I_SLICE)
    , slice_segment_address_(0)
    , is_dependent_slice_segment_(false)
    , quantization_parameter_(0)
    , is_slice_sao_luma_(false)
    , is_slice_sao_chroma_(false)
    , is_used_cabac_init_(false)
    , is_cu_chroma_qp_offset_enabled_(false)
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

    is_first_slice_segment_in_pic_ = bit_stream->ReadBool();
    if ((context->GetNalUnitType() >= BLA_W_LP) && 
        (context->GetNalUnitType() <= RSV_IRAP_VCL23))
        bool is_no_output_of_prior_pics = bit_stream->ReadBool();

    GolombReader golomb_reader(bit_stream);
    uint32_t slice_pic_parameter_set_id = golomb_reader.ReadUnsignedValue();
    const ParametersManager& pm = context->GetParametersManager();
    pps_ = pm.GetPictureParameterSet(slice_pic_parameter_set_id);
    if (!pps_)
        return false;

    sps_ = pm.GetSequenceParameterSet(pps_->GetAssociateSequenceParameterSetID());
    if (!sps_)
        return false;

    if (!is_first_slice_segment_in_pic_)
    {
        if (pps_->IsDependentSliceSegmentEnabled())
            is_dependent_slice_segment_ = bit_stream->ReadBool();

        slice_segment_address_ = 
            bit_stream->Read<uint32_t>(sps_->GetSliceSegmentAddressBitLength());
    }
    if (!is_dependent_slice_segment_)
    {
        bool success = ParseIndependentSyntax(bit_stream, context);
        if (!success)
            return false;
    }

    return ParseTileInfo(bit_stream);
}

bool SliceSegmentHeader::IsFirstSliceSegmentInPic() const
{
    return is_first_slice_segment_in_pic_;
}

uint32_t SliceSegmentHeader::GetWidth() const
{
    return sps_->GetPicWidthInLumaSamples();
}

uint32_t SliceSegmentHeader::GetHeight() const
{
    return sps_->GetPicHeightInLumaSamples();
}

const TileInfo& SliceSegmentHeader::GetTileInfo() const
{
    return pps_->GetTileInfo();
}

uint32_t SliceSegmentHeader::GetCTBLog2SizeY() const
{
    return sps_->GetCTBLog2SizeY();
}

uint32_t SliceSegmentHeader::GetCTBHeight() const
{
    return sps_->GetCTBHeight();
}

uint32_t SliceSegmentHeader::GetMinTBLog2SizeY() const
{
    return sps_->GetLog2MinLumaTransformBlockSize();
}

uint32_t SliceSegmentHeader::GetMaxTransformBlockSizeY() const
{
    return sps_->GetMaxTransformBlockSizeY();
}

uint32_t SliceSegmentHeader::GetMinTransformBlockSizeY() const
{
    return sps_->GetMinTransformBlockSizeY();
}

SliceType SliceSegmentHeader::GetSliceType() const
{
    return slice_type_;
}

uint32_t SliceSegmentHeader::GetSliceSegmentAddress() const
{
    return slice_segment_address_;
}

bool SliceSegmentHeader::IsDependentSliceSegment() const
{
    return is_dependent_slice_segment_;
}

uint32_t SliceSegmentHeader::GetQuantizationParameter() const
{
    return quantization_parameter_;
}

bool SliceSegmentHeader::IsEntropyCodingSyncEnabled() const
{
    return pps_->IsEntropyCodingSyncEnabled();
}

bool SliceSegmentHeader::IsTilesEnabled() const
{
    return pps_->IsTilesEnabled();
}

bool SliceSegmentHeader::IsSliceSAOLuma() const
{
    return is_slice_sao_luma_;
}

bool SliceSegmentHeader::IsSliceSAOChroma() const
{
    return is_slice_sao_chroma_;
}

bool SliceSegmentHeader::IsUsedCABACInit() const
{
    return is_used_cabac_init_;
}

bool SliceSegmentHeader::IsCUQPDeltaEnabled() const
{
    return pps_->IsCUQPDeltaEnabled();
}

bool SliceSegmentHeader::IsCUChromaQPOffsetEnabled() const
{
    return is_cu_chroma_qp_offset_enabled_;
}

bool SliceSegmentHeader::IsTransquantBypassEnabled() const
{
    return pps_->IsTransquantBypassEnabled();
}

bool SliceSegmentHeader::IsAMPEnabled() const
{
    return sps_->IsAMPEnabled();
}

bool SliceSegmentHeader::IsPCMEnabled() const
{
    return sps_->IsPCMEnabled();
}

ChromaFormatType SliceSegmentHeader::GetChromaFormatType() const
{
    return sps_->GetChromaFormatType();
}

uint32_t SliceSegmentHeader::GetBitDepthLuma() const
{
    return sps_->GetBitDepthLuma();
}

uint32_t SliceSegmentHeader::GetBitDepthChroma() const
{
    return sps_->GetBitDepthChroma();
}

uint32_t SliceSegmentHeader::GetMinCBLog2SizeY() const
{
    return sps_->GetLog2MinLumaCodingBlockSize();
}

uint32_t SliceSegmentHeader::GetMinCBSizeY() const
{
    return sps_->GetMinLumaCodingBlockSizeY();
}

uint32_t SliceSegmentHeader::GetLog2MinCUQPDeltaSize() const
{
    return sps_->GetCTBLog2SizeY() - pps_->GetDiffCUQPDeltaDepth();
}

uint32_t SliceSegmentHeader::GetLog2MinCUChromaQPOffsetSize() const
{
    return sps_->GetCTBLog2SizeY() - 
        pps_->GetPPSRangeExtension().GetDiffCUChromaQPOffsetDepth();
}

uint32_t SliceSegmentHeader::GetMinPCMCodingBlockSizeY() const
{
    return sps_->GetMinPCMCodingBlockSizeY();
}

uint32_t SliceSegmentHeader::GetMaxPCMCodingBlockSizeY() const
{
    return sps_->GetMaxPCMCodingBlockSizeY();
}

uint32_t SliceSegmentHeader::GetMaxTransformHierarchyDepthIntra() const
{
    return sps_->GetMaxTransformHierarchyDepthIntra();
}

uint32_t SliceSegmentHeader::GetMaxTransformHierarchyDepthInter() const
{
    return sps_->GetMaxTransformHierarchyDepthInter();
}

PaletteTable SliceSegmentHeader::GetPalettePredictorInitializer() const
{
    if (pps_->GetPPSSCCExtension().HasPPSPalettePredictorInitializerPresent())
        return pps_->GetPPSSCCExtension().GetPalettePredictorInitializer();

    if (sps_->GetSPSSCCExtension().HasSPSPalettePredictorInitializerPresent())
        return sps_->GetSPSSCCExtension().GetPalettePredictorInitializer(); 

    return PaletteTable();   
}

uint32_t SliceSegmentHeader::GetPaletteMaxSize() const
{
    return sps_->GetSPSSCCExtension().GetPaletteMaxSize();
}

uint32_t SliceSegmentHeader::GetPredictorPaletteMaxSize() const
{
    return sps_->GetSPSSCCExtension().GetPredictorPaletteMaxSize();
}

const vector<int32_t>& SliceSegmentHeader::GetNegativeRefPOCList() const
{
    return negative_ref_poc_list_;
}

const vector<int32_t>& SliceSegmentHeader::GetPositiveRefPOCList() const
{
    return positive_ref_poc_list_;
}

const SPSScreenContentCodingExtension& 
    SliceSegmentHeader::GetSPSScreenContentCodingExtension() const
{
    return sps_->GetSPSSCCExtension();
}

const PPSScreenContentCodingExtension& 
    SliceSegmentHeader::GetPPSScreenContentCodingExtension() const
{
    return pps_->GetPPSSCCExtension();
}

const PPSRangeExtension& SliceSegmentHeader::GetPPSRangeExtension() const
{
    return pps_->GetPPSRangeExtension();
}

uint32_t SliceSegmentHeader::GetChromaQPOffsetListtLen() const
{
    return pps_->GetPPSRangeExtension().GetChromaQPOffsetListtLen();
}

const vector<int32_t>& SliceSegmentHeader::GetCbQPOffsetList() const
{
    return pps_->GetPPSRangeExtension().GetCbQPOffsetList();
}

const vector<int32_t>& SliceSegmentHeader::GetCrQPOffsetList() const
{
    return pps_->GetPPSRangeExtension().GetCrQPOffsetList();
}

bool SliceSegmentHeader::ParseIndependentSyntax(
    BitStream* bit_stream, ISliceSegmentHeaderContext* context)
{
    assert(bit_stream);

    bit_stream->SkipBits(pps_->GetExtraSliceHeaderBitLength());

    GolombReader golomb_reader(bit_stream);
    slice_type_ = static_cast<SliceType>(golomb_reader.ReadUnsignedValue());

    if (pps_->HasOutputFlagPresent())
        bool is_pic_output = bit_stream->ReadBool();

    // 如果是单一色彩的话,则需要知道究竟是何种颜色
    if (sps_->GetChromaFormatType() == YUV_MONO_CHROME)
        uint8_t colour_plane_id = bit_stream->Read<uint8_t>(2);

    bool is_slice_temporal_mvp_enabled = false;
    LongTermRefPOCInfoSet long_term_ref_poc_infos;
    ShortTermReferencePictureSet short_term_ref_poc_infos(
        sps_->GetShortTermReferencePictureSetCount());

    if ((context->GetNalUnitType() != IDR_N_LP) && 
        (context->GetNalUnitType() != IDR_W_RADL))
    {
        uint8_t slice_pic_order_cnt_lsb =
            bit_stream->Read<uint8_t>(sps_->GetPicOrderCountLSBBitLength());
        context->SetPictureOrderCountByLSB(slice_pic_order_cnt_lsb, 
                                           sps_->GetMaxLSBOfPicOrderCount());

        bool success = ParseReferencePictureSet(bit_stream, context,
                                                &long_term_ref_poc_infos, 
                                                &short_term_ref_poc_infos);
        if (!success)
            return false;

        if (sps_->IsTemporalMVPEnabled())
            is_slice_temporal_mvp_enabled = bit_stream->ReadBool();
    }

    if (sps_->IsSampleAdaptiveOffsetEnabled())
    {
        is_slice_sao_luma_ = bit_stream->ReadBool();
        if ((sps_->GetChromaFormatType() != MONO_CHROME) && 
            (sps_->GetChromaFormatType() != YUV_MONO_CHROME))
            is_slice_sao_chroma_ = bit_stream->ReadBool();
    }
    if (slice_type_ != I_SLICE)
    {
        bool success = ParseReferenceDetailInfo(is_slice_temporal_mvp_enabled, 
                                                short_term_ref_poc_infos,
                                                long_term_ref_poc_infos,
                                                bit_stream, context);
        if (!success)
            return false;
    }
    bool success = ParseQuantizationParameterInfo(bit_stream);
    if (!success)
        return false;

    return ParseReconstructPictureInfo(bit_stream);
}

bool SliceSegmentHeader::ParseReferencePictureSet(
    BitStream* bit_stream, ISliceSegmentHeaderContext* context,
    LongTermRefPOCInfoSet* long_term_ref_poc_infos, 
    ShortTermReferencePictureSet* short_term_ref_poc_infos)
{
    assert(long_term_ref_poc_infos);
    assert(short_term_ref_poc_infos);
    if (!long_term_ref_poc_infos || !short_term_ref_poc_infos)
        return false;

    bool has_short_term_ref_pic_set_sps = bit_stream->ReadBool();

    if (!has_short_term_ref_pic_set_sps)
    {
        ShortTermReferencePictureSetContext short_term_rps_context(sps_);
        bool success = 
            short_term_ref_poc_infos->Parse(bit_stream, &short_term_rps_context);
        if (!success)
            return false;
    }
    else 
    {
        uint32_t short_term_rps_count = 
            sps_->GetShortTermReferencePictureSetCount();
        if (short_term_rps_count > 1)
        {
            // TODO: 能够把此处的log2放到context里做
            uint32_t bits = CeilLog2(short_term_rps_count);
            uint32_t short_term_ref_pic_set_idx = bit_stream->Read<int>(bits);
            *short_term_ref_poc_infos = *sps_->GetShortTermReferencePictureSet(
                short_term_ref_pic_set_idx);
        }
    }

    if (sps_->HasLongTermReferencePicturesPresent())
    {
        const vector<LongTermReferenceLSBPictureOrderCountInfo>& 
            lt_ref_lsb_poc_infos = 
                sps_->GetLongTermReferencePictureOrderCountInfos();

        uint32_t num_long_term_sps = 0;
        GolombReader golomb_reader(bit_stream);
        if (lt_ref_lsb_poc_infos.size() > 0)
            num_long_term_sps = golomb_reader.ReadUnsignedValue();

        uint32_t num_long_term_pics = golomb_reader.ReadUnsignedValue();
        // TODO: 可以只算一次,而不是每次读取slice_segment时候都要算
        uint32_t lt_idx_sps_bit_length = CeilLog2(lt_ref_lsb_poc_infos.size());
        
        PictureOrderCount current_poc = context->GetPictureOrderCount();
        auto get_lt_poc_value = 
            [current_poc, this](uint32_t delta_poc_msb_cycle_lt, uint32_t lsb)
        {
            uint32_t lt_msb = 
                delta_poc_msb_cycle_lt * this->sps_->GetMaxLSBOfPicOrderCount();
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
            long_term_ref_poc_infos->push_back(info);
        }

        preview_delta_poc_msb_cycle_lt = 0;
        for (uint32_t i = 0; i < num_long_term_pics; ++i)
        {
            uint32_t poc_lsb_lt = 
                bit_stream->Read<uint32_t>(sps_->GetPicOrderCountLSBBitLength());
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
            long_term_ref_poc_infos->push_back(info);
        }
    }
    return true;
}

bool SliceSegmentHeader::ParseReferenceDetailInfo(
    bool is_slice_temporal_mvp_enabled, 
    const ShortTermReferencePictureSet& short_term_ref_poc_infos,
    const LongTermRefPOCInfoSet& long_term_ref_poc_infos,
    BitStream* bit_stream, ISliceSegmentHeaderContext* context)
{
    bool is_num_ref_idx_active_override = bit_stream->ReadBool();
    GolombReader golomb_reader(bit_stream);
    uint32_t num_ref_idx_negative_active = 
        pps_->GetNumRefIdxNegativeDefaultActive();
    uint32_t num_ref_idx_positive_active = 
        pps_->GetNumRefIdxPositiveDefaultActive();
    if (is_num_ref_idx_active_override)
    {
        num_ref_idx_negative_active = golomb_reader.ReadUnsignedValue() + 1;
        if (B_SLICE == slice_type_)
            num_ref_idx_positive_active = golomb_reader.ReadUnsignedValue() + 1;
    }
    uint32_t reference_picture_count = 
        GetCurrentAvailableReferencePictureCount(short_term_ref_poc_infos, 
                                                 long_term_ref_poc_infos);

    ReferencePictureListsModification ref_pic_list_modification(
        num_ref_idx_negative_active, num_ref_idx_positive_active);
    if (pps_->HasListsModificationPresent() && (reference_picture_count > 1))
    {
        bool success = ref_pic_list_modification.Parse(
            bit_stream, slice_type_, CeilLog2(reference_picture_count));
        if (!success)
            return false;
    }
    bool is_current_picture_ref_enabled = 
        pps_->GetPPSSCCExtension().IsPPSCurrentPictureReferenceEnabled();

    bool success = ConstructReferencePOCList(context, 
                                             is_current_picture_ref_enabled,
                                             short_term_ref_poc_infos,
                                             long_term_ref_poc_infos,
                                             ref_pic_list_modification,
                                             &negative_ref_poc_list_, 
                                             &positive_ref_poc_list_);
    if (!success)
        return false;

    if (B_SLICE == slice_type_)
        bool is_mvd_positive_zero = bit_stream->ReadBool();

    if (pps_->HasCABACInitPresent())
        is_used_cabac_init_ = bit_stream->ReadBool();

    if (is_slice_temporal_mvp_enabled)
    {
        bool is_collocated_from_negative = true;
        if (B_SLICE == slice_type_)
            is_collocated_from_negative = bit_stream->ReadBool();

        if ((is_collocated_from_negative && (num_ref_idx_negative_active > 0)) ||
            (!is_collocated_from_negative && (num_ref_idx_positive_active > 0)))
            uint32_t collocated_ref_idx = golomb_reader.ReadUnsignedValue();
    }
    if ((pps_->HasWeightedPred() && (P_SLICE == slice_type_)) ||
        (pps_->HasWeightedBipred() && (B_SLICE == slice_type_)))
    {
        PredictionWeightTable prediction_weight_table;
        PredictionWeightTableContext prediction_weight_table_context(this, sps_, 
                                                                     context);
        success = prediction_weight_table.Parse(
            bit_stream, &prediction_weight_table_context);
        if (!success)
            return false;
    }
    int32_t max_num_merge_cand = 5 - golomb_reader.ReadUnsignedValue();
    const SPSScreenContentCodingExtension& sps_scc_extension =
        sps_->GetSPSSCCExtension();
    if (sps_scc_extension.GetMotionVectorResolutionControlIDC() == 2)
        bool is_use_integer_mv = bit_stream->ReadBool();

    return true;
}

uint32_t SliceSegmentHeader::GetCurrentAvailableReferencePictureCount(
    const ShortTermReferencePictureSet& short_term_ref_poc_infos,
    const LongTermRefPOCInfoSet& long_term_ref_poc_infos)
{
    uint32_t reference_picture_count = 0;
    for (const auto& i : short_term_ref_poc_infos.GetNegativeDeltaPOCs())
    {
        if (i.can_use_for_current_decode_picture)
            ++reference_picture_count;
    }
    for (const auto& i : short_term_ref_poc_infos.GetPositiveDeltaPOCs())
    {
        if (i.can_use_for_current_decode_picture)
            ++reference_picture_count;
    }

    for (const auto& i : long_term_ref_poc_infos)
    {
        if (i.is_used_by_curr_pic_lt)
            ++reference_picture_count;
    }
    
    return pps_->GetPPSSCCExtension().IsPPSCurrentPictureReferenceEnabled() ?
        reference_picture_count + 1 : reference_picture_count;
}

// 推导出来的所有的参考帧未必要全部用完，因此，有可能会出现
// ref_pic_list_modification.GetListEntryOfNegative().size() < 
// merge_cache_ref_list.size()。 GetListEntryOfNegative同理

bool SliceSegmentHeader::ConstructReferencePOCList(
    ISliceSegmentHeaderContext* context, bool is_current_picture_ref_enabled,
    const ShortTermReferencePictureSet& short_term_ref_poc_infos,
    const LongTermRefPOCInfoSet& current_lt_ref_poc_infos,
    const ReferencePictureListsModification& ref_pic_list_modification,
    vector<int32_t>* negative_ref_pic_list,
    vector<int32_t>* positive_ref_pic_list)
{
    uint32_t current_poc_value = context->GetPictureOrderCount().value;

    // 暂时性还不清楚会不会产生负数,但由于poc都是正数,按照文档定义一致性的原则,认为这里也是
    // 正数
    vector<uint32_t> poc_st_curr_before;
    for (const auto& i : short_term_ref_poc_infos.GetNegativeDeltaPOCs())
    {
        // 相对位置转绝对位置
        uint32_t ref_poc_value = static_cast<uint32_t>(
            i.delta_poc_of_current_decode_picture + current_poc_value);

        if (i.can_use_for_current_decode_picture)
            poc_st_curr_before.push_back(ref_poc_value);
    }
    vector<uint32_t> poc_st_curr_after;
    for (const auto& i : short_term_ref_poc_infos.GetPositiveDeltaPOCs())
    {
        uint32_t ref_poc_value = static_cast<uint32_t>(
            i.delta_poc_of_current_decode_picture + current_poc_value);

        if (i.can_use_for_current_decode_picture)
            poc_st_curr_after.push_back(ref_poc_value);
    }
    vector<uint32_t> poc_lt_curr;
    for (const auto& i : current_lt_ref_poc_infos)
    {
        if (i.is_used_by_curr_pic_lt)
            poc_lt_curr.push_back(i.picture_order_count_value);
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

        for (const auto& i : ref_pic_list_resort_indices)
            ref_list->push_back(merge_cache_ref_list[i]);
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
    BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    GolombReader golomb_reader(bit_stream);
    int32_t slice_qp_delta = golomb_reader.ReadSignedValue();
    quantization_parameter_ = 
        pps_->GetInitQuantizationParameter() + slice_qp_delta;
    if (pps_->HasPPSSliceChromaQPOffsetPresent())
    {
        int32_t slice_cb_qp_offset = golomb_reader.ReadSignedValue();
        int32_t slice_cr_qp_offset = golomb_reader.ReadSignedValue();
    }
    if (pps_->GetPPSSCCExtension().HasPPSSliceActQPOffsetsPresent())
    {
        int32_t slice_act_y_qp_offset = golomb_reader.ReadSignedValue();
        int32_t slice_act_cb_qp_offset = golomb_reader.ReadSignedValue();
        int32_t slice_act_cr_qp_offset = golomb_reader.ReadSignedValue();
    }
    if (pps_->GetPPSRangeExtension().IsChromaQPOffsetListEnabled())
        is_cu_chroma_qp_offset_enabled_ = bit_stream->ReadBool();

    return true;
}

bool SliceSegmentHeader::ParseReconstructPictureInfo(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    bool is_deblocking_filter_override = false;
    if (pps_->IsDeblockingFilterOverrideEnabled())
        is_deblocking_filter_override = bit_stream->ReadBool();

    bool is_slice_deblocking_filter_disabled = pps_->IsDeblockingFilterDisabled();
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
    if (pps_->IsPPSLoopFilterAcrossSliceEnabled() && 
        (is_slice_sao_luma_ || is_slice_sao_chroma_ || 
        !is_slice_deblocking_filter_disabled))
        bool is_slice_loop_filter_across_slice_enabled = bit_stream->ReadBool();

    return true;
}

bool SliceSegmentHeader::ParseTileInfo(BitStream* bit_stream)
{
    if (!bit_stream)
        return false;

    if (pps_->IsTilesEnabled() || pps_->IsEntropyCodingSyncEnabled())
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

#include "hevc_decoder/syntax/coding_unit.h"

#include <algorithm>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/math.h"
#include "hevc_decoder/base/plane_util.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/syntax/coding_unit_context.h"
#include "hevc_decoder/syntax/pcm_sample.h"
#include "hevc_decoder/syntax/prediction_unit.h"
#include "hevc_decoder/syntax/palette_coding.h"
#include "hevc_decoder/syntax/transform_tree.h"
#include "hevc_decoder/syntax/palette_coding_context.h"
#include "hevc_decoder/syntax/prediction_unit_context.h"
#include "hevc_decoder/syntax/transform_tree_context.h"
#include "hevc_decoder/vld_decoder/cu_skip_flag_reader.h"
#include "hevc_decoder/vld_decoder/cu_transquant_bypass_flag_reader.h"
#include "hevc_decoder/vld_decoder/pred_mode_flag_reader.h"
#include "hevc_decoder/vld_decoder/palette_mode_flag_reader.h"
#include "hevc_decoder/vld_decoder/part_mode_reader.h"
#include "hevc_decoder/vld_decoder/pcm_flag_reader.h"
#include "hevc_decoder/vld_decoder/cabac_reader.h"
#include "hevc_decoder/vld_decoder/mpm_idx_reader.h"
#include "hevc_decoder/vld_decoder/prev_intra_luma_pred_flag_reader.h"
#include "hevc_decoder/vld_decoder/rem_intra_luma_pred_mode_reader.h"
#include "hevc_decoder/vld_decoder/intra_chroma_pred_mode_reader.h"
#include "hevc_decoder/vld_decoder/rqt_root_cbf_reader.h"

using std::array;
using std::unique_ptr;
using std::vector;
using std::shared_ptr;
using std::sort;
using boost::multi_array;

class CUSkipFlagReaderContext : public ICUSkipFlagReaderContext
{
public:
    CUSkipFlagReaderContext(const Coordinate& point, uint32_t layer,
                            ICodingUnitContext* cu_context, CodingUnit* cu)
        : point_(point)
        , layer_(layer)
        , cu_context_(cu_context)
        , cu_(cu)
    {

    }

    virtual ~CUSkipFlagReaderContext()
    {

    }

private:
    virtual bool IsLeftBlockAvailable() const override
    {
        Coordinate left_neighbour = {point_.GetX() - 1, point_.GetY()};
        return cu_context_->IsNeighbourBlockAvailable(point_, left_neighbour);
    }

    virtual bool IsUpBlockAvailable() const override
    {
        Coordinate up_neighbour = {point_.GetX(), point_.GetY() - 1};
        return cu_context_->IsNeighbourBlockAvailable(point_, up_neighbour);
    }

    virtual uint32_t GetCurrentBlockLayer() const override
    {
        return cu_->GetCurrentLayer();
    }

    virtual uint32_t GetLeftBlockLayer() const override
    {
        Coordinate left_neighbour = {point_.GetX() - 1, point_.GetY()};
        uint32_t layer = 0;
        bool success = cu_context_->GetNearestCULayerByCoordinate(left_neighbour, 
                                                                  &layer);

        return success ? layer : 0;
    }

    virtual uint32_t GetUpBlockLayer() const override
    {
        Coordinate up_neighbour = {point_.GetX(), point_.GetY() - 1};
        uint32_t layer = 0;
        bool success = cu_context_->GetNearestCULayerByCoordinate(up_neighbour,
                                                                  &layer);

        return success ? layer : 0;
    }

    Coordinate point_;
    uint32_t layer_;
    ICodingUnitContext* cu_context_;
    CodingUnit* cu_;
};

class PartModeReaderContext : public IPartModeReaderContext
{
public:
    PartModeReaderContext(CodingUnit* cu, ICodingUnitContext* cu_context)
        : cu_(cu)
        , cu_context_(cu_context)
    {

    }

    virtual ~PartModeReaderContext()
    {

    }

private:
    virtual uint32_t GetCBSizeY() const override
    {
        return cu_->GetCBSizeY();
    }

    virtual bool IsAsymmetricMotionPartitionsEnabled() const override
    {
        return cu_context_->IsAsymmetricMotionPartitionsEnabled();
    }

    virtual uint32_t GetMinCBSizeY() const override
    {
        return cu_context_->GetMinCBSizeY();
    }

    virtual PredModeType GetCUPredMode() const override
    {
        return cu_->GetPredMode();
    }

    CodingUnit* cu_;
    ICodingUnitContext* cu_context_;
};

class PredictionUnitContext : public IPredictionUnitContext
{
public:
    PredictionUnitContext(bool is_cu_skip)
        : is_cu_skip_(is_cu_skip)
    {

    }

    virtual ~PredictionUnitContext()
    {

    }

    virtual bool IsCUSkip() const override
    {
        return is_cu_skip_;
    }

private:
    bool is_cu_skip_;
};

class PaletteCodingContext : public IPaletteCodingContext
{
public:
    PaletteCodingContext(ICodingUnitContext* cu_context)
        : cu_context_(cu_context)
    {

    }

    virtual ~PaletteCodingContext()
    {

    }

    virtual shared_ptr<PaletteTable> GetPredictorPaletteTable() const override
    {
        return cu_context_->GetPredictorPaletteTable();
    }

    virtual uint32_t GetPaletteMaxSize() const override
    {
        return cu_context_->GetPaletteMaxSize();
    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return cu_context_->GetChromaFormatType();
    }

    virtual uint32_t GetBitDepthOfLuma() const override
    {
        return cu_context_->GetBitDepthOfLuma();
    }

    virtual uint32_t GetBitDepthOfChroma() const override
    {
        return cu_context_->GetBitDepthOfChroma();
    }

    virtual uint32_t GetPredictorPaletteMaxSize() const override
    {
        return cu_context_->GetPredictorPaletteMaxSize();
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return cu_context_->GetCABACInitType();
    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return cu_context_->IsCUQPDeltaEnabled();
    }

    virtual bool IsCUQPDeltaCoded() const override
    {
        return cu_context_->IsCUQPDeltaCoded();
    } 

    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) override
    {
        cu_context_->SetCUQPDeltaVal(cu_qp_delta_val);
    }

    virtual bool IsCUTransquantBypass() const override
    {
        return coding_unit_->IsCUTransquantBypass();
    }

    virtual bool IsCUChromaQPOffsetEnable() const override
    {
        return cu_context_->IsCUChromaQPOffsetEnable();
    }

    virtual bool IsCUChromaQPOffsetCoded() const override
    {
        return cu_context_->IsCUChromaQPOffsetCoded();
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return cu_context_->GetChromaQPOffsetListtLen();
    }

    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index)
        override
    {
        cu_context_->SetCUChromaQPOffsetIndex(cu_chroma_qp_offset_index);
    }

private:
    ICodingUnitContext* cu_context_;
    CodingUnit* coding_unit_;
};

class TransformTreeContextInCU : public ITransformTreeContext
{
public:
    TransformTreeContextInCU(ICodingUnitContext* cu_context,
                             CodingUnit* cu, uint32_t max_transform_tree_layer, 
                             bool is_intra_split)
        : cu_context_(cu_context)
        , cu_(cu)
        , max_transform_tree_layer_(max_transform_tree_layer)
        , is_intra_split_(is_intra_split)
        , default_transform_coefficient_flag_({true, true})
    {

    }

    virtual ~TransformTreeContextInCU()
    {

    }

    virtual uint32_t GetMaxTransformBlockSizeY() const override
    {
        return cu_context_->GetMaxTransformBlockSizeY();
    }

    virtual uint32_t GetMinTransformBlockSizeY() const override
    {
        return cu_context_->GetMinTransformBlockSizeY();
    }

    virtual uint32_t GetMaxTransformTreeDepth() const override
    {
        return max_transform_tree_layer_;
    }

    virtual bool IsAllowIntraSplit() const override
    {
        return is_intra_split_;
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return cu_context_->GetCABACInitType();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthInter() const override
    {
        return cu_context_->GetMaxTransformHierarchyDepthInter();
    }

    virtual PredModeType GetCUPredMode() const override
    {
        return cu_->GetPredMode();
    }

    virtual PartModeType GetPartMode() const override
    {
        return cu_->GetPartMode();
    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return cu_context_->GetChromaFormatType();
    }

    virtual const array<bool, 2>&
        IsPreviousCBContainedTransformCoefficientOfColorBlue() const override
    {
        return default_transform_coefficient_flag_;
    }

    virtual const array<bool, 2>&
        IsPreviousCBContainedTransformCoefficientOfColorRed() const override
    {
        return default_transform_coefficient_flag_;
    }

    virtual uint32_t GetMinCBLog2SizeY() const override
    {
        return cu_context_->GetMinCBLog2SizeY();
    }

    virtual uint32_t GetMinCBSizeY() const override
    {
        return cu_context_->GetMinCBSizeY();
    }

    virtual bool IsResidualAdaptiveColorTransformEnabled() const override
    {
        return cu_context_->IsResidualAdaptiveColorTransformEnabled();
    }

    virtual const vector<uint32_t>& GetIntraChromaPredModeIdentification() const 
        override
    {
        return cu_->GetIntraChromaPredModeIdentification();
    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return cu_context_->IsCUQPDeltaEnabled();
    }

    virtual bool IsCUQPDeltaCoded() const override
    {
        return cu_context_->IsCUQPDeltaCoded();
    }

    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) override
    {
        cu_context_->SetCUQPDeltaVal(cu_qp_delta_val);
    }

    virtual bool IsCUTransquantBypass() const override
    {
        return cu_->IsCUTransquantBypass();
    }

    virtual bool IsCUChromaQPOffsetEnable() const override
    {
        return cu_context_->IsCUChromaQPOffsetEnable();
    }

    virtual bool IsCUChromaQPOffsetCoded() const override
    {
        return cu_context_->IsCUChromaQPOffsetCoded();
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return cu_context_->GetChromaQPOffsetListtLen();
    }

    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index)
        override
    {
        cu_context_->SetCUChromaQPOffsetIndex(cu_chroma_qp_offset_index);
    }

    virtual bool IsCrossComponentPredictionEnabled() const override
    {
        return cu_context_->IsCrossComponentPredictionEnabled();
    }

    virtual bool IsTransformSkipEnabled() const override
    {
        return cu_context_->IsTransformSkipEnabled();
    }

    virtual uint32_t GetMaxTransformSkipSize() const override
    {
        return cu_context_->GetMaxTransformSkipSize();
    }

    virtual bool IsExplicitRDPCMEnabled() const override
    {
        return cu_context_->IsExplicitRDPCMEnabled();
    }

    virtual IntraPredModeType GetIntraLumaPredMode(const Coordinate& c) const
        override
    {
        return cu_->GetIntraLumaPredMode(c);
    }

    virtual IntraPredModeType GetIntraChromaPredMode(const Coordinate& c) const
        override
    {
        return cu_->GetIntraChromaPredMode(c);
    }

    virtual bool IsTransformSkipContextEnabled() const override
    {
        return cu_context_->IsTransformSkipContextEnabled();
    }

    virtual bool IsImplicitRDPCMEnabled() const override
    {
        return cu_context_->IsImplicitRDPCMEnabled();
    }

    virtual bool IsCABACBypassAlignmentEnabled() const override
    {
        return cu_context_->IsCABACBypassAlignmentEnabled();
    }

    virtual bool IsSignDataHidingEnabled() const override
    {
        return cu_context_->IsSignDataHidingEnabled();
    }

    virtual bool IsPersistentRiceAdaptationEnabled() const override
    {
        return cu_context_->IsPersistentRiceAdaptationEnabled();
    }

    virtual uint32_t GetBitDepthOfLuma() const override
    {
        return cu_context_->GetBitDepthOfLuma();
    }

    virtual uint32_t GetBitDepthOfChroma() const override
    {
        return cu_context_->GetBitDepthOfChroma();
    }

    virtual bool HasExtendedPrecisionProcessing() const override
    {
        return cu_context_->HasExtendedPrecisionProcessing();
    }

private:
    ICodingUnitContext* cu_context_;
    CodingUnit* cu_;
    uint32_t max_transform_tree_layer_;
    bool is_intra_split_;
    array<bool, 2> default_transform_coefficient_flag_;
};

CodingUnit::CodingUnit(const Coordinate& point, uint32_t layer, 
                       uint32_t cb_size_y)
    : point_(point)
    , layer_(layer)
    , cb_size_y_(cb_size_y)
    , pred_mode_(MODE_SKIP)
    , part_mode_(PART_2Nx2N)
    , is_cu_transquant_bypass_(false)
    , intra_chroma_pred_mode_identification_()
    , intra_luma_pred_modes_()
    , intra_chroma_pred_modes_()
    , is_pcm_(false)
    , prediction_units_()
{

}

CodingUnit::~CodingUnit()
{

}

bool CodingUnit::Parse(CABACReader* cabac_reader, ICodingUnitContext* context)
{
    if (context->IsTransquantBypassEnabled())
    {
        CUTransquantBypassFlagReader reader(cabac_reader, 
                                            context->GetCABACInitType());
        is_cu_transquant_bypass_ = reader.Read();
    }
    bool is_cu_skip = false;
    if (context->GetSliceType() != I_SLICE)
    {
        CUSkipFlagReaderContext cu_skip_reader_context(point_, layer_, context, 
                                                       this);
        CUSkipFlagReader reader(cabac_reader, context->GetCABACInitType(), 
                                &cu_skip_reader_context);
        is_cu_skip = reader.Read();
    }
    if (is_cu_skip)
    {
        shared_ptr<PredictionUnit> prediction_unit(
            new PredictionUnit(point_, cb_size_y_, cb_size_y_));
        PredictionUnitContext prediction_unit_context(is_cu_skip);
        if (!prediction_unit->Parse(cabac_reader, &prediction_unit_context))
            return false;

        prediction_units_.push_back(prediction_unit);
        return true;
    }
    return ParseDetailInfo(cabac_reader, context, is_cu_skip);
}

uint32_t CodingUnit::GetCurrentLayer() const
{
    return layer_;
}

uint32_t CodingUnit::GetCBSizeY() const
{
    return cb_size_y_;
}

PredModeType CodingUnit::GetPredMode() const
{
    return pred_mode_;
}

PartModeType CodingUnit::GetPartMode() const
{
    return part_mode_;
}

bool CodingUnit::IsCUTransquantBypass() const
{
    return is_cu_transquant_bypass_;
}

bool CodingUnit::IsPCM() const
{
    return is_pcm_;
}

const vector<uint32_t>& CodingUnit::GetIntraChromaPredModeIdentification() const
{
    return intra_chroma_pred_mode_identification_;
}

IntraPredModeType CodingUnit::GetIntraLumaPredMode(const Coordinate& c) const
{
    assert(IsPointInSquare(c, point_, cb_size_y_));
    if (!IsPointInSquare(c, point_, cb_size_y_))
        return INTRA_DC;

    if (intra_luma_pred_modes_.size() == 1)
        return intra_luma_pred_modes_[0];

    Coordinate relative_c = {c.GetX() - point_.GetX(), c.GetY() - point_.GetY()};
    return intra_luma_pred_modes_[GetQuadrant(relative_c, cb_size_y_)];
}

IntraPredModeType CodingUnit::GetIntraChromaPredMode(const Coordinate& c) const
{
    assert(IsPointInSquare(c, point_, cb_size_y_));
    if (!IsPointInSquare(c, point_, cb_size_y_))
        return INTRA_DC;

    if (intra_chroma_pred_modes_.size() == 1)
        return intra_chroma_pred_modes_[0];

    Coordinate relative_c = {c.GetX() - point_.GetX(), c.GetY() - point_.GetY()};
    return intra_chroma_pred_modes_[GetQuadrant(relative_c, cb_size_y_)];
}

const Coordinate& CodingUnit::GetCoordinate() const
{
    return point_;
}

bool CodingUnit::ParseDetailInfo(CABACReader* cabac_reader,
                                 ICodingUnitContext* context, bool is_cu_skip)
{
    if (context->GetSliceType() != I_SLICE)
    {
        PredModeFlagReader reader(cabac_reader, context->GetCABACInitType());
        pred_mode_ = reader.Read();
    }
    else
    {
        pred_mode_ = MODE_INTRA;
    }

    bool is_palette_mode = false;
    if (context->IsPaletteModeEnabled() && (MODE_INTRA == pred_mode_) &&
        (cb_size_y_ <= context->GetMaxTransformBlockSizeY()))
    {
        PaletteModeFlagReader palette_mode_flag_reader(
            cabac_reader, context->GetCABACInitType());
        is_palette_mode = palette_mode_flag_reader.Read();
    }

    if (is_palette_mode)
    {
        PaletteCoding palette_coding(cb_size_y_);
        PaletteCodingContext palette_coding_context(context);
        return palette_coding.Parse(cabac_reader, &palette_coding_context);
    }
    else
    {
        if ((pred_mode_ != MODE_INTRA) || 
            (context->GetMinCBSizeY() == cb_size_y_))
        {
            PartModeReaderContext part_mode_reader_context(this, context);
            PartModeReader part_mode_reader(cabac_reader, 
                                            context->GetCABACInitType(),
                                            &part_mode_reader_context);
            part_mode_ = part_mode_reader.Read();
        }
        bool success = false;
        if (MODE_INTRA == pred_mode_)
        {
            success = 
                ParseIntraDetailInfo(cabac_reader, context, part_mode_);
        }
        else
        {
            success = ParseInterDetailInfo(cabac_reader, context, is_cu_skip, 
                                            part_mode_);
        }

        if (!success)
            return false;

        if (!is_pcm_)
        {
            // rqt_root_cbf
            bool has_transform_tree = true;
            if ((pred_mode_ != MODE_INTRA) &&
                !((PART_2Nx2N == part_mode_) && !prediction_units_.empty() &&
                    prediction_units_[0]->IsMergeMode()))
            {
                RQTRootCBFReader reader(cabac_reader, 
                                        context->GetCABACInitType());
                has_transform_tree = reader.Read();
            }
            if (has_transform_tree)
            {
                uint32_t max_transform_depth = 
                    context->GetMaxTransformHierarchyDepthInter();
                bool is_intra_split = false;
                if (MODE_INTRA == pred_mode_)
                {
                    max_transform_depth = 
                        context->GetMaxTransformHierarchyDepthIntra();
                    if (PART_NxN == part_mode_)
                    {
                        max_transform_depth += 1;
                        is_intra_split = true;
                    }
                }
                uint32_t log2_cb_size_y = Log2(cb_size_y_);
                TransformTree transform_tree(point_, point_, log2_cb_size_y, 
                                                0, 0);
                TransformTreeContextInCU transform_tree_context(
                    context, this, max_transform_depth, is_intra_split);

                return transform_tree.Parse(cabac_reader, 
                                            &transform_tree_context);
            }
        }
    }
    return true;
}

bool CodingUnit::ParseIntraDetailInfo(CABACReader* cabac_reader, 
                                      ICodingUnitContext* context, 
                                      PartModeType part_mode)
{
    if (!cabac_reader || !context)
        return false;

    is_pcm_ = false;
    if ((PART_2NxN == part_mode) && context->IsPCMEnabled() &&
        (cb_size_y_ >= context->GetMinPCMCodingBlockSizeY()) &&
        (cb_size_y_ <= context->GetMaxPCMCodingBlockSizeY()))
    {
        is_pcm_ = PCMFlagReader(cabac_reader).Read();
    }
    if (is_pcm_)
    {
        BitStream* bit_stream = cabac_reader->GetSourceBitStream();
        bit_stream->ByteAlign();
        PCMSample samples;
        if (!samples.Parse(bit_stream))
            return false;

        cabac_reader->Reset();
        return true;
    }
    // 把pbOffset去掉，改为block_count,其实就是按照part_mode把cu分成多少个而已
    uint32_t block_count = (PART_NxN == part_mode) ? 4 : 1;
    if (!ParseIntraLumaPredictedMode(cabac_reader, context, block_count))
        return false;

    intra_chroma_pred_mode_identification_.resize(block_count);
    // 如果是444模式，则每个块都有相对应的帧内预测模式
    if ((context->GetChromaFormatType() == YUV_444) || 
        (context->GetChromaFormatType() == YUV_MONO_CHROME))
    {
        for (uint32_t i = 0; i < block_count; ++i)
        {
            IntraChromaPredModeReader reader(cabac_reader, 
                                             context->GetCABACInitType());
            intra_chroma_pred_mode_identification_[i] = reader.Read();
        }
    }
    else if (context->GetChromaFormatType() != MONO_CHROME)
    {
        IntraChromaPredModeReader reader(cabac_reader,
                                         context->GetCABACInitType());
        intra_chroma_pred_mode_identification_[0] = reader.Read();
    }
    if (context->GetChromaFormatType() != MONO_CHROME)
    {
        return DeriveIntraChromaPredictedModes(
            context, intra_luma_pred_modes_, 
            intra_chroma_pred_mode_identification_);
    }
    return true;
}

bool CodingUnit::ParseInterDetailInfo(CABACReader* cabac_reader, 
                                      ICodingUnitContext* context, 
                                      bool is_cu_skip, PartModeType part_mode)
{
    if (PART_2Nx2N == part_mode)
    {
        shared_ptr<PredictionUnit> prediction_unit(
            new PredictionUnit(point_, cb_size_y_, cb_size_y_));
        prediction_units_.push_back(prediction_unit);
    }
    else if (PART_2NxN == part_mode)
    {
        uint32_t unit_height = cb_size_y_ >> 1;
        shared_ptr<PredictionUnit> prediction_up_unit(
            new PredictionUnit(point_, cb_size_y_, unit_height));
        prediction_units_.push_back(prediction_up_unit);

        Coordinate down_unit_point(point_.GetX(), point_.GetY() + unit_height);
        shared_ptr<PredictionUnit> prediction_down_unit(
            new PredictionUnit(down_unit_point, cb_size_y_, unit_height));
        prediction_units_.push_back(prediction_down_unit);
    }
    else if (PART_Nx2N == part_mode)
    {
        uint32_t unit_width = cb_size_y_ >> 1;
        shared_ptr<PredictionUnit> prediction_left_unit(
            new PredictionUnit(point_, unit_width, cb_size_y_));
        prediction_units_.push_back(prediction_left_unit);

        Coordinate right_unit_point(point_.GetX() + unit_width, point_.GetY());
        shared_ptr<PredictionUnit> prediction_right_unit(
            new PredictionUnit(right_unit_point, unit_width, cb_size_y_));
        prediction_units_.push_back(prediction_right_unit);
    }
    else if (PART_2NxnU == part_mode)
    {
        uint32_t up_unit_height = cb_size_y_ >> 2;
        shared_ptr<PredictionUnit> prediction_up_unit(
            new PredictionUnit(point_, cb_size_y_, up_unit_height));
        prediction_units_.push_back(prediction_up_unit);

        Coordinate down_unit_point(point_.GetX(), 
                                   point_.GetY() + up_unit_height);
        shared_ptr<PredictionUnit> prediction_down_unit(
            new PredictionUnit(down_unit_point, cb_size_y_, 
                               cb_size_y_  - up_unit_height));
        prediction_units_.push_back(prediction_down_unit);
    }
    else if (PART_2NxnD == part_mode)
    {
        uint32_t up_unit_height = cb_size_y_ - (cb_size_y_ >> 2);
        shared_ptr<PredictionUnit> prediction_up_unit(
            new PredictionUnit(point_, cb_size_y_, up_unit_height));
        prediction_units_.push_back(prediction_up_unit);

        Coordinate down_unit_point(point_.GetX(), point_.GetY() + up_unit_height);
        shared_ptr<PredictionUnit> prediction_down_unit(
            new PredictionUnit(down_unit_point, cb_size_y_,
                               cb_size_y_ - up_unit_height));
        prediction_units_.push_back(prediction_down_unit);
    }
    else if (PART_nLx2N == part_mode)
    {
        uint32_t left_unit_width = cb_size_y_ >> 1;
        shared_ptr<PredictionUnit> prediction_left_unit(
            new PredictionUnit(point_, left_unit_width, cb_size_y_));
        prediction_units_.push_back(prediction_left_unit);

        Coordinate right_unit_point(point_.GetX() + left_unit_width, 
                                    point_.GetY());
        shared_ptr<PredictionUnit> prediction_right_unit(
            new PredictionUnit(right_unit_point, cb_size_y_ - left_unit_width, 
                               cb_size_y_));
        prediction_units_.push_back(prediction_right_unit);
    }
    else if (PART_nRx2N == part_mode)
    {
        uint32_t left_unit_width = cb_size_y_ - (cb_size_y_ >> 1);
        shared_ptr<PredictionUnit> prediction_left_unit(
            new PredictionUnit(point_, left_unit_width, cb_size_y_));
        prediction_units_.push_back(prediction_left_unit);

        Coordinate right_unit_point(point_.GetX() + left_unit_width, 
                                    point_.GetY());
        shared_ptr<PredictionUnit> prediction_right_unit(
            new PredictionUnit(right_unit_point, cb_size_y_ - left_unit_width,
                               cb_size_y_));
        prediction_units_.push_back(prediction_right_unit);
    }
    else
    {
        uint32_t prediction_unit_size_y = cb_size_y_ >> 1;
        shared_ptr<PredictionUnit> prediction_left_up_unit(
            new PredictionUnit(point_, prediction_unit_size_y, 
                               prediction_unit_size_y));
        prediction_units_.push_back(prediction_left_up_unit);
        
        Coordinate right_up_unit_point(point_.GetX() + prediction_unit_size_y, 
                                       point_.GetY());
        shared_ptr<PredictionUnit> prediction_right_up_unit(
            new PredictionUnit(right_up_unit_point, prediction_unit_size_y,
                               prediction_unit_size_y));
        prediction_units_.push_back(prediction_right_up_unit);

        Coordinate left_down_unit_point(point_.GetX(), 
                                        point_.GetY() + prediction_unit_size_y);
        shared_ptr<PredictionUnit> prediction_left_down_unit(
            new PredictionUnit(left_down_unit_point, prediction_unit_size_y,
                               prediction_unit_size_y));
        prediction_units_.push_back(prediction_left_down_unit);

        Coordinate right_down_unit_point(point_.GetX() + prediction_unit_size_y, 
                                         point_.GetY() + prediction_unit_size_y);
        shared_ptr<PredictionUnit> prediction_right_down_unit(
            new PredictionUnit(left_down_unit_point, prediction_unit_size_y,
                               prediction_unit_size_y));
        prediction_units_.push_back(prediction_left_down_unit);
    }
    for (auto& unit : prediction_units_)
    {
        PredictionUnitContext prediction_unit_context(is_cu_skip);
        if (!unit->Parse(cabac_reader, &prediction_unit_context))
            return false;
    }

    return !prediction_units_.empty();
}

bool CodingUnit::ParseIntraLumaPredictedMode(CABACReader* cabac_reader, 
                                             ICodingUnitContext* context, 
                                             uint32_t block_count)
{
    multi_array<bool, 1> is_prev_intra_luma_pred(boost::extents[block_count]);
    for (uint32_t i = 0; i < block_count; ++i)
    {
        PrevIntraLumaPredFlagReader reader(cabac_reader,
                                           context->GetCABACInitType());
        is_prev_intra_luma_pred[i] = reader.Read();
    }

    vector<uint32_t> mpm_idx;
    mpm_idx.resize(block_count);
    vector<uint32_t> rem_intra_luma_pred_mode;
    rem_intra_luma_pred_mode.resize(block_count);
    for (uint32_t i = 0; i < block_count; ++i)
    {
        if (is_prev_intra_luma_pred[i])
        {
            uint32_t idx = MPMIdxReader(cabac_reader).Read();
            mpm_idx[i] = idx;
        }
        else
        {
            uint32_t luma_pred_mode =
                RemIntraLumaPredModeReader(cabac_reader).Read();
            rem_intra_luma_pred_mode[i] = luma_pred_mode;
        }
    }
    return DeriveIntraLumaPredictedModes(context, is_prev_intra_luma_pred, 
                                         mpm_idx, rem_intra_luma_pred_mode);
}
   
// 8.4.2
bool CodingUnit::DeriveIntraLumaPredictedModes(
    ICodingUnitContext* context, 
    const multi_array<bool, 1>& is_prev_intra_luma_pred, 
    const vector<uint32_t>& mpm_idx, 
    const vector<uint32_t>& rem_intra_luma_pred_mode)
{
    assert(mpm_idx.size() == rem_intra_luma_pred_mode.size());
    if (mpm_idx.size() != rem_intra_luma_pred_mode.size())
        return false;
    

    intra_luma_pred_modes_.resize(mpm_idx.size());
    uint32_t pb_size_y = cb_size_y_ >> 1;
    for (uint32_t i = 0; i < mpm_idx.size(); ++i)
    {
        Coordinate current_pb_point = 
        {
            point_.GetX() + (pb_size_y * (i & 1)),
            point_.GetY() + (pb_size_y * ((i >> 1) & 1))
        };

        Coordinate left_neighbour_point = 
        {
            current_pb_point.GetX() - 1, current_pb_point.GetY(),
        };

        Coordinate up_neighbour_point = 
        {
            current_pb_point.GetX(), current_pb_point.GetY() - 1,
        };

        intra_luma_pred_modes_[i] = DeriveSingleIntraLumaPredictedMode(
            context, is_prev_intra_luma_pred[i], mpm_idx[i], 
            rem_intra_luma_pred_mode[i], current_pb_point, up_neighbour_point, 
            left_neighbour_point);
    }
    return true;
}

IntraPredModeType CodingUnit::DeriveSingleIntraLumaPredictedMode(
    ICodingUnitContext* context, bool is_prev_intra_luma_pred, uint32_t mpm_idx, 
    uint32_t rem_intra_luma_pred_mode, const Coordinate& current_pb_point,
    const Coordinate& up_neighbour_point, const Coordinate& left_neighbour_point)
{
    auto left_pb_intra_luma_pred_mode = 
        GetNeighbourBlockIntraPredModeType(context, current_pb_point, 
                                           left_neighbour_point);
    auto up_pb_intra_luma_pb_point = 
        GetNeighbourBlockIntraPredModeType(context, current_pb_point, 
                                           up_neighbour_point);

    auto candidate_intra_pred_modes = GetCandidateIntraPredModes(
        left_pb_intra_luma_pred_mode, up_pb_intra_luma_pb_point);

    if (is_prev_intra_luma_pred)
        return candidate_intra_pred_modes[mpm_idx];

    sort(candidate_intra_pred_modes.begin(), candidate_intra_pred_modes.end());
    for (const auto& i : candidate_intra_pred_modes)
        if (rem_intra_luma_pred_mode >= static_cast<uint32_t>(i))
            ++rem_intra_luma_pred_mode;

    return static_cast<IntraPredModeType>(rem_intra_luma_pred_mode);
}

IntraPredModeType CodingUnit::GetNeighbourBlockIntraPredModeType(
    ICodingUnitContext* context, const Coordinate& current_point,
    const Coordinate& neighbour_point)
{
    bool is_available = 
        context->IsNeighbourBlockAvailable(current_point, neighbour_point);
    if (!is_available)
        return INTRA_DC;

    if (neighbour_point.GetY() < context->GetCTUBaseCoordinate().GetY())
        return INTRA_DC;

    auto neighbour_cu = context->GetCodingUnit(neighbour_point);
    if (!neighbour_cu)
        return INTRA_DC;

    if ((neighbour_cu->GetPredMode() != MODE_INTRA) || neighbour_cu->IsPCM())
        return INTRA_DC;

    return neighbour_cu->GetIntraLumaPredMode(neighbour_point);
}

array<IntraPredModeType, 3> CodingUnit::GetCandidateIntraPredModes(
    IntraPredModeType left_pb_intra_luma_pred_mode, 
    IntraPredModeType up_pb_intra_luma_pred_mode)
{
    array<IntraPredModeType, 3> candidate_intra_pred_modes;
    if (left_pb_intra_luma_pred_mode == up_pb_intra_luma_pred_mode)
    {
        if ((INTRA_PLANAR == left_pb_intra_luma_pred_mode) ||
            (INTRA_DC == left_pb_intra_luma_pred_mode))
        {
            candidate_intra_pred_modes[0] = INTRA_PLANAR;
            candidate_intra_pred_modes[1] = INTRA_DC;
            candidate_intra_pred_modes[2] = INTRA_ANGULAR26;
        }
        else
        {
            candidate_intra_pred_modes[0] = left_pb_intra_luma_pred_mode;
            candidate_intra_pred_modes[1] = static_cast<IntraPredModeType>(2 +
                ((static_cast<uint32_t>(left_pb_intra_luma_pred_mode) + 29) %
                 32));
            candidate_intra_pred_modes[2] = static_cast<IntraPredModeType>(2 +
                ((static_cast<uint32_t>(left_pb_intra_luma_pred_mode) - 2 + 1) %
                 32));
        }
    }
    else
    {
        candidate_intra_pred_modes[0] = left_pb_intra_luma_pred_mode;
        candidate_intra_pred_modes[1] = up_pb_intra_luma_pred_mode;
        if ((candidate_intra_pred_modes[0] != INTRA_PLANAR) &&
            (candidate_intra_pred_modes[1] != INTRA_PLANAR))
            candidate_intra_pred_modes[2] = INTRA_PLANAR;
        else if ((candidate_intra_pred_modes[0] != INTRA_DC) &&
            (candidate_intra_pred_modes[1] != INTRA_DC))
            candidate_intra_pred_modes[2] = INTRA_DC;
        else
            candidate_intra_pred_modes[2] = INTRA_ANGULAR26;
    }
    return candidate_intra_pred_modes;
}

// 8.4.3
bool CodingUnit::DeriveIntraChromaPredictedModes(
    ICodingUnitContext* context, 
    const vector<IntraPredModeType>& intra_luma_pred_modes, 
    const vector<uint32_t>& intra_chroma_pred_mode_identification)
{
    int mode_indices[5][5] = {34,  0,  0,  0,  0,  
                              26, 34, 26, 26, 26,
                              10, 10, 34, 10, 10,
                               1,  1,  1, 34,  1,
                               0, 26, 10,  1, -1};

    uint32_t intra_luma_pred_modes_indices[35] = 
    {
        0, 3, 4, 4, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 1, 4, 4, 4, 4, 4, 4, 4, 4
    };

    IntraPredModeType candidate_intra_chroma_predicted_modes[35] = 
    {
        INTRA_PLANAR, INTRA_DC, INTRA_ANGULAR2, INTRA_ANGULAR2, INTRA_ANGULAR2, 
        INTRA_ANGULAR2, INTRA_ANGULAR3, INTRA_ANGULAR5, INTRA_ANGULAR7, 
        INTRA_ANGULAR8, INTRA_ANGULAR10, INTRA_ANGULAR12, INTRA_ANGULAR13, 
        INTRA_ANGULAR15, INTRA_ANGULAR17, INTRA_ANGULAR18, INTRA_ANGULAR19, 
        INTRA_ANGULAR20, INTRA_ANGULAR21, INTRA_ANGULAR22, INTRA_ANGULAR23, 
        INTRA_ANGULAR23, INTRA_ANGULAR24, INTRA_ANGULAR24, INTRA_ANGULAR25, 
        INTRA_ANGULAR25, INTRA_ANGULAR26, INTRA_ANGULAR27, INTRA_ANGULAR27, 
        INTRA_ANGULAR28, INTRA_ANGULAR28, INTRA_ANGULAR29, INTRA_ANGULAR29, 
        INTRA_ANGULAR30, INTRA_ANGULAR31
    };

    assert(intra_chroma_pred_mode_identification.size() == 
           intra_luma_pred_modes.size());

    if (intra_chroma_pred_mode_identification.size() != 
        intra_luma_pred_modes.size())
        return false;
   
    intra_chroma_pred_modes_.resize(intra_luma_pred_modes.size());

    // 内部函数，不进行任何判断
    auto mode_index_fetchor = [&](uint32_t i) -> int
    {
        uint32_t luma_pred_mode_index =
            intra_luma_pred_modes_indices[intra_luma_pred_modes[i]];

        uint32_t chroma_pred_mode_index = 
            intra_chroma_pred_mode_identification[i];

        int mode_index =
            mode_indices[chroma_pred_mode_index][luma_pred_mode_index];

        if (-1 == mode_index)
            mode_index = static_cast<int>(intra_luma_pred_modes[i]);

        return mode_index;
    };

    if (context->GetChromaFormatType() == YUV_422)
    {
        for (uint32_t i = 0; i < intra_luma_pred_modes.size(); ++i)
        {
            intra_chroma_pred_modes_[i] = 
                candidate_intra_chroma_predicted_modes[mode_index_fetchor(i)];
        }
    }
    else
    {
        for (uint32_t i = 0; i < intra_luma_pred_modes.size(); ++i)
        {
            intra_chroma_pred_modes_[i] = 
                static_cast<IntraPredModeType>(mode_index_fetchor(i));
        }
    }
    return true;
}

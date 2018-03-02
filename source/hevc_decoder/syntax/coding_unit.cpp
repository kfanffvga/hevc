#include "hevc_decoder/syntax/coding_unit.h"

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/syntax/coding_unit_context.h"
#include "hevc_decoder/syntax/pcm_sample.h"
#include "hevc_decoder/syntax/prediction_unit.h"
#include "hevc_decoder/syntax/palette_coding.h"
#include "hevc_decoder/syntax/transform_tree.h"
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

using std::unique_ptr;
using std::vector;
using std::shared_ptr;

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
        Coordinate left_neighbour = {point_.x - 1, point_.y};
        return cu_context_->IsNeighbourBlockAvailable(point_, left_neighbour);
    }

    virtual bool IsUpBlockAvailable() const override
    {
        Coordinate up_neighbour = {point_.x, point_.y - 1};
        return cu_context_->IsNeighbourBlockAvailable(point_, up_neighbour);
    }

    virtual uint32_t GetCurrentBlockLayer() const override
    {
        return cu_->GetCurrentLayer();
    }

    virtual uint32_t GetLeftBlockLayer() const override
    {
        Coordinate left_neighbour = {point_.x - 1, point_.y};
        return cu_context_->GetNearestCULayerByCoordinate(left_neighbour);
    }

    virtual uint32_t GetUpBlockLayer() const override
    {
        Coordinate up_neighbour = {point_.x, point_.y - 1};
        return cu_context_->GetNearestCULayerByCoordinate(up_neighbour);
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

CodingUnit::CodingUnit(const Coordinate& point, uint32_t layer, 
                       uint32_t cb_size_y)
    : point_(point)
    , layer_(layer)
    , cb_size_y_(cb_size_y)
    , pred_mode_(MODE_SKIP)
{

}

CodingUnit::~CodingUnit()
{

}

bool CodingUnit::Parse(CABACReader* cabac_reader, ICodingUnitContext* context)
{
    if (!context->IsTransquantBypassEnabled())
    {
        CUTransquantBypassFlagReader reader(cabac_reader, 
                                            context->GetCABACInitType());
        bool is_cu_transquant_bypass = reader.Read();
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
        if (!prediction_unit->Parse(cabac_reader))
            return false;

        prediction_units_.push_back(prediction_unit);
        return true;
    }
    return ParseDetailInfo(cabac_reader, context);
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

bool CodingUnit::ParseDetailInfo(CABACReader* cabac_reader,
                                 ICodingUnitContext* context)
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
        return palette_coding.Parse(cabac_reader);
    }
    else
    {
        PartModeType part_mode = PART_2Nx2N;
        if ((pred_mode_ != MODE_INTRA) || 
            (context->GetMinCBSizeY() == cb_size_y_))
        {
            PartModeReaderContext part_mode_reader_context(this, context);
            PartModeReader part_mode_reader(cabac_reader, 
                                            context->GetCABACInitType(),
                                            &part_mode_reader_context);
            part_mode = part_mode_reader.Read();

            bool success = false;
            bool is_pcm = false;
            if (MODE_INTRA == pred_mode_)
            {
                success = ParseIntraDetailInfo(cabac_reader, context, part_mode,
                                               &is_pcm);
            }
            else
            {
                success = ParseInterDetailInfo(cabac_reader, context, part_mode);
            }

            if (!success)
                return false;

            if (!is_pcm)
            {
                // rqt_root_cbf
                bool has_transform_tree = true;
                if ((pred_mode_ != MODE_INTRA) &&
                    !((PART_2Nx2N == part_mode) && !prediction_units_.empty() &&
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
                    if (MODE_INTRA == pred_mode_)
                    {
                        max_transform_depth = 
                            context->GetMaxTransformHierarchyDepthIntra();
                        if (PART_NxN == part_mode)
                            max_transform_depth += 1;
                    }
                    TransformTree transform_tree(point_, point_, cb_size_y_, 0, 
                                                 0);
                    return transform_tree.Parse(cabac_reader);
                }
            }
        }
    }
    return true;
}

bool CodingUnit::ParseIntraDetailInfo(CABACReader* cabac_reader, 
                                      ICodingUnitContext* context, 
                                      PartModeType part_mode, bool* is_pcm)
{
    if (!is_pcm || !cabac_reader || !context)
        return false;

    *is_pcm = false;
    if ((PART_2NxN == part_mode) && context->IsPCMEnabled() &&
        (cb_size_y_ >= context->GetMinPCMCodingBlockSizeY()) &&
        (cb_size_y_ <= context->GetMaxPCMCodingBlockSizeY()))
    {
        *is_pcm = PCMFlagReader(cabac_reader).Read();
    }
    if (*is_pcm)
    {
        BitStream* bit_stream = cabac_reader->GetSourceBitStream();
        bit_stream->ByteAlign();
        PCMSample samples;
        if (!samples.Parse(bit_stream))
            return false;

        cabac_reader->Reset();
        return true;
    }
    // 此处用一维数组表示，这里无非是要么一个，要么把一个分为4个，因此用一维数据意义是一样的
    // 并且把pbOffset去掉，改为block_count
    uint32_t block_count = (PART_NxN == part_mode) ? 4 : 1;
    unique_ptr<bool> is_prev_intra_luma_pred(new bool[block_count]);
    for (uint32_t i = 0; i < block_count; ++i)
    {
        PrevIntraLumaPredFlagReader reader(cabac_reader, 
                                           context->GetCABACInitType());
        is_prev_intra_luma_pred.get()[i] = reader.Read();
    }
    
    vector<uint32_t> mpm_idx;
    mpm_idx.resize(block_count);
    vector<uint32_t> rem_intra_luma_pred_mode;
    rem_intra_luma_pred_mode.resize(block_count);
    for (uint32_t i = 0; i < block_count; ++i)
    {
        if (is_prev_intra_luma_pred.get()[i])
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
    vector<uint32_t> intra_chroma_pred_mode;
    intra_chroma_pred_mode.resize(block_count);
    // 如果是444模式，则每个块都有相对应的帧内预测模式
    if (context->GetChromaFormatType() == YUV_444)
    {
        for (uint32_t i = 0; i < block_count; ++i)
        {
            IntraChromaPredModeReader reader(cabac_reader, 
                                             context->GetCABACInitType());
            uint32_t chroma_pred_mode = reader.Read();
            intra_chroma_pred_mode[i] = chroma_pred_mode;
        }
    }
    else if ((context->GetChromaFormatType() != MONO_CHROME) &&
        (context->GetChromaFormatType() != YUV_MONO_CHROME))
    {
        IntraChromaPredModeReader reader(cabac_reader,
                                         context->GetCABACInitType());
        uint32_t chroma_pred_mode = reader.Read();
        intra_chroma_pred_mode[0] = chroma_pred_mode;
    }
    return true;
}

bool CodingUnit::ParseInterDetailInfo(CABACReader* cabac_reader, 
                                      ICodingUnitContext* context,
                                      PartModeType part_mode)
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

        Coordinate down_unit_point = {point_.x, point_.y + unit_height};
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

        Coordinate right_unit_point = {point_.x + unit_width, point_.y};
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

        Coordinate down_unit_point = {point_.x, point_.y + up_unit_height};
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

        Coordinate down_unit_point = {point_.x, point_.y + up_unit_height};
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

        Coordinate right_unit_point = {point_.x + left_unit_width, point_.y};
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

        Coordinate right_unit_point = {point_.x + left_unit_width, point_.y};
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
        
        Coordinate right_up_unit_point = 
            {point_.x + prediction_unit_size_y, point_.y};
        shared_ptr<PredictionUnit> prediction_right_up_unit(
            new PredictionUnit(right_up_unit_point, prediction_unit_size_y,
                               prediction_unit_size_y));
        prediction_units_.push_back(prediction_right_up_unit);

        Coordinate left_down_unit_point =
            {point_.x, point_.y + prediction_unit_size_y};
        shared_ptr<PredictionUnit> prediction_left_down_unit(
            new PredictionUnit(left_down_unit_point, prediction_unit_size_y,
                               prediction_unit_size_y));
        prediction_units_.push_back(prediction_left_down_unit);

        Coordinate right_down_unit_point ={point_.x + prediction_unit_size_y, 
                                           point_.y + prediction_unit_size_y};
        shared_ptr<PredictionUnit> prediction_right_down_unit(
            new PredictionUnit(left_down_unit_point, prediction_unit_size_y,
                               prediction_unit_size_y));
        prediction_units_.push_back(prediction_left_down_unit);
    }
    for (auto& unit : prediction_units_)
    {
        if (!unit->Parse(cabac_reader))
            return false;
    }

    return !prediction_units_.empty();
}

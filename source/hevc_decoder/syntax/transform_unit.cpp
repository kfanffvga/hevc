#include "hevc_decoder/syntax/transform_unit.h"

#include <algorithm>
#include <vector>

#include "hevc_decoder/base/math.h"
#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/transform_unit_context.h"
#include "hevc_decoder/syntax/delta_qp.h"
#include "hevc_decoder/syntax/delta_qp_context.h"
#include "hevc_decoder/syntax/chroma_qp_offset.h"
#include "hevc_decoder/syntax/chroma_qp_offset_context.h"
#include "hevc_decoder/syntax/residual_coding_context.h"
#include "hevc_decoder/syntax/residual_coding.h"
#include "hevc_decoder/syntax/cross_comp_pred.h"
#include "hevc_decoder/syntax/cross_comp_pred_context.h"
#include "hevc_decoder/vld_decoder/tu_residual_act_flag_reader.h"

using std::max;
using std::vector;
using std::unique_ptr;

class DeltaQPContextForTransformUnit : public IDeltaQPContext
{
public:
    DeltaQPContextForTransformUnit(ITransformUnitContext* tu_context)
        : tu_context_(tu_context)
    {

    }

    virtual ~DeltaQPContextForTransformUnit()
    {

    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return tu_context_->IsCUQPDeltaEnabled();
    }

    virtual bool IsCUQPDeltaCoded() const override
    {
        return tu_context_->IsCUQPDeltaCoded();
    }

    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) override
    {
        tu_context_->SetCUQPDeltaVal(cu_qp_delta_val);
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return tu_context_->GetCABACInitType();
    }

private:
    ITransformUnitContext* tu_context_;
};

class ChromaQPOffsetForTransformUnit : public IChromaQPOffsetContext
{
public:
    ChromaQPOffsetForTransformUnit(ITransformUnitContext* tu_context)
        : tu_context_(tu_context)
    {

    }
    virtual ~ChromaQPOffsetForTransformUnit()
    {

    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return tu_context_->GetCABACInitType();
    }

    virtual bool IsCUChromaQPOffsetEnable() const override
    {
        return tu_context_->IsCUChromaQPOffsetEnable();
    }

    virtual bool IsCUChromaQPOffsetCoded() const override
    {
        return tu_context_->IsCUChromaQPOffsetCoded();
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return tu_context_->GetChromaQPOffsetListtLen();
    }

    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index)
        override
    {
        tu_context_->SetCUChromaQPOffsetIndex(cu_chroma_qp_offset_index);
    }

private:
    ITransformUnitContext* tu_context_;
};

class ResidualCodingContext : public IResidualCodingContext
{
public:
    ResidualCodingContext()
    {

    }

    virtual ~ResidualCodingContext()
    {

    }
};

class CrossCompPredContext : public ICrossCompPredContext
{
public:
    CrossCompPredContext()
    {

    }

    virtual ~CrossCompPredContext()
    {

    }
};

TransformUnit::TransformUnit(uint32_t block_index)
    : block_index_(block_index)
{

}

TransformUnit::~TransformUnit()
{

}

bool TransformUnit::Parse(CABACReader* cabac_reader, 
                          ITransformUnitContext* context)
{
    uint32_t chroma_transform_size = context->GetTransformBlockSize();
    ChromaFormatType chroma_format_type = context->GetChromaFormatType();
    if ((YUV_444 == chroma_format_type) || 
        (YUV_MONO_CHROME == chroma_format_type))
        chroma_transform_size >>= 1;

    chroma_transform_size = max(4ui32, chroma_transform_size);

    uint32_t cbf_depth_chroma = context->GetTransformBlockDepth();
    Coordinate coordinate_for_chroma = context->GetCurrentCoordinate();
    if (((YUV_444 == chroma_format_type) ||
        (YUV_MONO_CHROME == chroma_format_type)) && (4 == chroma_transform_size))
    {
        cbf_depth_chroma -= 1;
        coordinate_for_chroma = context->GetBaseCoordinate();
    }
        
    bool has_coef_for_luma = context->GetCBFLumaValue();
    bool has_coef_for_chroma = 
        context->GetCBFColorBlueValue(0, cbf_depth_chroma) ||
        context->GetCBFColorRedValue(0, cbf_depth_chroma) || 
        ((YUV_422 == chroma_format_type) && 
            (context->GetCBFColorBlueValue(1, cbf_depth_chroma) || 
             context->GetCBFColorRedValue(1, cbf_depth_chroma)));

    if (has_coef_for_luma || has_coef_for_chroma)
    {
        return ParseResidualOfTransformCoefficient(cabac_reader, context, 
                                                   has_coef_for_chroma, 
                                                   has_coef_for_luma);
    }

    return true;
}

bool TransformUnit::ParseResidualOfTransformCoefficient(
    CABACReader* cabac_reader, ITransformUnitContext* context, 
    bool has_coef_for_chroma, bool has_coef_for_luma)
{
    if (context->IsResidualAdaptiveColorTransformEnabled())
    {
        bool is_intra_chroma_pred_mode_four = false;
        if (context->GetCUPredMode() != MODE_INTER)
        {
            const vector<uint32_t>& intra_chroma_pred_mode =
                context->GetIntraChromaPredMode();

            assert(!intra_chroma_pred_mode.empty());
            if (intra_chroma_pred_mode.empty())
                return false;

            is_intra_chroma_pred_mode_four = 
                (context->GetPartMode() == PART_2Nx2N) && 
                (4 == intra_chroma_pred_mode[0]);

            bool is_all_four = false;
            for (const auto& i : intra_chroma_pred_mode)
                is_all_four &= (4 == i);

            is_intra_chroma_pred_mode_four |= is_all_four;
        }
        if ((context->GetCUPredMode() == MODE_INTER) ||
            is_intra_chroma_pred_mode_four)
        {
            TUResidualActFlagReader reader(cabac_reader, 
                                           context->GetCABACInitType());
            bool has_tu_residual_act = reader.Read();
        }
    }
    DeltaQP delta_qp;
    DeltaQPContextForTransformUnit delta_qp_context(context);
    if (!delta_qp.Parse(cabac_reader, &delta_qp_context))
        return false;

    if (has_coef_for_chroma && !context->IsCUTransquantBypass())
    {
        ChromaQPOffset chroma_qp_offset;
        ChromaQPOffsetForTransformUnit chroma_qp_offset_context(context);
        if (!chroma_qp_offset.Parse(cabac_reader, &chroma_qp_offset_context))
            return false;
    }
    
    return ParseResidualDetailInfo(cabac_reader, context, has_coef_for_luma);
}

bool TransformUnit::ParseResidualDetailInfo(CABACReader* cabac_reader, 
                                            ITransformUnitContext* context, 
                                            bool has_coef_for_luma)
{
    if (has_coef_for_luma)
    {
        ResidualCoding residual_coding_for_luma;
        ResidualCodingContext residual_coding_context;
        bool success = residual_coding_for_luma.Parse(cabac_reader, 
                                                      &residual_coding_context);
        if (!success)
            return false;
    }
    ChromaFormatType chroma_format_type = context->GetChromaFormatType();
    if ((context->GetTransformBlockSize() > 4) ||
        ((YUV_444 == chroma_format_type) ||
        (YUV_MONO_CHROME == chroma_format_type)))
    {
        auto cbf_chroma_blue_value_getter = 
            [context](uint32_t index, uint32_t depth) -> bool
        {
            return context->GetCBFColorBlueValue(index, depth);
        };
        CrossCompPred chroma_blue_cross_comp_pred;
        vector<unique_ptr<ResidualCoding>> chroma_blue_residual_codings;
        bool success = 
            ParseSingleChromaResidualInfoWithCrossCompomentPrediction(
                cabac_reader, context, has_coef_for_luma, 
                cbf_chroma_blue_value_getter, &chroma_blue_cross_comp_pred, 
                &chroma_blue_residual_codings);
        if (!success)
            return false;

        auto cbf_chroma_red_value_getter = 
            [context](uint32_t index, uint32_t depth) -> bool
        {
            return context->GetCBFColorRedValue(index, depth);
        };
        CrossCompPred chroma_red_cross_comp_pred;
        vector<unique_ptr<ResidualCoding>> chroma_red_residual_codings;
        success = ParseSingleChromaResidualInfoWithCrossCompomentPrediction(
            cabac_reader, context, has_coef_for_luma,
            cbf_chroma_red_value_getter, &chroma_red_cross_comp_pred,
            &chroma_red_residual_codings);
        if (!success)
            return false;
    }
    else if (3 == block_index_)
    {
        uint32_t residual_coding_count = (YUV_422 == chroma_format_type) ? 2 : 1;
        uint32_t depth = context->GetTransformBlockDepth();
        vector<unique_ptr<ResidualCoding>> chroma_blue_residual_codings;
        chroma_blue_residual_codings.resize(residual_coding_count);
        for (uint32_t i = 0; i < residual_coding_count; ++i)
        {
            if (!context->GetCBFColorBlueValue(i, depth - 1))
            {
                chroma_blue_residual_codings[i].reset(new ResidualCoding());
                ResidualCodingContext residual_coding_context;
                bool success = chroma_blue_residual_codings[i]->Parse(
                    cabac_reader, &residual_coding_context);
                if (!success)
                    return false;
            }
        }
        
        vector<unique_ptr<ResidualCoding>> chroma_red_residual_codings;
        chroma_red_residual_codings.resize(residual_coding_count);
        for (uint32_t i = 0; i < residual_coding_count; ++i)
        {
            if (!context->GetCBFColorRedValue(i, depth - 1))
            {
                chroma_red_residual_codings[i].reset(new ResidualCoding());
                ResidualCodingContext residual_coding_context;
                bool success = chroma_red_residual_codings[i]->Parse(
                    cabac_reader, &residual_coding_context);
                if (!success)
                    return false;
            }
        }
    }
    return true;
}

bool TransformUnit::ParseSingleChromaResidualInfoWithCrossCompomentPrediction(
    CABACReader* cabac_reader, ITransformUnitContext* context, 
    bool has_coef_for_luma, const CBFChromaGetter& get_cbf_chroma_value,
    CrossCompPred* cross_comp_pred, 
    vector<unique_ptr<ResidualCoding>>* residual_codings)
{
    if (!cross_comp_pred || !residual_codings)
        return false;

    if (context->IsCrossComponentPredictionEnabled() && has_coef_for_luma &&
        ((context->GetCUPredMode() == MODE_INTER) ||
        (!context->GetIntraChromaPredMode().empty() &&
         (4 == context->GetIntraChromaPredMode()[0]))))
    {
        CrossCompPredContext cross_comp_pred_context;
        if (!cross_comp_pred->Parse(cabac_reader, &cross_comp_pred_context))
            return false;
    }

    ChromaFormatType chroma_format_type = context->GetChromaFormatType();
    uint32_t residual_coding_count = (YUV_422 == chroma_format_type) ? 2 : 1;
    residual_codings->resize(residual_coding_count);
    for (uint32_t i = 0; i < residual_coding_count; ++i)
    {
        if (get_cbf_chroma_value(i, context->GetTransformBlockDepth()))
        {
            (*residual_codings)[i].reset(new ResidualCoding());
            ResidualCodingContext residual_coding_context;
            bool success = (*residual_codings)[i]->Parse(
                cabac_reader, &residual_coding_context);

            if (!success)
                return false;
        }
    }
    return true;
}

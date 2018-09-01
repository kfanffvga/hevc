#include "hevc_decoder/syntax/transform_tree.h"

#include "hevc_decoder/base/math.h"
#include "hevc_decoder/syntax/transform_tree_context.h"
#include "hevc_decoder/syntax/transform_unit.h"
#include "hevc_decoder/syntax/transform_unit_context.h"
#include "hevc_decoder/vld_decoder/split_transform_flag_reader.h"
#include "hevc_decoder/vld_decoder/cbf_cb_reader.h"
#include "hevc_decoder/vld_decoder/cbf_cr_reader.h"
#include "hevc_decoder/vld_decoder/cbf_luma_reader.h"

using std::array;
using std::vector;

class TransformTreeContextInMySelf : public ITransformTreeContext
{
public:
    TransformTreeContextInMySelf(ITransformTreeContext* parent_node_context,
                                 TransformTree* transform_tree_node)
        : parent_node_context_(parent_node_context)
        , transform_tree_node_(transform_tree_node)
    {

    }

    virtual ~TransformTreeContextInMySelf()
    {

    }

    virtual uint32_t GetMaxTransformBlockSizeY() const override
    {
        return parent_node_context_->GetMaxTransformBlockSizeY();
    }

    virtual uint32_t GetMinTransformBlockSizeY() const override
    {
        return parent_node_context_->GetMinTransformBlockSizeY();
    }

    virtual uint32_t GetMaxTransformTreeDepth() const override
    {
        return parent_node_context_->GetMaxTransformTreeDepth();
    }

    virtual bool IsAllowIntraSplit() const override
    {
        return parent_node_context_->IsAllowIntraSplit();
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return parent_node_context_->GetCABACInitType();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthInter() const override
    {
        return parent_node_context_->GetMaxTransformHierarchyDepthInter();
    }

    virtual PredModeType GetCUPredMode() const override
    {
        return parent_node_context_->GetCUPredMode();
    }

    virtual PartModeType GetPartMode() const override
    {
        return parent_node_context_->GetPartMode();
    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return parent_node_context_->GetChromaFormatType();
    }

    virtual const array<bool, 2>& 
        IsPreviousCBContainedTransformCoefficientOfColorBlue() const override
    {
        return 
            transform_tree_node_->IsCBContainedTransformCoefficientOfColorBlue();
    }

    virtual const array<bool, 2>& 
        IsPreviousCBContainedTransformCoefficientOfColorRed() const override
    {
        return 
            transform_tree_node_->IsCBContainedTransformCoefficientOfColorRed();
    }

    virtual uint32_t GetMinCBLog2SizeY() const override
    {
        return parent_node_context_->GetMinCBLog2SizeY();
    }

    virtual uint32_t GetMinCBSizeY() const override
    {
        return parent_node_context_->GetMinCBLog2SizeY();
    }

    virtual bool IsResidualAdaptiveColorTransformEnabled() const override
    {
        return parent_node_context_->IsResidualAdaptiveColorTransformEnabled();
    }

    virtual const vector<uint32_t>& GetIntraChromaPredModeIdentification() const
        override
    {
        return parent_node_context_->GetIntraChromaPredModeIdentification();
    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return parent_node_context_->IsCUQPDeltaEnabled();
    }

    virtual bool IsCUQPDeltaCoded() const override
    {
        return parent_node_context_->IsCUQPDeltaCoded();
    }

    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) override
    {
        parent_node_context_->SetCUQPDeltaVal(cu_qp_delta_val);
    }

    virtual bool IsCUTransquantBypass() const override
    {
        return parent_node_context_->IsCUTransquantBypass();
    }

    virtual bool IsCUChromaQPOffsetEnable() const override
    {
        return parent_node_context_->IsCUChromaQPOffsetEnable();
    }

    virtual bool IsCUChromaQPOffsetCoded() const override
    {
        return parent_node_context_->IsCUChromaQPOffsetCoded();
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return parent_node_context_->GetChromaQPOffsetListtLen();
    }

    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index)
        override
    {
        parent_node_context_->SetCUChromaQPOffsetIndex(cu_chroma_qp_offset_index);
    }

    virtual bool IsCrossComponentPredictionEnabled() const override
    {
        return parent_node_context_->IsCrossComponentPredictionEnabled();
    }
    
    virtual bool IsTransformSkipEnabled() const override
    {
        return parent_node_context_->IsTransformSkipEnabled();
    }

    virtual uint32_t GetMaxTransformSkipSize() const override
    {
        return parent_node_context_->GetMaxTransformSkipSize();
    }

    virtual bool IsExplicitRDPCMEnabled() const override
    {
        return parent_node_context_->IsExplicitRDPCMEnabled();
    }

    virtual IntraPredModeType GetIntraLumaPredMode(const Coordinate& c) const
        override
    {
        return parent_node_context_->GetIntraLumaPredMode(c);
    }

    virtual IntraPredModeType GetIntraChromaPredMode(const Coordinate& c) const
        override
    {
        return parent_node_context_->GetIntraChromaPredMode(c);
    }

    virtual bool IsTransformSkipContextEnabled() const override
    {
        return parent_node_context_->IsTransformSkipContextEnabled();
    }

    virtual bool IsImplicitRDPCMEnabled() const override
    {
        return parent_node_context_->IsImplicitRDPCMEnabled();
    }

    virtual bool IsCABACBypassAlignmentEnabled() const override
    {
        return parent_node_context_->IsCABACBypassAlignmentEnabled();
    }

    virtual bool IsSignDataHidingEnabled() const override
    {
        return parent_node_context_->IsSignDataHidingEnabled();
    }

    virtual bool IsPersistentRiceAdaptationEnabled() const override
    {
        return parent_node_context_->IsPersistentRiceAdaptationEnabled();
    }

    virtual uint32_t GetBitDepthOfLuma() const override
    {
        return parent_node_context_->GetBitDepthOfLuma();
    }

    virtual uint32_t GetBitDepthOfChroma() const override
    {
        return parent_node_context_->GetBitDepthOfChroma();
    }
    
    virtual bool HasExtendedPrecisionProcessing() const override
    {
        return parent_node_context_->HasExtendedPrecisionProcessing();
    }

private:
    ITransformTreeContext* parent_node_context_;
    TransformTree* transform_tree_node_;
};

class TransformUnitContext : public ITransformUnitContext
{
public:
    TransformUnitContext(TransformTree* tree, 
                         ITransformTreeContext* tree_context)
        : tree_(tree)
        , tree_context_(tree_context)
    {

    }

    virtual ~TransformUnitContext()
    {

    }

    virtual uint32_t GetTransformBlockDepth() const override
    {
        return tree_->GetTreeDepth();
    }

    virtual const Coordinate& GetBaseCoordinate() const override
    {
        return tree_->GetBaseCoordinate();
    }

    virtual const Coordinate& GetCurrentCoordinate() const override
    {
        return tree_->GetCurrentCoordinate();
    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return tree_context_->GetChromaFormatType();
    }

    virtual bool GetCBFColorBlueValue(uint32_t index, uint32_t depth) const
        override
    {
        assert(index < 2);
        if (tree_->GetTreeDepth() == depth)
            return tree_->IsCBContainedTransformCoefficientOfColorBlue()[index];

        auto coef_value = 
            tree_context_->IsPreviousCBContainedTransformCoefficientOfColorBlue();
        return coef_value[index];
    }

    virtual bool GetCBFColorRedValue(uint32_t index, uint32_t depth) const
        override
    {
        assert(index < 2);
        if (tree_->GetTreeDepth() == depth)
            return tree_->IsCBContainedTransformCoefficientOfColorRed()[index];

        auto coef_value =
            tree_context_->IsPreviousCBContainedTransformCoefficientOfColorRed();
        return coef_value[index];

    }

    virtual bool GetCBFLumaValue() const override
    {
        return tree_->IsCBContainedTransformCoefficientOfLuma();
    }

    virtual uint32_t GetMinCBLog2SizeY() const override
    {
        return tree_context_->GetMinCBLog2SizeY();
    }

    virtual uint32_t GetMinCBSizeY() const override
    {
        return tree_context_->GetMinCBSizeY();
    }

    virtual bool IsResidualAdaptiveColorTransformEnabled() const override
    {
        return tree_context_->IsResidualAdaptiveColorTransformEnabled();
    }

    virtual PredModeType GetCUPredMode() const override
    {
        return tree_context_->GetCUPredMode();
    }

    virtual PartModeType GetPartMode() const override
    {
        return tree_context_->GetPartMode();
    }

    virtual const vector<uint32_t>& GetIntraChromaPredModeIdentification() const 
        override
    {
        return tree_context_->GetIntraChromaPredModeIdentification();
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return tree_context_->GetCABACInitType();
    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return tree_context_->IsCUQPDeltaEnabled();
    }

    virtual bool IsCUQPDeltaCoded() const override
    {
        return tree_context_->IsCUQPDeltaCoded();
    }

    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) override
    {
        tree_context_->SetCUQPDeltaVal(cu_qp_delta_val);
    }

    virtual bool IsCUTransquantBypass() const override
    {
        return tree_context_->IsCUTransquantBypass();
    }

    virtual bool IsCUChromaQPOffsetEnable() const override
    {
        return tree_context_->IsCUChromaQPOffsetEnable();
    }

    virtual bool IsCUChromaQPOffsetCoded() const override
    {
        return tree_context_->IsCUChromaQPOffsetCoded();
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return tree_context_->GetChromaQPOffsetListtLen();
    }

    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index)
        override
    {
        return tree_context_->SetCUChromaQPOffsetIndex(cu_chroma_qp_offset_index);
    }

    virtual bool IsCrossComponentPredictionEnabled() const override
    {
        return tree_context_->IsCrossComponentPredictionEnabled();
    }

    virtual bool IsTransformSkipEnabled() const override
    {
        return tree_context_->IsTransformSkipEnabled();
    }

    virtual uint32_t GetMaxTransformSkipSize() const override
    {
        return tree_context_->GetMaxTransformSkipSize();
    }

    virtual bool IsExplicitRDPCMEnabled() const override
    {
        return  tree_context_->IsExplicitRDPCMEnabled();
    }

    virtual IntraPredModeType GetIntraLumaPredMode(const Coordinate& c) const
        override
    {
        return tree_context_->GetIntraLumaPredMode(c);
    }

    virtual IntraPredModeType GetIntraChromaPredMode(const Coordinate& c) const
        override
    {
        return tree_context_->GetIntraChromaPredMode(c);
    }
    
    virtual bool IsTransformSkipContextEnabled() const override
    {
        return tree_context_->IsTransformSkipContextEnabled();
    }

    virtual bool IsImplicitRDPCMEnabled() const override
    {
        return tree_context_->IsImplicitRDPCMEnabled();
    }

    virtual bool IsCABACBypassAlignmentEnabled() const override
    {
        return tree_context_->IsCABACBypassAlignmentEnabled();
    }

    virtual bool IsSignDataHidingEnabled() const override
    {
        return tree_context_->IsSignDataHidingEnabled();
    }

    virtual bool IsPersistentRiceAdaptationEnabled() const override
    {
        return tree_context_->IsPersistentRiceAdaptationEnabled();
    }

    virtual uint32_t GetBitDepthOfLuma() const override
    {
        return tree_context_->GetBitDepthOfLuma();
    }

    virtual uint32_t GetBitDepthOfChroma() const override
    {
        return tree_context_->GetBitDepthOfChroma();
    }

    virtual bool HasExtendedPrecisionProcessing() const override
    {
        return tree_context_->HasExtendedPrecisionProcessing();
    }

private:
    TransformTree* tree_;
    ITransformTreeContext* tree_context_;
};

TransformTree::TransformTree(const Coordinate& current, const Coordinate& base, 
                             uint32_t log2_transform_unit_size_y, uint32_t depth, 
                             uint32_t block_index)
    : current_coordinate_(current)
    , base_coordinate_(base)
    , transform_unit_size_y_(1 << log2_transform_unit_size_y)
    , log2_transform_unit_size_y_(log2_transform_unit_size_y)
    , depth_(depth)
    , block_index_(block_index)
    , cbf_cb_()
    , cbf_cr_()
    , cbf_luma_(false)
    , sub_transform_trees_()
{

}

TransformTree::~TransformTree()
{

}

bool TransformTree::Parse(CABACReader* cabac_reader, 
                          ITransformTreeContext* context)
{
    bool is_split_transform = false;
    if ((transform_unit_size_y_ <= context->GetMaxTransformBlockSizeY()) &&
        (transform_unit_size_y_ > context->GetMinTransformBlockSizeY()) &&
        (depth_ < context->GetMaxTransformTreeDepth()) &&
        !(context->IsAllowIntraSplit() && (0 == depth_)))
    {
        SplitTransformFlagReader reader(cabac_reader, 
                                        context->GetCABACInitType(), 
                                        log2_transform_unit_size_y_);
        is_split_transform = reader.Read();
    }
    else
    {
        // Concretely, this means that sub-CB transformed are required when 
        // inter splits are used and max_transform_hierarchy_depth_inter is 
        // set to 0. In other words, when the inter coded block has been split 
        // for prediction purposes and that transform blocks should be the same 
        // size as coded blocks, bypass the restriction and subdivide the 
        // transform block.

        // 因为context->GetMaxTransformHierarchyDepthInter() 等于 0 因此可以理解为
        // 没有分割更小的tb，所以tb的大小与pb的大小一样，在这种情况下，就可以按照上面的解释
        // 进行分割，但至于为什么要这样做，估计要等高人指点了
        bool is_inter_split = 
            (context->GetMaxTransformHierarchyDepthInter() == 0) &&
            (context->GetCUPredMode() == MODE_INTER) && 
            (context->GetPartMode() != PART_2Nx2N) && (0 == depth_);

        is_split_transform = 
            (transform_unit_size_y_ > context->GetMaxTransformBlockSizeY()) ||
            (context->IsAllowIntraSplit() && (0 == depth_)) || is_inter_split;

    }
    bool success = ParseCBFCBValues(cabac_reader, context, is_split_transform);
    if (!success)
        return false;

    if (is_split_transform)
        success = ParseSubTransformTree(cabac_reader, context);
    else
        success = ParseTransformUnit(cabac_reader, context);

    return success;
}

const array<bool, 2>& 
    TransformTree::IsCBContainedTransformCoefficientOfColorBlue() const
{
    return cbf_cb_;
}

const array<bool, 2>& 
    TransformTree::IsCBContainedTransformCoefficientOfColorRed() const
{
    return cbf_cr_;
}

bool TransformTree::IsCBContainedTransformCoefficientOfLuma() const
{
    return cbf_luma_;
}

uint32_t TransformTree::GetBlockSize() const
{
    return transform_unit_size_y_;
}

uint32_t TransformTree::GetTreeDepth() const
{
    return depth_;
}

const Coordinate& TransformTree::GetBaseCoordinate() const
{
    return base_coordinate_;
}

const Coordinate& TransformTree::GetCurrentCoordinate() const
{
    return current_coordinate_;
}

bool TransformTree::ParseCBFCBValues(CABACReader* cabac_reader,
                                     ITransformTreeContext* context, 
                                     bool is_split_transform)
{
    ChromaFormatType chroma_format_type = context->GetChromaFormatType();
    if (((YUV_MONO_CHROME == chroma_format_type) || (YUV_444 == chroma_format_type)) ||
        ((log2_transform_unit_size_y_ > 2) && (chroma_format_type != MONO_CHROME)))
    {
        if ((0 == depth_) ||
            context->IsPreviousCBContainedTransformCoefficientOfColorBlue()[0])
        {
            {
                CBFCBReader reader(cabac_reader, context->GetCABACInitType(), 
                                   depth_);
                cbf_cb_[0] = reader.Read();
            }
            if ((YUV_422 == chroma_format_type) &&
                (!is_split_transform || (3 == log2_transform_unit_size_y_)))
            {
                CBFCBReader reader(cabac_reader, context->GetCABACInitType(),
                                   depth_);
                cbf_cb_[1] = reader.Read();
            }
        }
        if ((0 == depth_) ||
            context->IsPreviousCBContainedTransformCoefficientOfColorRed()[0])
        {
            {
                CBFCRReader reader(cabac_reader, context->GetCABACInitType(),
                                   depth_);
                cbf_cr_[0] = reader.Read();
            }
            if ((YUV_422 == chroma_format_type) &&
                (!is_split_transform || (3 == log2_transform_unit_size_y_)))
            {
                CBFCRReader reader(cabac_reader, context->GetCABACInitType(),
                                   depth_);
                cbf_cr_[1] = reader.Read();
            }
        }
    }
    return true;
}

bool TransformTree::ParseSubTransformTree(CABACReader* cabac_reader, 
                                          ITransformTreeContext* context)
{
    uint32_t sub_transform_tree_size_y = transform_unit_size_y_ >> 1;
    uint32_t x = current_coordinate_.GetX() + sub_transform_tree_size_y;
    uint32_t y = current_coordinate_.GetY() + sub_transform_tree_size_y;
    array<Coordinate, 4> sub_transform_trees_coordinate = 
    {
        current_coordinate_, {x, current_coordinate_.GetY()}, 
        {current_coordinate_.GetX(), y}, {x, y}
    };
    uint32_t sub_layer = depth_ + 1;
    for (uint32_t i = 0; i < sub_transform_trees_coordinate.size(); ++i)
    {
        sub_transform_trees_[i].reset(
            new TransformTree(sub_transform_trees_coordinate[i], 
                              current_coordinate_, 
                              log2_transform_unit_size_y_ - 1, sub_layer, i));

        TransformTreeContextInMySelf sub_transform_tree_context(context, this);
        bool success = 
            sub_transform_trees_[i]->Parse(cabac_reader, 
                                           &sub_transform_tree_context);
        if (!success)
            return false;
    }
    return true;
}

bool TransformTree::ParseTransformUnit(CABACReader* cabac_reader, 
                                       ITransformTreeContext* context)
{
    if ((context->GetCUPredMode() == MODE_INTRA) || (depth_ != 0) ||
        cbf_cb_[0] || cbf_cr_[0] ||
        ((context->GetChromaFormatType() == YUV_422) && cbf_cr_[1] && cbf_cb_[1]))
    {
        CBFLumaReader reader(cabac_reader, context->GetCABACInitType(), depth_);
        cbf_luma_ = reader.Read();
    }
    TransformUnit transform_unit(block_index_, log2_transform_unit_size_y_);
    TransformUnitContext transform_unit_context(this, context);
    return transform_unit.Parse(cabac_reader, &transform_unit_context);
}

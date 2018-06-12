#include "hevc_decoder/syntax/coding_quadtree.h"

#include <cassert>

#include "hevc_decoder/syntax/coding_quadtree_context.h"
#include "hevc_decoder/vld_decoder/split_cu_flag_reader.h"
#include "hevc_decoder/syntax/coding_unit.h"
#include "hevc_decoder/syntax/coding_unit_context.h"

using std::shared_ptr;

class SplitCUFlagReaderContext : public ISplitCUFlagReaderContext
{
public:
    SplitCUFlagReaderContext(const Coordinate& point,
                             ICodingQuadtreeContext* context, 
                             CodingQuadtree* coding_quadtree)
        : point_(point)
        , coding_quadtree_context_(context)
        , coding_quadtree_(coding_quadtree)
    {

    }

    virtual ~SplitCUFlagReaderContext()
    {

    }

private:
    virtual bool IsLeftBlockAvailable() const override
    {
        Coordinate left_neighbour = {point_.x - 1, point_.y};
        return coding_quadtree_context_->IsNeighbourBlockAvailable(
            point_, left_neighbour);
    }

    virtual bool IsUpBlockAvailable() const override
    {
        Coordinate up_neighbour = {point_.x, point_.y - 1};
        return coding_quadtree_context_->IsNeighbourBlockAvailable(
            point_, up_neighbour);
    }

    virtual uint32_t GetCurrentBlockLayer() const override
    {
        return coding_quadtree_->GetCurrentLayer();
    }

    virtual uint32_t GetLeftBlockLayer() const override
    {
        Coordinate left_neighbour = {point_.x - 1, point_.y};
        return coding_quadtree_context_->GetNearestCULayerByCoordinate(
            left_neighbour);
    }

    virtual uint32_t GetUpBlockLayer() const override
    {
        Coordinate up_neighbour = {point_.x, point_.y - 1};
        return coding_quadtree_context_->GetNearestCULayerByCoordinate(
            up_neighbour);
    }

    Coordinate point_;
    ICodingQuadtreeContext* coding_quadtree_context_;
    CodingQuadtree* coding_quadtree_;
};

// 由于cu_qp_delta_val是从cu里一层层传递上来的，因此，可以做一个假设，假设cu层的log2CBSize
// 是小于Log2MinCuQpDeltaSize的，则传递上去的过程中，随着log2CBSize的增大，终究会有一个
// log2CBSize是大于Log2MinCuQpDeltaSize，那么第一个log2CBSize大于Log2MinCuQpDeltaSize
// 的节点就是接收cu_qp_delta_val的，并且把IsCuQpDeltaCoded的状态改变,也就是说，当
// log2CBSize > Log2MinCuQpDeltaSize，则自己接收消息，如果不是，则通知上层节点
class CodingQuadtreeContextInMyself : public ICodingQuadtreeContext
{
public:
    CodingQuadtreeContextInMyself(ICodingQuadtreeContext* parent_node_context,
                                  CodingQuadtree* coding_quadtree_node)
        : parent_node_context_(parent_node_context)
        , coding_quadtree_node_(coding_quadtree_node)
        , is_cu_qp_delta_coded_(false)
        , is_cu_chroma_qp_offset_coded_(false)
    {

    }

    virtual ~CodingQuadtreeContextInMyself()
    {

    }

    virtual uint32_t GetFrameWidthInLumaSamples() const override
    {
        return parent_node_context_->GetFrameWidthInLumaSamples();
    }

    virtual uint32_t GetFrameHeightInLumaSamples() const override
    {
        return parent_node_context_->GetFrameHeightInLumaSamples();
    }

    virtual uint32_t GetMinCBLog2SizeY() const override
    {
        return parent_node_context_->GetMinCBLog2SizeY();
    }

    virtual uint32_t GetMinCBSizeY() const override
    {
        return parent_node_context_->GetMinCBSizeY();
    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return parent_node_context_->IsCUQPDeltaEnabled();
    }

    virtual uint32_t GetLog2MinCUQPDeltaSize() const override
    {
        return parent_node_context_->GetLog2MinCUQPDeltaSize();
    }

    virtual bool IsCUChromaQPOffsetEnabled() const override
    {
        return parent_node_context_->IsCUChromaQPOffsetEnabled();
    }

    virtual uint32_t GetLog2MinCUChromaQPOffsetSize() const override
    {
        return parent_node_context_->GetLog2MinCUChromaQPOffsetSize();
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return parent_node_context_->GetCABACInitType();
    }

    virtual bool IsTransquantBypassEnabled() const override
    {
        return parent_node_context_->IsTransquantBypassEnabled();
    }

    virtual SliceType GetSliceType() const override
    {
        return parent_node_context_->GetSliceType();
    }

    virtual bool IsPaletteModeEnabled() const override
    {
        return parent_node_context_->IsPaletteModeEnabled();
    }

    virtual uint32_t GetMaxTransformBlockSizeY() const override
    {
        return parent_node_context_->GetMaxTransformBlockSizeY();
    }

    virtual uint32_t GetMinTransformBlockSizeY() const override
    {
        return parent_node_context_->GetMinTransformBlockSizeY();
    }

    virtual uint32_t GetMinPCMCodingBlockSizeY() const override
    {
        return parent_node_context_->GetMinPCMCodingBlockSizeY();
    }

    virtual uint32_t GetMaxPCMCodingBlockSizeY() const override
    {
        return parent_node_context_->GetMaxPCMCodingBlockSizeY();
    }

    virtual bool IsAsymmetricMotionPartitionsEnabled() const
    {
        return parent_node_context_->IsAsymmetricMotionPartitionsEnabled();
    }

    virtual bool IsPCMEnabled() const override
    {
        return parent_node_context_->IsPCMEnabled();
    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return parent_node_context_->GetChromaFormatType();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthIntra() const override
    {
        return parent_node_context_->GetMaxTransformHierarchyDepthIntra();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthInter() const override
    {
        return parent_node_context_->GetMaxTransformHierarchyDepthInter();
    }

    virtual bool IsNeighbourBlockAvailable(
        const Coordinate& current, const Coordinate& neighbour) const
    {
        return parent_node_context_->IsNeighbourBlockAvailable(current, 
                                                               neighbour);
    }

    virtual uint32_t GetNearestCULayerByCoordinate(const Coordinate& point)
        const override
    {
        return parent_node_context_->GetNearestCULayerByCoordinate(point);
    }

    virtual std::shared_ptr<PaletteTable> GetPredictorPaletteTable()
        const override
    {
        return parent_node_context_->GetPredictorPaletteTable();
    }

    virtual uint32_t GetPaletteMaxSize() const override
    {
        return parent_node_context_->GetPaletteMaxSize();
    }

    virtual uint32_t GetBitDepthLuma() const override
    {
        return parent_node_context_->GetBitDepthLuma();
    }

    virtual uint32_t GetBitDepthChroma() const override
    {
        return parent_node_context_->GetBitDepthChroma();
    }

    virtual uint32_t GetPredictorPaletteMaxSize() const override
    {
        return parent_node_context_->GetPredictorPaletteMaxSize();
    }

    virtual bool IsCUQPDeltaCoded() const override
    {
        uint32_t log2_cb_size = coding_quadtree_node_->GetLog2SizeY();
        uint32_t log2_min_cu_qp_delta_size = 
            parent_node_context_->GetLog2MinCUQPDeltaSize();
        return log2_cb_size > log2_min_cu_qp_delta_size ? is_cu_qp_delta_coded_ :
            parent_node_context_->IsCUQPDeltaCoded();
    }

    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val)
    {
        assert(!is_cu_qp_delta_coded_);
        uint32_t log2_cb_size = coding_quadtree_node_->GetLog2SizeY();
        uint32_t log2_min_cu_qp_delta_size =
            parent_node_context_->GetLog2MinCUQPDeltaSize();

        if (log2_cb_size > log2_min_cu_qp_delta_size)
        {
            is_cu_qp_delta_coded_ = true;
            coding_quadtree_node_->SetCUQPDeltaVal(cu_qp_delta_val);
        }
        else
        {
            parent_node_context_->SetCUQPDeltaVal(cu_qp_delta_val);
        }
    }

    virtual bool IsCUChromaQPOffsetCoded() const override
    {
        uint32_t log2_cb_size = coding_quadtree_node_->GetLog2SizeY();
        uint32_t log2_min_cu_chroma_qp_delta_size =
            parent_node_context_->GetLog2MinCUChromaQPOffsetSize();

        return log2_cb_size > log2_min_cu_chroma_qp_delta_size ? 
            is_cu_chroma_qp_offset_coded_ : 
            parent_node_context_->IsCUChromaQPOffsetCoded();
    }

    virtual void SetCUChromaQPOffsetCrAndCb(
        int32_t cu_chroma_qp_offset_cr, int32_t cu_chroma_qp_offset_cb) override
    {
        assert(!is_cu_chroma_qp_offset_coded_);
        uint32_t log2_cb_size = coding_quadtree_node_->GetLog2SizeY();
        uint32_t log2_min_cu_chroma_qp_delta_size =
            parent_node_context_->GetLog2MinCUChromaQPOffsetSize();

        if (log2_cb_size > log2_min_cu_chroma_qp_delta_size)
        {
            is_cu_chroma_qp_offset_coded_ = true;
            coding_quadtree_node_->SetCUChromaQPOffsetCr(cu_chroma_qp_offset_cr);
            coding_quadtree_node_->SetCUChromaQPOffsetCb(cu_chroma_qp_offset_cb);
        }
        else
        {
            parent_node_context_->SetCUChromaQPOffsetCrAndCb(
                cu_chroma_qp_offset_cr, cu_chroma_qp_offset_cb);
        }
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return parent_node_context_->GetChromaQPOffsetListtLen();
    }

    virtual const std::vector<int32_t>& GetCbQPOffsetList() const override
    {
        return parent_node_context_->GetCbQPOffsetList();
    }

    virtual const std::vector<int32_t>& GetCrQPOffsetList() const override
    {
        return parent_node_context_->GetCrQPOffsetList();
    }

private:
    ICodingQuadtreeContext* parent_node_context_;
    CodingQuadtree* coding_quadtree_node_;
    bool is_cu_qp_delta_coded_;
    bool is_cu_chroma_qp_offset_coded_;

};

class CodingUnitContext : public ICodingUnitContext
{
public:
    CodingUnitContext(ICodingQuadtreeContext* quadtree_context)
        : quadtree_context_(quadtree_context)
    {

    }

    virtual ~CodingUnitContext()
    {

    }

    virtual bool IsTransquantBypassEnabled() const override
    {
        return quadtree_context_->IsTransquantBypassEnabled();
    }

    virtual CABACInitType GetCABACInitType() const override
    {
        return quadtree_context_->GetCABACInitType();
    }

    virtual SliceType GetSliceType() const override
    {
        return quadtree_context_->GetSliceType();
    }

    virtual bool IsPaletteModeEnabled() const override
    {
        return quadtree_context_->IsPaletteModeEnabled();
    }

    virtual uint32_t GetMaxTransformBlockSizeY() const override
    {
        return quadtree_context_->GetMaxTransformBlockSizeY();
    }

    virtual uint32_t GetMinTransformBlockSizeY() const override
    {
        return quadtree_context_->GetMinTransformBlockSizeY();
    }

    virtual uint32_t GetMinCBSizeY() const override
    {
        return quadtree_context_->GetMinCBSizeY();
    }

    virtual uint32_t GetMinPCMCodingBlockSizeY() const override
    {
        return quadtree_context_->GetMinPCMCodingBlockSizeY();
    }

    virtual uint32_t GetMaxPCMCodingBlockSizeY() const override
    {
        return quadtree_context_->GetMaxPCMCodingBlockSizeY();
    }

    virtual bool IsAsymmetricMotionPartitionsEnabled() const override
    {
        return quadtree_context_->IsAsymmetricMotionPartitionsEnabled();
    }

    virtual bool IsPCMEnabled() const override
    {
        return quadtree_context_->IsPCMEnabled();
    }

    virtual ChromaFormatType GetChromaFormatType() const override
    {
        return quadtree_context_->GetChromaFormatType();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthIntra() const override
    {
        return quadtree_context_->GetMaxTransformHierarchyDepthIntra();
    }

    virtual uint32_t GetMaxTransformHierarchyDepthInter() const override
    {
        return quadtree_context_->GetMaxTransformHierarchyDepthInter();
    }

    virtual bool IsNeighbourBlockAvailable(
        const Coordinate& current, const Coordinate& neighbour) const override
    {
        return quadtree_context_->IsNeighbourBlockAvailable(current, neighbour);
    }

    virtual uint32_t GetNearestCULayerByCoordinate(const Coordinate& point)
        const
    {
        return quadtree_context_->GetNearestCULayerByCoordinate(point);
    }

    virtual shared_ptr<PaletteTable> GetPredictorPaletteTable() const override
    {
        return quadtree_context_->GetPredictorPaletteTable();
    }

    virtual uint32_t GetPaletteMaxSize() const override
    {
        return quadtree_context_->GetPaletteMaxSize();
    }

    virtual uint32_t GetBitDepthLuma() const override
    {
        return quadtree_context_->GetBitDepthLuma();
    }

    virtual uint32_t GetBitDepthChroma() const override
    {
        return quadtree_context_->GetBitDepthChroma();
    }

    virtual uint32_t GetPredictorPaletteMaxSize() const override
    {
        return quadtree_context_->GetPredictorPaletteMaxSize();
    }

    virtual bool IsCUQPDeltaEnabled() const override
    {
        return quadtree_context_->IsCUQPDeltaEnabled();
    }

    virtual bool IsCUQPDeltaCoded() const override
    {
        return quadtree_context_->IsCUQPDeltaCoded();
    }

    virtual void SetCUQPDeltaVal(int32_t cu_qp_delta_val) override
    {
        quadtree_context_->SetCUQPDeltaVal(cu_qp_delta_val);
    }

    virtual bool IsCUChromaQPOffsetEnable() const override
    {
        return quadtree_context_->IsCUChromaQPOffsetEnabled();
    }

    virtual bool IsCUChromaQPOffsetCoded() const override
    {
        return quadtree_context_->IsCUChromaQPOffsetCoded();
    }

    virtual uint32_t GetChromaQPOffsetListtLen() const override
    {
        return quadtree_context_->GetChromaQPOffsetListtLen();
    }

    virtual void SetCUChromaQPOffsetIndex(uint32_t cu_chroma_qp_offset_index)
        override
    {
        if (cu_chroma_qp_offset_index >= GetChromaQPOffsetListtLen())
            return;

        quadtree_context_->SetCUChromaQPOffsetCrAndCb(
            quadtree_context_->GetCrQPOffsetList()[cu_chroma_qp_offset_index],
            quadtree_context_->GetCbQPOffsetList()[cu_chroma_qp_offset_index]);
    }

private:
    ICodingQuadtreeContext* quadtree_context_;
};

CodingQuadtree::CodingQuadtree(const Coordinate& point, uint32_t cb_log2_size_y, 
                               uint32_t layer)
    : point_(point)
    , cb_log2_size_y_(cb_log2_size_y)
    , cb_size_y_(1 << cb_log2_size_y_)
    , layer_(layer)
    , cu_qp_delta_val_(0)
    , cu_chroma_qp_offset_cb_(0)
    , cu_chroma_qp_offset_cr_(0)
    , sub_coding_quadtrees_()
{

}

CodingQuadtree::~CodingQuadtree()
{

}

bool CodingQuadtree::Parse(CABACReader* cabac_reader, 
                           ICodingQuadtreeContext* context)
{
    if (!cabac_reader || !context)
        return false;

    bool is_split_cu = cb_log2_size_y_ > context->GetMinCBLog2SizeY();
    if (((point_.x + cb_size_y_) <= context->GetFrameWidthInLumaSamples()) &&
        ((point_.y + cb_size_y_) <= context->GetFrameHeightInLumaSamples()) &&
        (cb_log2_size_y_ > context->GetMinCBLog2SizeY()))
    {
        SplitCUFlagReaderContext split_cu_flag_reader_context(point_, context, 
                                                              this);
        SplitCUFlagReader reader(cabac_reader, context->GetCABACInitType(),
                                 &split_cu_flag_reader_context);
        is_split_cu = reader.Read();
    }

    if (is_split_cu)
    {
        uint32_t x = point_.x + (cb_size_y_ >> 1);
        uint32_t y = point_.y + (cb_size_y_ >> 1);
        uint32_t sub_cb_log2_size_y = cb_log2_size_y_ - 1;
        uint32_t sub_layer = layer_ + 1;
        sub_coding_quadtrees_[0].reset(
            new CodingQuadtree(point_, sub_cb_log2_size_y, sub_layer));
        if (x < context->GetFrameWidthInLumaSamples())
        {
            sub_coding_quadtrees_[1].reset(
                new CodingQuadtree({x, point_.y}, sub_cb_log2_size_y,
                                    sub_layer));
        }
        if (y < context->GetFrameHeightInLumaSamples())
        {
            sub_coding_quadtrees_[2].reset(
                new CodingQuadtree({point_.x, y}, sub_cb_log2_size_y,
                                   sub_layer));
        }
        if ((x < context->GetFrameWidthInLumaSamples()) &&
            (y < context->GetFrameHeightInLumaSamples()))
        {
            sub_coding_quadtrees_[3].reset(
                new CodingQuadtree({x, y}, sub_cb_log2_size_y, sub_layer));
        }
        for (uint32_t i = 0; i < 4; ++i)
        {
            if (sub_coding_quadtrees_[i])
            {
                CodingQuadtreeContextInMyself coding_quadtree_context(
                    context, sub_coding_quadtrees_[i].get());
                bool success = sub_coding_quadtrees_[i]->Parse(
                    cabac_reader, &coding_quadtree_context);
                if (!success)
                    return false;
            }
        }
    }
    else
    {
        CodingUnit cu(point_, layer_, cb_size_y_);
        CodingUnitContext cu_context(context);
        if (!cu.Parse(cabac_reader, &cu_context))
            return false;
    }
    return true;
}

uint32_t CodingQuadtree::GetNearestCULayerByCoordinate(const Coordinate& point)
    const
{
    if ((point.x < point_.x) || (point.x > point_.x + cb_size_y_) ||
        (point.y < point_.y) || (point.y > point_.y + cb_size_y_))
        return 0;

    Coordinate reference_point = {point.x - point_.x, point.y - point_.y};
    uint32_t sub_cb_size_y = cb_size_y_ >> 1;
    uint32_t index = ((reference_point.x & sub_cb_size_y) > 0 ? 1 : 0) | 
        (((reference_point.y & sub_cb_size_y) > 0 ? 1 : 0) << 1);

    if (!sub_coding_quadtrees_[index])
        return layer_;

    return sub_coding_quadtrees_[index]->GetNearestCULayerByCoordinate(point);
}

uint32_t CodingQuadtree::GetCurrentLayer() const
{
    return layer_;
}

uint32_t CodingQuadtree::GetCUQPDeltaVal() const
{
    return cu_qp_delta_val_;
}

uint32_t CodingQuadtree::GetLog2SizeY() const
{
    return cb_log2_size_y_;
}

void CodingQuadtree::SetCUQPDeltaVal(int32_t cu_qp_delta_val)
{
    cu_qp_delta_val_ = cu_qp_delta_val;
}

void CodingQuadtree::SetCUChromaQPOffsetCb(int32_t cu_chroma_qp_offset_cb)
{
    cu_chroma_qp_offset_cb_ = cu_chroma_qp_offset_cb;
}

void CodingQuadtree::SetCUChromaQPOffsetCr(int32_t cu_chroma_qp_offset_cr)
{
    cu_chroma_qp_offset_cr_ = cu_chroma_qp_offset_cr;
}

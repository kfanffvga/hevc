#include "hevc_decoder/syntax/coding_quadtree.h"

#include "hevc_decoder/syntax/coding_quadtree_context.h"
#include "hevc_decoder/vld_decoder/split_cu_flag_reader.h"

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
        Coordinate up_neighbour = {point_.x - 1, point_.y};
        return coding_quadtree_context_->GetNearestCULayerByCoordinate(
            up_neighbour);
    }

    Coordinate point_;
    ICodingQuadtreeContext* coding_quadtree_context_;
    CodingQuadtree* coding_quadtree_;
};

CodingQuadtree::CodingQuadtree(const Coordinate& point, uint32_t cb_log2_size_y, 
                               uint32_t layer)
    : point_(point)
    , cb_log2_size_y_(cb_log2_size_y)
    , cb_size_y_(1 << cb_log2_size_y_)
    , layer_(layer)
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
                bool success = 
                    sub_coding_quadtrees_[i]->Parse(cabac_reader, context);
                if (!success)
                    return false;
            }
        }
    }
    else
    {

    }
    return true;
}

uint32_t CodingQuadtree::GetNearestCULayerByCoordinate(const Coordinate& point)
{
    if ((point.x < point_.x) || (point.x > point_.x + cb_size_y_) ||
        (point.y < point_.y) || (point.y > point_.y + cb_size_y_))
        return 0;

    Coordinate reference_point = {point.x - point_.x, point.y - point_.y};
    uint32_t index = (reference_point.x & (cb_size_y_ >> 1)) || 
        ((reference_point.y & (cb_size_y_ >> 1)) << 1);

    if (!sub_coding_quadtrees_[index])
        return layer_;

    return sub_coding_quadtrees_[index]->GetNearestCULayerByCoordinate(point);
}

uint32_t CodingQuadtree::GetCurrentLayer()
{
    return layer_;
}
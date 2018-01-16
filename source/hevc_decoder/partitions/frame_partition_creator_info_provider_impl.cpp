#include "hevc_decoder/partitions/frame_partition_creator_info_provider_impl.h"

#include "hevc_decoder/base/math.h"

using std::vector;

FramePartitionCreatorInfoProviderImpl::FramePartitionCreatorInfoProviderImpl(
    uint32_t width, uint32_t height)
    : IFramePartitionCreatorInfoProvider()
    , width_(width)
    , height_(height)
    , tile_cols_width_in_ctb_()
    , tile_rows_height_in_ctb_()
    , ctb_log2_size_y_(0)
    , min_tb_log2_size_y_(0)
{

}

FramePartitionCreatorInfoProviderImpl::~FramePartitionCreatorInfoProviderImpl()
{

}

bool FramePartitionCreatorInfoProviderImpl::Init(
    const vector<uint32_t>& tile_cols_width_in_ctb, 
    const vector<uint32_t>& tile_rows_height_in_ctb, uint32_t ctb_log2_size_y, 
    uint32_t min_tb_log2_size_y)
{
    if (tile_rows_height_in_ctb.empty() || tile_cols_width_in_ctb.empty())
        return false;

    if ((0 == ctb_log2_size_y) || (0 == min_tb_log2_size_y))
        return false;

    tile_rows_height_in_ctb_ = tile_rows_height_in_ctb;
    tile_cols_width_in_ctb_ = tile_cols_width_in_ctb;
    ctb_log2_size_y_ = ctb_log2_size_y;
    min_tb_log2_size_y_ = min_tb_log2_size_y;

    uint32_t width_in_ctb = UpAlignRightShift(width_, ctb_log2_size_y);
    tile_cols_width_in_ctb_.resize(tile_cols_width_in_ctb.size());
    for (uint32_t i = 0; i < tile_cols_width_in_ctb_.size() - 1; ++i)
    {
        tile_cols_width_in_ctb_[i] = tile_cols_width_in_ctb[i];
        width_in_ctb -= tile_cols_width_in_ctb[i];
    }
    tile_cols_width_in_ctb_[tile_cols_width_in_ctb_.size() - 1] = width_in_ctb;

    uint32_t height_in_ctb = UpAlignRightShift(height_, ctb_log2_size_y);
    tile_rows_height_in_ctb_.resize(tile_rows_height_in_ctb.size());
    for (uint32_t i = 0; i < tile_rows_height_in_ctb_.size() - 1; ++i)
    {
        tile_rows_height_in_ctb_[i] = tile_rows_height_in_ctb[i];
        height_in_ctb -= tile_rows_height_in_ctb[i];
    }
    tile_rows_height_in_ctb_[tile_rows_height_in_ctb_.size() - 1] = height_in_ctb;
    return true;
}

bool FramePartitionCreatorInfoProviderImpl::Init(uint32_t num_tile_cols, 
                                                 uint32_t num_tile_rows, 
                                                 uint32_t ctb_log2_size_y, 
                                                 uint32_t min_tb_log2_size_y)
{
    if ((0 == num_tile_rows) || (0 == num_tile_cols))
        return false;

    if ((0 == ctb_log2_size_y) || (0 == min_tb_log2_size_y))
        return false;

    uint32_t width_in_ctb = UpAlignRightShift(width_, ctb_log2_size_y);
    uint32_t tile_width_in_ctb = UpAlignDiv(width_in_ctb, num_tile_cols);
    tile_cols_width_in_ctb_.resize(num_tile_cols);
    for (uint32_t i = 0; i < num_tile_cols - 1; ++i)
        tile_cols_width_in_ctb_[i] = tile_width_in_ctb;

    tile_cols_width_in_ctb_[num_tile_cols - 1] = 
        width_in_ctb - tile_width_in_ctb * (num_tile_cols - 1);

    uint32_t height_in_ctb = UpAlignRightShift(height_, ctb_log2_size_y);
    uint32_t tile_height_in_ctb = UpAlignDiv(height_in_ctb, num_tile_rows);
    tile_rows_height_in_ctb_.resize(num_tile_rows);
    for (uint32_t i = 0; i < num_tile_rows - 1; ++i)
        tile_rows_height_in_ctb_[i] = tile_height_in_ctb;

    tile_rows_height_in_ctb_[num_tile_rows - 1] = 
        height_in_ctb - tile_height_in_ctb * (num_tile_rows - 1);
        
    ctb_log2_size_y_ = ctb_log2_size_y;
    min_tb_log2_size_y_ = min_tb_log2_size_y;
    return true;
}

uint32_t FramePartitionCreatorInfoProviderImpl::GetWidth() const
{
    return width_;
}

uint32_t FramePartitionCreatorInfoProviderImpl::GetHeight() const
{
    return height_;
}

const vector<uint32_t>& 
    FramePartitionCreatorInfoProviderImpl::GetTileColsWidthInCTB() const
{
    return tile_cols_width_in_ctb_;
}

const vector<uint32_t>& 
    FramePartitionCreatorInfoProviderImpl::GetTileRowsHeightInCTB() const
{
    return tile_rows_height_in_ctb_;
}

uint32_t FramePartitionCreatorInfoProviderImpl::GetCTBLog2SizeY() const
{
    return ctb_log2_size_y_;
}

uint32_t FramePartitionCreatorInfoProviderImpl::GetMinTBLog2SizeY() const
{
    return min_tb_log2_size_y_;
}

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

    uint32_t tile_width_in_ctb = 
        UpAlignDiv(UpAlignRightShift(width_, ctb_log2_size_y), num_tile_cols);
    tile_cols_width_in_ctb_.resize(num_tile_cols);
    for (auto& i : tile_cols_width_in_ctb_)
        i = tile_width_in_ctb;

    uint32_t tile_height_in_ctb = 
        UpAlignDiv(UpAlignRightShift(height_, ctb_log2_size_y), num_tile_rows);
    tile_rows_height_in_ctb_.resize(num_tile_rows);
    for (auto& i : tile_rows_height_in_ctb_)
        i = tile_height_in_ctb;

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

#include "hevc_decoder/base/frame_partition.h"

#include "hevc_decoder/base/math.h"
#include "hevc_decoder/base/frame_syntax_context.h"

using std::vector;

FramePartition::FramePartition(uint32_t frame_width, uint32_t frame_height)
    : frame_height_(frame_height)
    , frame_width_(frame_width)
    , ctb_log2_size_y_(0)
    , min_tb_log2_size_y_(0)
    , initialzed_(false)
    , tile_and_raster_partition_info_()
    , transform_block_partition_info_()
{

}

FramePartition::~FramePartition()
{

}

bool FramePartition::InitOnUniformSpacing(uint32_t num_tile_cols, 
                                          uint32_t num_tile_rows, 
                                          uint32_t ctb_log2_size_y, 
                                          uint32_t min_tb_log2_size_y)
{
    if ((0 == num_tile_rows) || (0 == num_tile_cols))
        return false;

    uint32_t tile_width_in_ctb = (frame_width_ >> ctb_log2_size_y) / num_tile_cols;
    vector<uint32_t> tile_cols_width_in_ctb;
    tile_cols_width_in_ctb.resize(num_tile_cols);
    for (auto& width : tile_cols_width_in_ctb)
        width = tile_width_in_ctb;

    uint32_t tile_height_in_ctb = 
        (frame_height_ >> ctb_log2_size_y) / num_tile_rows;
    vector<uint32_t> tile_rows_height_in_ctb;
    tile_rows_height_in_ctb.resize(num_tile_rows);
    for (auto& height : tile_rows_height_in_ctb)
        height = tile_height_in_ctb;

    return Init(tile_cols_width_in_ctb, tile_rows_height_in_ctb, 
                ctb_log2_size_y, min_tb_log2_size_y);
}

 bool FramePartition::Init(const vector<uint32_t>& tile_cols_width_in_ctb, 
                           const vector<uint32_t>& tile_rows_height_in_ctb, 
                           uint32_t ctb_log2_size_y, uint32_t min_tb_log2_size_y)
{
    if (tile_cols_width_in_ctb.empty() || tile_rows_height_in_ctb.empty())
        return false;

    if (!ctb_log2_size_y || !min_tb_log2_size_y) 
        return false;

    vector<uint32_t> absolute_tile_width;
    uint32_t base = 0;
    for (const auto& i : tile_cols_width_in_ctb)
    {
        base += i << ctb_log2_size_y;
        absolute_tile_width.push_back(base);
    }
    
    base = 0;
    vector<uint32_t> absolute_tile_height;
    for (const auto& i : tile_rows_height_in_ctb)
    {
        base += i << ctb_log2_size_y;
        absolute_tile_height.push_back(base);
    }

    uint32_t tile_index = 0, tile_begin_x = 0, tile_begin_y = 0;
    uint32_t ctbs_per_row = 
        UpAlign(frame_width_, ctb_log2_size_y) >> ctb_log2_size_y;
    uint32_t tile_scan_index = 0;
    for (const auto& tile_end_y : absolute_tile_height)
    {
        for (const auto& tile_end_x : absolute_tile_width)
        {
            uint32_t end_x = tile_end_x == frame_width_ ? 
                UpAlign(tile_end_x, ctb_log2_size_y) : tile_end_x;

            uint32_t end_y = tile_end_y == frame_height_ ?
                UpAlign(tile_end_y, ctb_log2_size_y) : tile_end_y;

            InitSingleTilePartition(tile_begin_x, tile_begin_y, end_x, end_y, 
                                    tile_index, ctb_log2_size_y, ctbs_per_row,
                                    &tile_scan_index);

            tile_begin_x = tile_end_x;
            tile_begin_y = tile_end_y;
            ++tile_index;
        }
    }

    InitTransformBlockPartition(min_tb_log2_size_y, ctb_log2_size_y);
    min_tb_log2_size_y_ = min_tb_log2_size_y;
    ctb_log2_size_y_ = ctb_log2_size_y;
    initialzed_ = true;
    return true;
}

 void FramePartition::InitSingleTilePartition(uint32_t tile_begin_x, 
                                              uint32_t tile_begin_y, 
                                              uint32_t tile_end_x, 
                                              uint32_t tile_end_y, 
                                              uint32_t tile_index, 
                                              uint32_t ctb_log2_size_y,
                                              uint32_t ctbs_per_row,
                                              uint32_t* tile_scan_index)
 {
     bool is_first_block_in_tile = true;
     uint32_t block_tile_scan_index = 0;
     uint32_t ctb_size_y = 1 << ctb_log2_size_y;
     for (uint32_t y = tile_begin_y; y < tile_end_y; y += ctb_size_y)
     {
         uint32_t row_begin_index = (y >> ctb_log2_size_y) * ctbs_per_row;
         for (uint32_t x = tile_begin_x; x < tile_end_x; x += ctb_size_y)
         {
             uint32_t raster_scan_index = (x >> ctb_log2_size_y) + row_begin_index;
             
             CodedTreeBlockPositionInfo info = 
             {
                 {x, y}, raster_scan_index, *tile_scan_index, tile_index,
                 is_first_block_in_tile
             };
             is_first_block_in_tile = false;
             tile_and_raster_partition_info_.insert(info);
             ++*tile_scan_index;
         }
     }
 }

 void FramePartition::InitTransformBlockPartition(uint32_t ctb_log2_size_y, 
                                                  uint32_t min_tb_log2_size_y)
 {
     // Fix Me 对齐问题,此处没考虑
     if (ctb_log2_size_y < min_tb_log2_size_y)
         return;

     auto& tile_scan_values = 
         tile_and_raster_partition_info_.get<FramePartition::TileScanIndex>();
     uint32_t log2_min_tbs_per_ctb = ctb_log2_size_y - min_tb_log2_size_y;
     uint32_t ctb_width_in_min_tbs = 1 << log2_min_tbs_per_ctb;
     for (const auto& i : tile_scan_values)
     {
         uint32_t tb_index = i.block_tile_scan_index << log2_min_tbs_per_ctb;
         for (uint32_t x = 0; x < ctb_width_in_min_tbs; ++x)
         {
             for (uint32_t y = 0; y < ctb_width_in_min_tbs; ++y)
             {
                 uint32_t tb_index_of_current_ctb = 0;
                 for (uint32_t j = 0; j < log2_min_tbs_per_ctb; ++j)
                 {
                     uint32_t v = 1 << j;
                     if ((v > x) && (v > y))
                         break;

                     tb_index_of_current_ctb |= 
                         ((((y & v) << 1) | (x & v)) << j);
                 }
                 tb_index += tb_index_of_current_ctb;

                 TransformBlockPositionInfo info = 
                 {
                     {x << min_tb_log2_size_y, y << min_tb_log2_size_y},
                     tb_index, i.block_raster_scan_index, 
                     i.block_tile_scan_index, i.tile_index
                 };
                 transform_block_partition_info_.insert(info);
             }
         }
     }
 }

 uint32_t FramePartition::RasterScanToTileScan(uint32_t index)
 {
    auto& raster_scan_values = 
        tile_and_raster_partition_info_.get<FramePartition::RasterScanIndex>();
     
    auto begin = raster_scan_values.find(index);
    if (begin != raster_scan_values.end())
        return begin->block_tile_scan_index;

    assert(false && "不应该出现raster scan 与 tile scan 不对应的情况");
    return 0;
 }

 bool FramePartition::IsZScanOrderNeighbouringBlockAvailable(
     const Coordinate& current_block, const Coordinate& neighbouring_block,
     const IFrameSyntaxContext* frame_context)
 {
    if (!initialzed_ || !frame_context)
        return false;

    if ((current_block.x > frame_width_) || (current_block.y > frame_height_) ||
        (neighbouring_block.x > frame_width_) || 
        (neighbouring_block.y > frame_height_))
        return false;

    Coordinate c = { current_block.x >> min_tb_log2_size_y_, 
                    current_block.y >> min_tb_log2_size_y_ };

    auto& coordinate_values = 
        transform_block_partition_info_.get<CoordinateKey>();

    auto tb_of_current_block = coordinate_values.find(c);
    if (coordinate_values.end() == tb_of_current_block)
        return false;

    c.x = neighbouring_block.x >> min_tb_log2_size_y_;
    c.y = neighbouring_block.y >> min_tb_log2_size_y_;

    auto tb_of_neighbouring_block = coordinate_values.find(c);
    if (coordinate_values.end() == tb_of_neighbouring_block)
        return false;

    if (tb_of_neighbouring_block->index > tb_of_current_block->index)
        return false;

    // 两个块必须在同一个tile里
    if (tb_of_current_block->belong_to_tile_index != 
        tb_of_neighbouring_block->belong_to_tile_index)
        return false;

    uint32_t slice_address_of_current_block = 
        frame_context->GetSliceAddressByRasterScanBlockIndex(
            tb_of_current_block->belong_to_raster_scan_index);

    uint32_t slice_address_of_neighbouring_block =
        frame_context->GetSliceAddressByRasterScanBlockIndex(
            tb_of_neighbouring_block->belong_to_raster_scan_index);
    
    // 两个块必须在同一个slice segment里
    return slice_address_of_current_block == slice_address_of_neighbouring_block;
 }

 uint32_t FramePartition::GetTileIndex(const Coordinate& block)
 {
     auto& coordinate_values = 
         tile_and_raster_partition_info_.get<CoordinateKey>();

     auto block_info = coordinate_values.find(block);
     if (coordinate_values.end() == block_info)
         return static_cast<uint32_t>(-1);

     return block_info->tile_index;
 }

 bool FramePartition::IsTheFirstCTBInTile(const Coordinate& block)
 {
     auto& coordinate_values = 
         tile_and_raster_partition_info_.get<CoordinateKey>();
     auto block_info = coordinate_values.find(block);
     if (coordinate_values.end() == block_info)
         return false;

     return block_info->is_first_block_in_tile;
 }


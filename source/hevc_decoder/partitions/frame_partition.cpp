#include "hevc_decoder/partitions/frame_partition.h"

#include "hevc_decoder/base/math.h"
#include "hevc_decoder/partitions/slice_segment_address_provider.h"
#include "hevc_decoder/partitions/frame_partition_creator_info_provider.h"

using std::vector;
using std::move;
using std::shared_ptr;

FramePartition::FramePartition()
    : provider_()
    , tile_and_raster_partition_info_()
    , transform_block_partition_info_()
{

}

FramePartition::~FramePartition()
{

}

uint32_t FramePartition::GetWidth()
{
    return provider_->GetWidth();
}

uint32_t FramePartition::GetHeight()
{
    return provider_->GetHeight();
}

uint32_t FramePartition::GetMinTBLog2SizeY()
{
    return provider_->GetMinTBLog2SizeY();
}

const IFramePartitionCreatorInfoProvider*
    FramePartition::GetCreationInfoProvider()
{
    return provider_.get();
}

bool FramePartition::Init(
     const shared_ptr<IFramePartitionCreatorInfoProvider>& provider)
{
    if (!provider)
        return false;

    if (provider->GetTileColsWidthInCTB().empty() || 
        provider->GetTileRowsHeightInCTB().empty())
        return false;

    if (!provider->GetCTBLog2SizeY() || !provider->GetMinTBLog2SizeY()) 
        return false;

    tile_and_raster_partition_info_.clear();
    transform_block_partition_info_.clear();
    uint32_t ctb_log2_size_y = provider->GetCTBLog2SizeY();
    vector<uint32_t> absolute_tile_width;
    uint32_t base = 0;
    for (const auto& i : provider->GetTileColsWidthInCTB())
    {
        base += i << ctb_log2_size_y;
        absolute_tile_width.push_back(base);
    }
    
    base = 0;
    vector<uint32_t> absolute_tile_height;
    for (const auto& i : provider->GetTileRowsHeightInCTB())
    {
        base += i << ctb_log2_size_y;
        absolute_tile_height.push_back(base);
    }

    uint32_t tile_index = 0, tile_begin_x = 0, tile_begin_y = 0;
    uint32_t ctbs_per_row = 
        UpAlign(provider->GetWidth(), ctb_log2_size_y) >> ctb_log2_size_y;
    uint32_t tile_scan_index = 0;
    for (const auto& tile_end_y : absolute_tile_height)
    {
        for (const auto& tile_end_x : absolute_tile_width)
        {
            InitSingleTilePartition(tile_begin_x, tile_begin_y, tile_end_x, 
                                    tile_end_y, tile_index, ctb_log2_size_y, 
                                    ctbs_per_row, &tile_scan_index);

            tile_begin_x = tile_end_x;
            tile_begin_y = tile_end_y;
            ++tile_index;
        }
    }

    bool success = InitTransformBlockPartition(ctb_log2_size_y, 
                                               provider->GetMinTBLog2SizeY());
    if (!success)
        return false;

    provider_ = provider;
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
         bool is_first_block_in_row_of_tile = true;
         for (uint32_t x = tile_begin_x; x < tile_end_x; x += ctb_size_y)
         {
             uint32_t raster_scan_index = (x >> ctb_log2_size_y) + row_begin_index;
             
             CodedTreeBlockPositionInfo info = 
             {
                 {x, y}, raster_scan_index, *tile_scan_index, tile_index,
                 is_first_block_in_tile, is_first_block_in_row_of_tile
             };
             is_first_block_in_tile = false;
             is_first_block_in_row_of_tile = false;
             tile_and_raster_partition_info_.insert(info);
             ++*tile_scan_index;
         }
     }
 }

 bool FramePartition::InitTransformBlockPartition(uint32_t ctb_log2_size_y, 
                                                  uint32_t min_tb_log2_size_y)
 {
     // Fix Me 对齐问题,此处没考虑
     if (ctb_log2_size_y < min_tb_log2_size_y)
         return false;

     auto& raster_scan_values = 
         tile_and_raster_partition_info_.get<FramePartition::RasterScanIndex>();
     uint32_t log2_min_tbs_per_ctb = ctb_log2_size_y - min_tb_log2_size_y;
     uint32_t ctb_width_in_min_tbs = 1 << log2_min_tbs_per_ctb;
     uint32_t tbs_per_ctb = 1 << (log2_min_tbs_per_ctb << 1);
     for (const auto& ctb : raster_scan_values)
     {
         uint32_t base_tb_index = ctb.block_raster_scan_index * tbs_per_ctb; 
         for (uint32_t x = 0; x < ctb_width_in_min_tbs; ++x)
         {
             for (uint32_t y = 0; y < ctb_width_in_min_tbs; ++y)
             {
                 uint32_t tb_index_of_current_ctb = 0;
                 // 位交错算法
                 for (uint32_t j = 0; j < log2_min_tbs_per_ctb; ++j)
                 {
                     uint32_t v = 1 << j;
                     if ((v > x) && (v > y))
                         break;

                     tb_index_of_current_ctb |= 
                         ((((y & v) << 1) | (x & v)) << j);
                 }
                 tb_index_of_current_ctb += base_tb_index;

                 TransformBlockPositionInfo info = 
                 {
                     {x << min_tb_log2_size_y, y << min_tb_log2_size_y},
                     tb_index_of_current_ctb, ctb.block_raster_scan_index, 
                     ctb.block_tile_scan_index, ctb.tile_index
                 };
                 transform_block_partition_info_.insert(info);
             }
         }
     }
     return true;
 }

 bool FramePartition::RasterScanIndexToTileScanIndex(uint32_t raster_scan_index, 
                                                     uint32_t* tile_scan_index)
 {
    if (!tile_scan_index)
        return false;

    auto& raster_scan_values = 
        tile_and_raster_partition_info_.get<FramePartition::RasterScanIndex>();
     
    auto begin = raster_scan_values.find(raster_scan_index);
    if (begin != raster_scan_values.end())
    {
        *tile_scan_index = begin->block_tile_scan_index;
        return true;
    }
    assert(false && "不应该出现raster scan 与 tile scan 不对应的情况");
    return false;
 }

 bool FramePartition::GetTileIndexByCTBCoordinate(const Coordinate& block, uint32_t* tile_index)
 {
     if (!tile_index)
         return false;

     auto& coordinate_values = 
         tile_and_raster_partition_info_.get<CoordinateKey>();

     auto block_info = coordinate_values.find(block);
     if (block_info != coordinate_values.end())
     {
         *tile_index = block_info->tile_index;
         return true;
     }
     return false;
 }

 bool FramePartition::GetTileScanIndexByCTBCoordinate(const Coordinate& block, 
                                       uint32_t* tile_scan_index)
 {
     if (!tile_scan_index)
         return false;

     auto& coordinate_values =
         tile_and_raster_partition_info_.get<CoordinateKey>();
     auto block_info = coordinate_values.find(block);
     if (block_info != coordinate_values.end())
     {
         *tile_scan_index = block_info->block_tile_scan_index;
         return true;
     }
     return false;
 }

 bool FramePartition::GetRasterScanIndexByCTBCoordinate(
     const Coordinate& block, uint32_t* raster_scan_index)
 {
     if (!raster_scan_index)
         return false;

     auto& coordinate_values =
         tile_and_raster_partition_info_.get<CoordinateKey>();
     auto block_info = coordinate_values.find(block);
     if (block_info != coordinate_values.end())
     {
         *raster_scan_index = block_info->block_raster_scan_index;
         return true;
     }
     return false;
 }

 bool FramePartition::GetTileIndexByTransformBlockCoordinate(
     const Coordinate& block, uint32_t* tile_index)
 {
     if (!tile_index)
         return false;

     auto& coordinate_values = 
         transform_block_partition_info_.get<CoordinateKey>();
     auto block_info = coordinate_values.find(block);
     if (block_info != coordinate_values.end())
     {
         *tile_index = block_info->belong_to_tile_index;
         return true;
     }
     return false;
 }

 bool FramePartition::GetTileScanIndexByTransformBlockCoordinate(
     const Coordinate& block, uint32_t* tile_scan_index)
 {
     if (!tile_scan_index)
         return false;

     auto& coordinate_values =
         transform_block_partition_info_.get<CoordinateKey>();
     auto block_info = coordinate_values.find(block);
     if (block_info != coordinate_values.end())
     {
         *tile_scan_index = block_info->belong_to_tile_scan_index;
         return true;
     }
     return false;
 }

 bool FramePartition::GetZScanIndexByByTransformBlockCoordinate(
     const Coordinate& block, uint32_t* zscan_index)
 {
     if (!zscan_index)
         return false;

     auto& coordinate_values =
         transform_block_partition_info_.get<CoordinateKey>();
     auto block_info = coordinate_values.find(block);
     if (block_info != coordinate_values.end())
     {
         *zscan_index = block_info->index;
         return true;
     }
     return false;
 }

 bool FramePartition::GetRasterScanIndexByTransformBlockCoordinate(
     const Coordinate& block, uint32_t* raster_scan_index)
 {
     if (!raster_scan_index)
         return false;

     auto& coordinate_values =
         transform_block_partition_info_.get<CoordinateKey>();
     auto block_info = coordinate_values.find(block);
     if (block_info != coordinate_values.end())
     {
         *raster_scan_index = block_info->belong_to_raster_scan_index;
         return true;
     }
     return false;
 }

 bool FramePartition::GetIndexInfoByTransformBlockCoordinate(
     const Coordinate& block, TransformBlockIndexInfo* info)
 {
     if (!info)
         return false;

     auto& coordinate_values =
         transform_block_partition_info_.get<CoordinateKey>();
     auto block_info = coordinate_values.find(block);
     if (block_info != coordinate_values.end())
     {
         info->raster_scan_index = block_info->belong_to_raster_scan_index;
         info->tile_index = block_info->belong_to_tile_index;
         info->tile_scan_index = block_info->belong_to_tile_scan_index;
         info->zscan_index = block_info->index;
         return true;
     }
     return false;
 }

 bool FramePartition::GetCoordinateByTileScanIndex(uint32_t index, Coordinate* c)
 {
     if (!c)
         return false;

     auto& tile_scan_index_values = 
         tile_and_raster_partition_info_.get<TileScanIndex>();
     auto block_info = tile_scan_index_values.find(index);
     if (tile_scan_index_values.end() == block_info)
         return false;

     *c = block_info->block_coordinate;
     return true;
 }

 bool FramePartition::GetCoordinateByRasterScanIndex(uint32_t index, 
                                                     Coordinate* c)
 {
     if (!c)
         return false;

     auto& raster_scan_index_values =
         tile_and_raster_partition_info_.get<RasterScanIndex>();
     auto block_info = raster_scan_index_values.find(index);
     if (raster_scan_index_values.end() == block_info)
         return false;

     *c = block_info->block_coordinate;
     return true;
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

 bool FramePartition::IsTheFirstCTBInTileByRasterScanIndex(uint32_t index)
 {
     auto& raster_scan_indices = 
         tile_and_raster_partition_info_.get<RasterScanIndex>();
     auto block_info = raster_scan_indices.find(index);
     if (raster_scan_indices.end() == block_info)
         return false;

     return block_info->is_first_block_in_tile;
 }

 bool FramePartition::IsTheFirstCTBInTileByTileScanIndex(uint32_t index)
 {
     auto& tile_scan_indices =
         tile_and_raster_partition_info_.get<TileScanIndex>();
     auto block_info = tile_scan_indices.find(index);
     if (tile_scan_indices.end() == block_info)
         return false;

     return block_info->is_first_block_in_tile;
 }

 bool FramePartition::IsTheFirstCTBInRowOfFrameByRasterScanIndex(uint32_t index)
 {
     return !(index % UpAlignRightShift(provider_->GetWidth(), 
                                        provider_->GetCTBLog2SizeY()));
 }

 bool FramePartition::IsTheFirstCTBInRowOfFrameByTileScanIndex(uint32_t index)
 {
     auto& tile_scan_indices =
         tile_and_raster_partition_info_.get<TileScanIndex>();
     auto block_info = tile_scan_indices.find(index);
     if (tile_scan_indices.end() == block_info)
         return false;

     return IsTheFirstCTBInRowOfFrame(block_info->block_coordinate);
 }

 bool FramePartition::IsTheFirstCTBInRowOfFrame(const Coordinate& block)
 {
     return 0 == block.x;
 }

 bool FramePartition::IsTheFirstCTBInRowOfTileByRasterScanIndex(uint32_t index)
 {
    auto& raster_scan_indices =
         tile_and_raster_partition_info_.get<RasterScanIndex>();
    auto block_info = raster_scan_indices.find(index);
    if (raster_scan_indices.end() == block_info)
        return false;

    return block_info->is_first_block_in_row_of_tile;
 }

 bool FramePartition::IsTheFirstCTBInRowOfTileByTileScanIndex(uint32_t index)
 {
     auto& tile_scan_indices =
         tile_and_raster_partition_info_.get<TileScanIndex>();
     auto block_info = tile_scan_indices.find(index);
     if (tile_scan_indices.end() == block_info)
         return false;

     return block_info->is_first_block_in_row_of_tile;
 }

 bool FramePartition::IsTheFirstCTBInRowOfTile(const Coordinate& block)
 {
     auto& coordinate_values =
         tile_and_raster_partition_info_.get<CoordinateKey>();
     auto block_info = coordinate_values.find(block);
     if (coordinate_values.end() == block_info)
         return false;

     return block_info->is_first_block_in_row_of_tile;
 }
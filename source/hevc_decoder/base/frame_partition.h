#ifndef _FRAME_PARTITION_H_
#define _FRAME_PARTITION_H_

#include <vector>
#include <stdint.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "hevc_decoder/base/basic_types.h"

class FrameInfoProviderForFramePartition;

class FramePartition
{
public:
    FramePartition(uint32_t frame_width, uint32_t frame_height);
    ~FramePartition();

    bool InitOnUniformSpacing(uint32_t num_tile_cols, uint32_t num_tile_rows,
                              uint32_t ctb_log2_size_y, uint32_t min_tb_log2_size_y);

    bool Init(const std::vector<uint32_t>& tile_cols_width_in_ctb, 
              const std::vector<uint32_t>& tile_rows_height_in_ctb, 
              uint32_t ctb_log2_size_y, uint32_t min_tb_log2_size_y);

    uint32_t RasterScanToTileScan(uint32_t index);
    uint32_t GetTileIndex(const Coordinate& block);
    bool IsTheFirstCTBInTile(const Coordinate& block);

    // 6.4.1 判断邻居块对于当前块来说是否可用, 判断两个是否为同一个tile,同一个slice,
    // 并且当前块为邻居块的后面的可可用块
    bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, const Coordinate& neighbouring_block,
        const FrameInfoProviderForFramePartition* frame_info_provider);

private:
    struct CodedTreeBlockPositionInfo
    {
        Coordinate block_coordinate;
        uint32_t block_raster_scan_index;
        uint32_t block_tile_scan_index;
        uint32_t tile_index;
        bool is_first_block_in_tile;
    };

    struct TransformBlockPositionInfo
    {
        Coordinate block_coordinate;
        uint32_t index;
        uint32_t belong_to_raster_scan_index;
        uint32_t belong_to_tile_scan_index;
        uint32_t belong_to_tile_index;
    };

    struct CodedTreeBlockPositionInfoForCoordinate
    {
        typedef Coordinate result_type;
        const result_type operator()(const CodedTreeBlockPositionInfo& info) 
            const
        {
            return info.block_coordinate;
        }
    };

    struct CoordinateComparer
    {
        bool operator()(const Coordinate& a, const Coordinate& b) const
        {
           return b.y == a.y ? b.x < a.x : b.y < a.y;
        }
    };

    struct CoordinateKey;

    struct CodedTreeBlockPositionInfoForRasterScanIndex
    {
        typedef uint32_t result_type;
        const result_type operator()(const CodedTreeBlockPositionInfo& info) 
            const
        {
            return info.block_raster_scan_index;
        }
    };

    struct RasterScanIndex;

    struct CodedTreeBlockPositionInfoForTileScanIndex
    {
        typedef uint32_t result_type;
        const result_type operator()(const CodedTreeBlockPositionInfo& info) 
            const
        {
            return info.block_tile_scan_index;
        }
    };

    struct TileScanIndex;

    struct CodedTreeBlockPositionInfoForTileIndex
    {
        typedef uint32_t result_type;
        const result_type operator()(const CodedTreeBlockPositionInfo& info)
            const
        {
            return info.tile_index;
        }
    };

    struct TileIndex;

    struct uint32_tComparer
    {
        bool operator()(const uint32_t a, const uint32_t b) const
        {
            return a < b;
        }
    };

    typedef boost::multi_index_container<
        CodedTreeBlockPositionInfo, 
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<CoordinateKey>, 
                CodedTreeBlockPositionInfoForCoordinate, CoordinateComparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<RasterScanIndex>, 
                CodedTreeBlockPositionInfoForRasterScanIndex, uint32_tComparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<TileScanIndex>, 
                CodedTreeBlockPositionInfoForTileScanIndex, uint32_tComparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<TileIndex>, 
                CodedTreeBlockPositionInfoForTileIndex, uint32_tComparer>>>
        CodedTreeBlockPartitionContainer;

    struct TransformBlockPositionInfoForCoordinate
    {
        typedef Coordinate result_type;
        const result_type& operator()(const TransformBlockPositionInfo& info) 
            const
        {
            return info.block_coordinate;
        }
    };

    struct TransformBlockPositionInfoForZScanIndex
    {
        typedef uint32_t result_type;
        const result_type operator()(const TransformBlockPositionInfo& info)
            const
        {
            return info.index;
        }
    };

    struct TransformBlockIndex;

    struct TransformBlockPositionInfoForRasterScanIndex
    {
        typedef uint32_t result_type;
        const result_type operator()(const TransformBlockPositionInfo& info)
            const
        {
            return info.belong_to_raster_scan_index;
        }
    };

    struct TransformBlockPositionInfoForTileScanIndex
    {
        typedef uint32_t result_type;
        const result_type operator()(const TransformBlockPositionInfo& info)
            const
        {
            return info.belong_to_tile_scan_index;
        }
    };

    struct TransformBlockPositionInfoForTileIndex
    {
        typedef uint32_t result_type;
        const result_type operator()(const TransformBlockPositionInfo& info)
            const
        {
            return info.belong_to_tile_index;
        }
    };

    typedef boost::multi_index_container<
        TransformBlockPositionInfo,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<CoordinateKey>, 
                TransformBlockPositionInfoForCoordinate, CoordinateComparer>,                
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<TransformBlockIndex>, 
                TransformBlockPositionInfoForZScanIndex, uint32_tComparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<RasterScanIndex>, 
                TransformBlockPositionInfoForRasterScanIndex, uint32_tComparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<TileScanIndex>, 
                TransformBlockPositionInfoForTileScanIndex, uint32_tComparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<TileIndex>, 
                TransformBlockPositionInfoForTileIndex, uint32_tComparer>>>
        TransformBlockPartitionCantainer;

    void InitSingleTilePartition(uint32_t tile_begin_x, uint32_t tile_begin_y, 
                                 uint32_t tile_end_x, uint32_t tile_end_y,
                                 uint32_t tile_index, uint32_t ctb_log2_size_y, 
                                 uint32_t ctbs_per_row, uint32_t* tile_scan_index);

    void InitTransformBlockPartition(uint32_t ctb_log2_size_y, 
                                     uint32_t min_tb_log2_size_y);
            
    uint32_t frame_width_;
    uint32_t frame_height_;
    uint32_t ctb_log2_size_y_;
    uint32_t min_tb_log2_size_y_;
    bool initialzed_;
    CodedTreeBlockPartitionContainer tile_and_raster_partition_info_;
    TransformBlockPartitionCantainer transform_block_partition_info_;

};

#endif
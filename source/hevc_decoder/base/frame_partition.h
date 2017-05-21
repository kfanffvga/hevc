#ifndef _FRAME_PARTITION_H_
#define _FRAME_PARTITION_H_

#include <vector>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "hevc_decoder/base/basic_types.h"

class IFrameSyntaxContext;

class FramePartition
{
public:
    FramePartition(uint32 frame_width, uint32 frame_height);
    ~FramePartition();

    bool InitOnUniformSpacing(uint32 num_tile_cols, uint32 num_tile_rows,
                              uint32 ctb_log2_size_y, uint32 min_tb_log2_size_y);

    bool Init(const std::vector<uint32>& tile_cols_width, 
              const std::vector<uint32>& tile_rows_height, 
              uint32 ctb_log2_size_y, uint32 min_tb_log2_size_y);

    uint32 RasterScanToTileScan(uint32 index);

    // 6.4.1 判断邻居块对于当前块来说是否可用
    bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, const Coordinate& neighbouring_block,
        const IFrameSyntaxContext* frame_context);

private:
    struct CodedTreeBlockPositionInfo
    {
        Coordinate block_coordinate;
        uint32 block_raster_scan_index;
        uint32 block_tile_scan_index;
        uint32 tile_index;
    };

    struct TransformBlockPositionInfo
    {
        Coordinate block_coordinate;
        uint32 index;
        uint32 belong_to_raster_scan_index;
        uint32 belong_to_tile_scan_index;
        uint32 belong_to_tile_index;
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
        typedef uint32 result_type;
        const result_type operator()(const CodedTreeBlockPositionInfo& info) 
            const
        {
            return info.block_raster_scan_index;
        }
    };

    struct RasterScanIndex;

    struct CodedTreeBlockPositionInfoForTileScanIndex
    {
        typedef uint32 result_type;
        const result_type operator()(const CodedTreeBlockPositionInfo& info) 
            const
        {
            return info.block_tile_scan_index;
        }
    };

    struct TileScanIndex;

    struct CodedTreeBlockPositionInfoForTileIndex
    {
        typedef uint32 result_type;
        const result_type operator()(const CodedTreeBlockPositionInfo& info)
            const
        {
            return info.tile_index;
        }
    };

    struct TileIndex;

    struct UInt32Comparer
    {
        bool operator()(const uint32 a, const uint32 b) const
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
                CodedTreeBlockPositionInfoForRasterScanIndex, UInt32Comparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<TileScanIndex>, 
                CodedTreeBlockPositionInfoForTileScanIndex, UInt32Comparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<TileIndex>, 
                CodedTreeBlockPositionInfoForTileIndex, UInt32Comparer>>>
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
        typedef uint32 result_type;
        const result_type operator()(const TransformBlockPositionInfo& info)
            const
        {
            return info.index;
        }
    };

    struct TransformBlockIndex;

    struct TransformBlockPositionInfoForRasterScanIndex
    {
        typedef uint32 result_type;
        const result_type operator()(const TransformBlockPositionInfo& info)
            const
        {
            return info.belong_to_raster_scan_index;
        }
    };

    struct TransformBlockPositionInfoForTileScanIndex
    {
        typedef uint32 result_type;
        const result_type operator()(const TransformBlockPositionInfo& info)
            const
        {
            return info.belong_to_tile_scan_index;
        }
    };

    struct TransformBlockPositionInfoForTileIndex
    {
        typedef uint32 result_type;
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
                TransformBlockPositionInfoForZScanIndex, UInt32Comparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<RasterScanIndex>, 
                TransformBlockPositionInfoForRasterScanIndex, UInt32Comparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<TileScanIndex>, 
                TransformBlockPositionInfoForTileScanIndex, UInt32Comparer>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<TileIndex>, 
                TransformBlockPositionInfoForTileIndex, UInt32Comparer>>>
        TransformBlockPartitionCantainer;

    void InitSingleTilePartition(uint32 tile_begin_x, uint32 tile_begin_y, 
                                 uint32 tile_end_x, uint32 tile_end_y,
                                 uint32 tile_index, uint32 ctb_log2_size_y, 
                                 uint32 ctbs_per_row, uint32* tile_scan_index);

    void InitTransformBlockPartition(uint32 ctb_log2_size_y, 
                                     uint32 min_tb_log2_size_y);
            
    uint32 frame_width_;
    uint32 frame_height_;
    uint32 ctb_log2_size_y_;
    uint32 min_tb_log2_size_y_;
    bool initialzed_;
    CodedTreeBlockPartitionContainer tile_and_raster_partition_info_;
    TransformBlockPartitionCantainer transform_block_partition_info_;

};

#endif
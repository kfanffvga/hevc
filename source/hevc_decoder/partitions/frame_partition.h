#ifndef _FRAME_PARTITION_H_
#define _FRAME_PARTITION_H_

#include <vector>
#include <stdint.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "hevc_decoder/base/basic_types.h"

class ISliceSegmentAddressProvider;
class IFramePartitionCreatorInfoProvider;

class FramePartition
{
public:
    ~FramePartition();

    const IFramePartitionCreatorInfoProvider* GetCreationInfoProvider();
    bool RasterScanIndexToTileScanIndex(uint32_t raster_scan_index, 
                                        uint32_t* tile_scan_index);

    bool GetTileIndex(const Coordinate& block, uint32_t* tile_index);
    bool GetTileScanIndex(const Coordinate& block, uint32_t* tile_scan_index);
    bool GetRasterScanIndex(const Coordinate& block, 
                            uint32_t* raster_scan_index);

    bool GetCoordinateByTileScanIndex(uint32_t index, Coordinate* c);
    bool GetCoordinateByRasterScanIndex(uint32_t index, Coordinate* c);

    bool IsTheFirstCTBInTile(const Coordinate& block);
    bool IsTheFirstCTBInTileByRasterScanIndex(uint32_t index);
    bool IsTheFirstCTBInTileByTileScanIndex(uint32_t index);

    bool IsTheFirstCTBInRowOfFrameByRasterScanIndex(uint32_t index);
    bool IsTheFirstCTBInRowOfFrameByTileScanIndex(uint32_t index);
    bool IsTheFirstCTBInRowOfFrame(const Coordinate& block);

    bool IsTheFirstCTBInRowOfTileByRasterScanIndex(uint32_t index);
    bool IsTheFirstCTBInRowOfTileByTileScanIndex(uint32_t index);
    bool IsTheFirstCTBInRowOfTile(const Coordinate& block);

    // 6.4.1 判断邻居块对于当前块来说是否可用, 判断两个是否为同一个tile,同一个slice,
    // 并且当前块为邻居块的后面的可可用块
    bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, const Coordinate& neighbouring_block,
        const ISliceSegmentAddressProvider* slice_segment_address_provider);

private:
    friend class FramePartitionManager;

    FramePartition();

    bool Init(
        const std::shared_ptr<IFramePartitionCreatorInfoProvider>& provider);

    struct CodedTreeBlockPositionInfo
    {
        Coordinate block_coordinate;
        uint32_t block_raster_scan_index;
        uint32_t block_tile_scan_index;
        uint32_t tile_index;
        bool is_first_block_in_tile;
        bool is_first_block_in_row_of_tile;
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

    bool InitTransformBlockPartition(uint32_t ctb_log2_size_y, 
                                     uint32_t min_tb_log2_size_y);
            
    std::shared_ptr<IFramePartitionCreatorInfoProvider> provider_;
    CodedTreeBlockPartitionContainer tile_and_raster_partition_info_;
    TransformBlockPartitionCantainer transform_block_partition_info_;

};

#endif
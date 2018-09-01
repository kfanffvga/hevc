#ifndef _FRAME_PARTITION_H_
#define _FRAME_PARTITION_H_

#include <vector>
#include <stdint.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/coordinate.h"

class ISliceSegmentAddressProvider;
class IFramePartitionCreatorInfoProvider;

struct TransformBlockIndexInfo
{
    uint32_t tile_index;
    uint32_t raster_scan_index;
    uint32_t tile_scan_index;
    uint32_t zscan_index;
};

class FramePartition
{
public:
    ~FramePartition();

    uint32_t GetWidth();
    uint32_t GetHeight();
    uint32_t GetMinTBLog2SizeY();

    const IFramePartitionCreatorInfoProvider* GetCreationInfoProvider();
    bool RasterScanIndexToTileScanIndex(uint32_t raster_scan_index, 
                                        uint32_t* tile_scan_index);

    bool GetTileIndexByCTBCoordinate(const Coordinate& block, 
                                     uint32_t* tile_index);

    bool GetTileScanIndexByCTBCoordinate(const Coordinate& block, 
                                         uint32_t* tile_scan_index);

    bool GetRasterScanIndexByCTBCoordinate(const Coordinate& block, 
                                           uint32_t* raster_scan_index);

    bool GetTileIndexByTransformBlockCoordinate(const Coordinate& block, 
                                                uint32_t* tile_index);

    bool GetTileScanIndexByTransformBlockCoordinate(const Coordinate& block,
                                                    uint32_t* tile_scan_index);

    bool GetZScanIndexByByTransformBlockCoordinate(const Coordinate& block,
                                                   uint32_t* zscan_index);

    bool GetRasterScanIndexByTransformBlockCoordinate(
        const Coordinate& block, uint32_t* raster_scan_index);

    bool GetIndexInfoByTransformBlockCoordinate(const Coordinate& block, 
                                                TransformBlockIndexInfo* info);

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
           return b < a;
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
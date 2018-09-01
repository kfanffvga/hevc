#ifndef _BLOCK_SCAN_ORDER_PROVIDER_H_
#define _BLOCK_SCAN_ORDER_PROVIDER_H_

#include <array>
#include <vector>
#include <boost/multi_array.hpp>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/coordinate.h"

// 由于效率的考虑，本类不做任何的边界判断,暂时不考虑多线程问题

class BlockScanOrderProvider
{
public:
    enum BlockSize
    {
		BLOCK_SIZE_1X1 = 0,
		BLOCK_SIZE_2X2 = 1,
		BLOCK_SIZE_4X4 = 2,
		BLOCK_SIZE_8X8 = 3,
		BLOCK_SIZE_16X16 = 4,
		BLOCK_SIZE_32X32 = 5,
        UNKNOWN_BLOCK_SIZE = 6,
    };

    enum ScanType
    {
        UP_RIGHT_SCAN = 0,
        HORIZONTAL_SCAN = 1,
        VERTICAL_SCAN = 2,
        TRAVERSE_SCAN = 3
    };

    static BlockScanOrderProvider* GetInstance();

    ~BlockScanOrderProvider();

    Coordinate GetScanPosition(BlockSize block_size, ScanType scan_type, 
                               uint32_t index);

    BlockSize GetBlockSizeType(uint32_t block_border_length);

    bool GetLeftIndexByScanType(ScanType scan_type, BlockSize block_size, 
                                uint32_t current_index, uint32_t* left_index);

    bool GetTopIndexByScanType(ScanType scan_type, BlockSize block_size, 
                               uint32_t current_index, uint32_t* top_index);

    bool GetRightIndexByScanType(ScanType scan_type, BlockSize block_size, 
                                 uint32_t current_index, uint32_t* right_index);

    bool GetBottomIndexByScanType(ScanType scan_type, BlockSize block_size, 
                                  uint32_t current_index, 
                                  uint32_t* bottom_index);

private:
    BlockScanOrderProvider();
    void Init();
    void InitUpRightScanPosition();
    void InitHorizontalPosition();
    void InitVerticalPosition();
    void InitTraversePosition();

    uint32_t GetBlockBorderLength(BlockSize block_size);

    std::array<std::array<std::vector<Coordinate>, 4>, 6> scan_positions_;
    std::array<std::array<boost::multi_array<uint32_t, 2>, 4>, 6> scan_indeies_;
};

#endif

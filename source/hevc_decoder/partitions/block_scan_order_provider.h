#ifndef _BLOCK_SCAN_ORDER_PROVIDER_H_
#define _BLOCK_SCAN_ORDER_PROVIDER_H_

#include <array>
#include <vector>

#include "hevc_decoder/base/basic_types.h"

// 由于效率的考虑，本类不做任何的边界判断,暂时不考虑多线程问题

class BlockScanOrderProvider
{
public:
    enum BlockSize
    {
        BLOCK_SIZE_64X64 = 0,
        BLOCK_SIZE_32X32 = 1,
        BLOCK_SIZE_16X16 = 2,
        BLOCK_SIZE_8X8 = 3,
        BLOCK_SIZE_4X4 = 4,
        UNKNOWN_BLOCK_SIZE = 5
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

private:
    BlockScanOrderProvider();
    void Init();

    void InitUpRightScanPosition();
    void InitHorizontalPosition();
    void InitVerticalPosition();
    void InitTraversePosition();

    std::array<std::array<std::vector<Coordinate>, 4>, 5> scan_positions_;
};

#endif

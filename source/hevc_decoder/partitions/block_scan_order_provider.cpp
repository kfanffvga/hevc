#include "hevc_decoder/partitions/block_scan_order_provider.h"

#include <memory>
#include <map>
#include <cassert>

using std::unique_ptr;
using std::vector;
using std::map;
using std::make_pair;

static unique_ptr<BlockScanOrderProvider> block_scan_order_provider;

BlockScanOrderProvider* BlockScanOrderProvider::GetInstance()
{
    if (!block_scan_order_provider)
    {
        block_scan_order_provider.reset(new BlockScanOrderProvider());
        block_scan_order_provider->Init();
    }
    return block_scan_order_provider.get();
}

BlockScanOrderProvider::~BlockScanOrderProvider()
{

}

Coordinate BlockScanOrderProvider::GetScanPosition(BlockSize block_size, 
                                                   ScanType scan_type, 
                                                   uint32_t index)
{
    assert(block_size != UNKNOWN_BLOCK_SIZE);
    return scan_positions_[block_size][scan_type][index];
}

BlockScanOrderProvider::BlockSize BlockScanOrderProvider::GetBlockSizeType(
    uint32_t block_border_length)
{
    const static map<uint32_t, BlockSize> block_size_map = 
    {
        make_pair(64ui32, BLOCK_SIZE_64X64), make_pair(32ui32, BLOCK_SIZE_32X32),
        make_pair(16ui32, BLOCK_SIZE_16X16), make_pair(8ui32, BLOCK_SIZE_8X8),
        make_pair(4ui32, BLOCK_SIZE_4X4)
    };
    auto r = block_size_map.find(block_border_length);
    return block_size_map.end() == r ? UNKNOWN_BLOCK_SIZE : r->second;
}

BlockScanOrderProvider::BlockScanOrderProvider()
{

}

void BlockScanOrderProvider::Init()
{
    InitUpRightScanPosition();
    InitHorizontalPosition();
    InitVerticalPosition();
    InitTraversePosition();
}

void BlockScanOrderProvider::InitUpRightScanPosition()
{
    for (int32_t i = 0; i < 5; ++i)
    {
        int32_t length = 64 >> i;
        vector<Coordinate>& scan_pos = 
            scan_positions_[static_cast<BlockSize>(i)][UP_RIGHT_SCAN];
        scan_pos.resize(length * length);
        int32_t k = 0;
        for (int32_t j = 0; j < length; ++j)
        {
            for (int32_t y = j, x = 0; (y >= 0) && (x < length); ++x, --y)
            {
                scan_pos[k] = 
                    {static_cast<uint32_t>(x), static_cast<uint32_t>(y)};

                ++k;
            }
        }
        for (int32_t j = scan_pos.size() - 1; j >= k; --j)
        {
            int32_t reference_index = scan_pos.size() - j - 1;
            uint32_t x = length - scan_pos[reference_index].x - 1;
            uint32_t y = length - scan_pos[reference_index].y - 1;
            scan_pos[j] = {x, y};
        }
    }
}

void BlockScanOrderProvider::InitHorizontalPosition()
{
    for (uint32_t i = 0; i < 5; ++i)
    {
        uint32_t length = 64 >> i;
        vector<Coordinate>& scan_pos =
            scan_positions_[static_cast<BlockSize>(i)][HORIZONTAL_SCAN];

        scan_pos.resize(length * length);
        uint32_t j = 0;
        for (uint32_t y = 0; y < length; ++y)
            for (uint32_t x = 0; x < length; ++x, ++j)
                scan_pos[j] = {x, y};
    }
}

void BlockScanOrderProvider::InitVerticalPosition()
{
    for (uint32_t i = 0; i < 5; ++i)
    {
        uint32_t length = 64 >> i;
        vector<Coordinate>& scan_pos =
            scan_positions_[static_cast<BlockSize>(i)][VERTICAL_SCAN];

        scan_pos.resize(length * length);
        uint32_t j = 0;
        for (uint32_t x = 0; x < length; ++x)
            for (uint32_t y = 0; y < length; ++y, ++j)
                scan_pos[j] = {x, y};
    }
}

void BlockScanOrderProvider::InitTraversePosition()
{
    for (uint32_t i = 0; i < 5; ++i)
    {
        uint32_t length = 64 >> i;
        vector<Coordinate>& scan_pos =
            scan_positions_[static_cast<BlockSize>(i)][TRAVERSE_SCAN];

        scan_pos.resize(length * length);
        uint32_t j = 0;
        for (uint32_t y = 0; y < length; ++y)
        {
            if ((y & 0x1) == 0)
            {
                for (uint32_t x = 0; x < length; ++x, ++j)
                    scan_pos[j] = {x, y};
            }
            else
            {
                for (int32_t x = length - 1; x >= 0; --x, ++j)
                    scan_pos[j] = {static_cast<uint32_t>(x), y};
            }
        }   
    }
}


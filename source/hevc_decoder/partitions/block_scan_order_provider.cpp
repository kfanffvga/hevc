#include "hevc_decoder/partitions/block_scan_order_provider.h"

#include <memory>
#include <map>
#include <cassert>

#include "hevc_decoder/base/coordinate.h"

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
        make_pair(32ui32, BLOCK_SIZE_32X32), make_pair(16ui32, BLOCK_SIZE_16X16), 
		make_pair(8ui32, BLOCK_SIZE_8X8), make_pair(4ui32, BLOCK_SIZE_4X4),
		make_pair(2ui32, BLOCK_SIZE_2X2), make_pair(1ui32, BLOCK_SIZE_1X1)
    };
    auto r = block_size_map.find(block_border_length);
    return block_size_map.end() == r ? UNKNOWN_BLOCK_SIZE : r->second;
}

bool BlockScanOrderProvider::GetLeftIndexByScanType(ScanType scan_type, 
                                                    BlockSize block_size, 
                                                    uint32_t current_index, 
                                                    uint32_t* left_index)
{
    if (!left_index)
        return false;

    auto c = scan_positions_[block_size][scan_type][current_index];
    if (c.GetX() < 1)
        return false;

    *left_index = scan_indeies_[block_size][scan_type][c.GetX() - 1][c.GetY()];
    return true;
    
}

bool BlockScanOrderProvider::GetTopIndexByScanType(ScanType scan_type, 
                                                   BlockSize block_size, 
                                                   uint32_t current_index, 
                                                   uint32_t* top_index)
{
    if (!top_index)
        return false;

    auto c = scan_positions_[block_size][scan_type][current_index];
    if (c.GetY() < 1)
        return false;

    *top_index = scan_indeies_[block_size][scan_type][c.GetX()][c.GetY() - 1];
    return true;
}

bool BlockScanOrderProvider::GetRightIndexByScanType(ScanType scan_type, 
                                                     BlockSize block_size, 
                                                     uint32_t current_index, 
                                                     uint32_t* right_index)
{
    if (!right_index)
        return false;

    auto c = scan_positions_[block_size][scan_type][current_index];
    uint32_t length = GetBlockBorderLength(block_size);
    if (c.GetX() == length - 1)
        return false;

    *right_index = scan_indeies_[block_size][scan_type][c.GetX() + 1][c.GetY()];
    return true;
}

bool BlockScanOrderProvider::GetBottomIndexByScanType(ScanType scan_type, 
                                                      BlockSize block_size, 
                                                      uint32_t current_index, 
                                                      uint32_t* bottom_index)
{
    if (!bottom_index)
        return false;

    auto c = scan_positions_[block_size][scan_type][current_index];
    uint32_t length = GetBlockBorderLength(block_size);
    if (c.GetY() == length - 1)
        return false;

    *bottom_index = scan_indeies_[block_size][scan_type][c.GetX()][c.GetY() + 1];
    return true;
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

    for (uint32_t block_size = 0; block_size < 6; ++block_size)
    {
        uint32_t border_length = 
            GetBlockBorderLength(static_cast<BlockSize>(block_size));

        uint32_t block_area = border_length * border_length;
        for (uint32_t scan_type = 0; scan_type < 4; ++scan_type)
        {
            scan_indeies_[block_size][scan_type].resize(
                boost::extents[border_length][border_length]);

            for (uint32_t i = 0; i < block_area; ++i)
            {
                Coordinate c = scan_positions_[block_size][scan_type][i];
                scan_indeies_[block_size][scan_type][c.GetX()][c.GetY()] = i;
            }
        }
    }
}

void BlockScanOrderProvider::InitUpRightScanPosition()
{
    for (int32_t i = 0; i <= 5 ; ++i)
    {
        int32_t length = 32 >> (5 - i);
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
            uint32_t x = length - scan_pos[reference_index].GetX() - 1;
            uint32_t y = length - scan_pos[reference_index].GetY() - 1;
            scan_pos[j] = {x, y};
        }
    }
}

void BlockScanOrderProvider::InitHorizontalPosition()
{
    for (uint32_t i = 0; i <= 5; ++i)
    {
        uint32_t length = 32 >> (5 - i);
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
    for (uint32_t i = 0; i <= 5; ++i)
    {
        uint32_t length = 32 >> (5 - i);
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
    for (uint32_t i = 0; i <= 5; ++i)
    {
        uint32_t length = 32 >> (5 - i);
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

uint32_t BlockScanOrderProvider::GetBlockBorderLength(BlockSize block_size)
{
    const static map<BlockSize, uint32_t> border_length_map =
    {
        make_pair(BLOCK_SIZE_32X32, 32ui32), make_pair(BLOCK_SIZE_16X16, 16ui32),
        make_pair(BLOCK_SIZE_8X8, 8ui32), make_pair(BLOCK_SIZE_4X4, 4ui32),
        make_pair(BLOCK_SIZE_2X2, 2ui32), make_pair(BLOCK_SIZE_1X1, 1ui32),
        make_pair(UNKNOWN_BLOCK_SIZE, 0ui32)
    };
    auto r = border_length_map.find(block_size);
    return border_length_map.end() == r ? 0ui32 : r->second;
}


#include "hevc_decoder/base/tile_info.h"

#include <algorithm>

using std::vector;
using std::numeric_limits;

TileInfo::TileInfo(uint32_t num_tile_columns, uint32_t num_tile_rows, 
                   bool is_loop_filter_across_tiles_enabled)
    : is_uniform_spacing_(true)
    , num_tile_columns_(num_tile_columns)
    , num_tile_rows_(num_tile_rows)
    , is_loop_filter_across_tiles_enabled_(is_loop_filter_across_tiles_enabled)
    , column_width_()
    , row_height_()
{

}

TileInfo::TileInfo(const vector<uint32_t>& column_width, 
                   const vector<uint32_t>& row_height, 
                   bool is_loop_filter_across_tiles_enabled)
    : is_uniform_spacing_(false)
    , num_tile_columns_(column_width.size() + 1)
    , num_tile_rows_(row_height.size() + 1)
    , is_loop_filter_across_tiles_enabled_(is_loop_filter_across_tiles_enabled)
    , column_width_(column_width)
    , row_height_(row_height)
{
    column_width_.push_back(numeric_limits<uint32_t>::max());
    row_height_.push_back(numeric_limits<uint32_t>::max());
}

TileInfo::TileInfo()
    : is_uniform_spacing_(true)
    , num_tile_columns_(1)
    , num_tile_rows_(1)
    , is_loop_filter_across_tiles_enabled_(true)
    , column_width_()
    , row_height_()
{

}

bool TileInfo::IsUniformSpacing() const
{
    return is_uniform_spacing_;
}

uint32_t TileInfo::GetNumTileColumns() const
{
    return num_tile_columns_;
}

uint32_t TileInfo::GetNumTileRows() const
{
    return num_tile_rows_;
}

bool TileInfo::IsLoopFilterAcrossTilesEnabled() const
{
    return is_loop_filter_across_tiles_enabled_;
}

const std::vector<uint32_t>& TileInfo::GetColumnWidth() const
{
    return column_width_;
}

const std::vector<uint32_t>& TileInfo::GetRowHeight() const
{
    return row_height_;
}

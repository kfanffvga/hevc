#ifndef _TILE_INFO_H_
#define _TILE_INFO_H_

#include <stdint.h>
#include <vector>

class TileInfo
{
public:
    // 当调用该构造函数时候,表示平均分割
    TileInfo(uint32_t num_tile_columns, uint32_t num_tile_rows, 
             bool is_loop_filter_across_tiles_enabled);

    // 当调用该构造函数时候,表示指定分割点
    TileInfo(const std::vector<uint32_t>& column_width, 
             const std::vector<uint32_t>& row_height, 
             bool is_loop_filter_across_tiles_enabled);
    
    TileInfo();

    bool IsUniformSpacing() const;
    uint32_t GetNumTileColumns() const;
    uint32_t GetNumTileRows() const;
    bool IsLoopFilterAcrossTilesEnabled() const;

    // 当调用以下函数的时候,由于此时还不知道帧大小,因此,此处的最后tile是uint32_t的最大值
    // 并且column_width是相对大小
    const std::vector<uint32_t>& GetColumnWidth() const;
    const std::vector<uint32_t>& GetRowHeight() const;
    
private:
    bool is_uniform_spacing_;
    uint32_t num_tile_columns_;
    uint32_t num_tile_rows_;
    bool is_loop_filter_across_tiles_enabled_;
    std::vector<uint32_t> column_width_;
    std::vector<uint32_t> row_height_;
};
#endif
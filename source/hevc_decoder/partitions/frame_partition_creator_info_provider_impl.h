#ifndef _FRAME_PARTITION_CREATOR_INFO_PROVIDER_IMPL_H_
#define _FRAME_PARTITION_CREATOR_INFO_PROVIDER_IMPL_H_

#include "hevc_decoder/partitions/frame_partition_creator_info_provider.h"

//TODO: 此处用类与接口还是用一个简单的结构是需要斟酌的,但是时间原因,留待将来再研究
class FramePartitionCreatorInfoProviderImpl
    : public IFramePartitionCreatorInfoProvider
{
public:
    FramePartitionCreatorInfoProviderImpl(uint32_t width, uint32_t height);
    virtual ~FramePartitionCreatorInfoProviderImpl();

    bool Init(const std::vector<uint32_t>& tile_cols_width_in_ctb,
              const std::vector<uint32_t>& tile_rows_height_in_ctb,
              uint32_t ctb_log2_size_y, uint32_t min_tb_log2_size_y);

    bool Init(uint32_t num_tile_cols, uint32_t num_tile_rows,
              uint32_t ctb_log2_size_y, uint32_t min_tb_log2_size_y);

    virtual uint32_t GetWidth() const override;
    virtual uint32_t GetHeight() const override;
    virtual const std::vector<uint32_t>& GetTileColsWidthInCTB() const override;
    virtual const std::vector<uint32_t>& GetTileRowsHeightInCTB() const override;
    virtual uint32_t GetCTBLog2SizeY() const override;
    virtual uint32_t GetMinTBLog2SizeY() const override;
    
private:
    uint32_t width_;
    uint32_t height_;
    std::vector<uint32_t> tile_cols_width_in_ctb_;
    std::vector<uint32_t> tile_rows_height_in_ctb_;
    uint32_t ctb_log2_size_y_;
    uint32_t min_tb_log2_size_y_;


};
#endif
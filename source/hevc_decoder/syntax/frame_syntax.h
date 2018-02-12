#ifndef _FRAME_SYNTAX_H_
#define _FRAME_SYNTAX_H_

#include <memory>
#include <vector>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/vld_decoder/frame_info_provider_for_cabac.h"

class SliceSyntax;
class IFrameSyntaxContext;
class SliceSegmentSyntax;
class FramePartition;

// 每个帧应该都是独立可以解码的，因此，后面所有的预测器，转换器，去方块化器等所需要的信息，
// 均能在FrameSyntax 中获取，不管这些信息是怎么得到的，都应该在FrameSyntax传入到下一阶段
// 之前准备好

class FrameSyntax
{
public:
    static PictureOrderCount CalcPictureOrderCount(uint32_t previous_msb, 
                                                   uint32_t previous_lsb, 
                                                   bool is_idr_frame, 
                                                   uint32_t max_lsb, 
                                                   uint32_t lsb);

    FrameSyntax(IFrameSyntaxContext* frame_syntax_context);
    virtual ~FrameSyntax();

    bool AddSliceSegment(std::shared_ptr<SliceSegmentSyntax> slice_segment);
    bool HasFramePartition() const;

    const PictureOrderCount& GetPictureOrderCount() const;
    std::shared_ptr<FramePartition> GetFramePartition();
    bool GetSliceSegmentAddressByTileScanIndex(uint32_t tile_scan_index, 
                                               uint32_t* address) const;
    uint32_t GetContainCTUCountByTileScan();
    uint32_t GetCABACContextIndexInLastParsedSliceSegment();

private:
    friend class SliceSegmentContext;

    bool SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb);
    void SetFramePartition(
        const std::shared_ptr<FramePartition>& frame_partition);

    std::vector<std::unique_ptr<SliceSyntax>> slices_;
    PictureOrderCount picture_order_count_;
    IFrameSyntaxContext* frame_syntax_context_;
    std::shared_ptr<FramePartition> frame_partition_;
    uint32_t ctu_count_by_tile_scan_;

};
#endif
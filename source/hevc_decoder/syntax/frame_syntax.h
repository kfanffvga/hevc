#ifndef _FRAME_SYNTAX_H_
#define _FRAME_SYNTAX_H_

#include <memory>
#include <vector>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/vld_decoder/frame_info_provider_for_cabac.h"

class NalUnit;
class SliceSyntax;
class IFrameSyntaxContext;
class SliceSegmentSyntax;
class PredictorPaletteTableStorage;
class FramePartition;

// ÿ��֡Ӧ�ö��Ƕ������Խ���ģ���ˣ��������е�Ԥ������ת������ȥ���黯��������Ҫ����Ϣ��
// ������FrameSyntax �л�ȡ��������Щ��Ϣ����ô�õ��ģ���Ӧ����FrameSyntax���뵽��һ�׶�
// ֮ǰ׼����

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

    bool ParseSliceSegment(NalUnit* nal_unit, IFrameSyntaxContext* context);
    bool HasFramePartition() const;

    const PictureOrderCount& GetPictureOrderCount() const;
    std::shared_ptr<FramePartition> GetFramePartition();
    uint32_t GetContainCTUCountByTileScan();
    uint32_t GetCABACContextIndexInLastParsedSliceSegment();

    // 6.4.1 �ж��ھӿ���ڵ�ǰ����˵�Ƿ����, �ж������Ƿ�Ϊͬһ��tile,ͬһ��slice,
    // ���ҵ�ǰ��Ϊ�ھӿ�ĺ���Ŀɿ��ÿ�
    bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, const Coordinate& neighbouring_block);

private:
    friend class SliceSegmentContext;

    bool AddSliceSegment(std::shared_ptr<SliceSegmentSyntax> slice_segment);
    bool SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb);
    bool GetSliceSegmentAddress(uint32_t tile_scan_index, uint32_t* address)
        const;

    PredictorPaletteTableStorage* GetPredictorPaletteTableStorage();

    void SetFramePartition(
        const std::shared_ptr<FramePartition>& frame_partition);

    std::vector<std::unique_ptr<SliceSyntax>> slices_;
    PictureOrderCount picture_order_count_;
    IFrameSyntaxContext* frame_syntax_context_;
    std::shared_ptr<FramePartition> frame_partition_;
    uint32_t ctu_count_by_tile_scan_;
    std::shared_ptr<SliceSegmentSyntax> parsing_slice_segment_;
    std::unique_ptr<PredictorPaletteTableStorage> palette_table_storage_;

};
#endif
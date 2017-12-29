#ifndef _FRAME_SYNTAX_H_
#define _FRAME_SYNTAX_H_

#include <memory>
#include <vector>

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/variant_length_data_decoder/frame_info_provider_for_cabac.h"
#include "hevc_decoder/base/frame_info_provider_for_frame_partition.h"

class SliceSyntax;
class IFrameSyntaxContext;
class SliceSegmentSyntax;
class ISliceSegmentInfoProviderForCABAC;

class FrameSyntax : public IFrameInfoProviderForCABAC
                  , public FrameInfoProviderForFramePartition
{
public:
    static PictureOrderCount CalcPictureOrderCount(uint32_t previous_msb, 
                                                   uint32_t previous_lsb, 
                                                   bool is_idr_frame, 
                                                   uint32_t max_lsb, 
                                                   uint32_t lsb);

    FrameSyntax(IFrameSyntaxContext* frame_syntax_context);
    virtual ~FrameSyntax();

    bool AddSliceSegment(std::unique_ptr<SliceSegmentSyntax> slice_segment);
    virtual const PictureOrderCount& GetPictureOrderCount() const override;

private:
    friend class SliceSegmentContext;

    virtual uint32_t GetSliceAddressByRasterScanBlockIndex(uint32_t index) const
        override;
    
    virtual bool IsTheFirstBlockInTile(const Coordinate& block) const override;
    virtual bool IsTheFirstBlockInRowOfTile(const Coordinate& block) const 
        override;

    virtual bool IsTheFirstBlockInRowOfFrame(const Coordinate& block) const 
        override;

    virtual uint32_t GetCTBHeight() const override;
    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, 
        const Coordinate& neighbouring_block) const override;

    virtual const ICodingTreeBlockContext* GetCodingTreeBlockContext(
        const Coordinate& block) const override;

    virtual const ISliceSegmentInfoProviderForCABAC* 
        GetSliceSegmentInfoProviderForCABAC(uint32_t address) const override;

    bool SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb);

    std::vector<std::unique_ptr<SliceSyntax>> slices_;
    PictureOrderCount picture_order_count_;
    IFrameSyntaxContext* frame_syntax_context_;
};
#endif
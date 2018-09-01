#include "hevc_decoder/syntax/slice_segment_syntax.h"

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/syntax/slice_segment_header.h"
#include "hevc_decoder/syntax/slice_segment_header_context.h"
#include "hevc_decoder/syntax/slice_segment_context.h"
#include "hevc_decoder/syntax/slice_segment_data.h"
#include "hevc_decoder/syntax/slice_segment_data_context.h"
#include "hevc_decoder/syntax/byte_alignment.h"

using std::shared_ptr;

class SliceSegmentHeaderContext : public ISliceSegmentHeaderContext
{
public:
    SliceSegmentHeaderContext(ISliceSegmentContext* slice_segment_context, 
                              const SliceSegmentSyntax* slice_segment_syntax)
        : slice_segment_context_(slice_segment_context)
        , slice_segment_syntax_(slice_segment_syntax)
    {

    }

    virtual ~SliceSegmentHeaderContext()
    {

    }

    virtual void SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb)
        override
    {
        slice_segment_context_->SetPictureOrderCountByLSB(lsb, max_lsb);
    }

    virtual PictureOrderCount GetPictureOrderCount() const override
    {
        return slice_segment_context_->GetPictureOrderCount();
    }

    virtual NalUnitType GetNalUnitType() const override
    {
        return slice_segment_context_->GetNalUnitType();
    }

    virtual uint8_t GetNuhLayerID() const override
    {
        return slice_segment_context_->GetNuhLayerID();
    }

    virtual uint8_t GetNuhLayerIDByPOCValue(uint32_t poc_value) const override
    {
        return slice_segment_context_->GetNuhLayerIDByPOCValue(poc_value);
    }

    virtual const ParametersManager& GetParametersManager() const override
    {
        return slice_segment_context_->GetParametersManager();
    }

private:
    ISliceSegmentContext* slice_segment_context_;
    const SliceSegmentSyntax* slice_segment_syntax_;

};

class SliceSegmentDataContext : public ISliceSegmentDataContext
{
public:
    SliceSegmentDataContext(ISliceSegmentContext* slice_segment_context,
                            SliceSegmentHeader* header)
        : slice_segment_context_(slice_segment_context)
        , header_(header)
    {

    }

    virtual ~SliceSegmentDataContext()
    {

    }

    virtual void OnCodedTreeUnitParsed() override
    {

    }

    virtual shared_ptr<FramePartition> GetFramePartition() override
    {
        return slice_segment_context_->GetFramePartition();
    }

    virtual CABACContextStorage* GetCABACContextStorage() override
    {
        return slice_segment_context_->GetCABACContextStorage();
    }

    virtual uint32_t GetFirstCTUIndexOfTileScan() override
    {
        return slice_segment_context_->GetFirstCTUIndexOfTileScan();
    }

    virtual uint32_t GetCABACContextIndexInPriorSliceSegment() override
    {
        // 中断言表示cabac的检查出错了
        assert(header_->IsDependentSliceSegment());
        if (!header_->IsDependentSliceSegment())
            return 0;

        ISliceSegmentContext* context = slice_segment_context_;
        return context->GetCABACContextIndexInLastParsedSliceSegment();
    }

    virtual SliceSegmentHeader* GetSliceSegmentHeader() override
    {
        return header_;
    }

    virtual bool IsZScanOrderNeighbouringBlockAvailable(
        const Coordinate& current_block, const Coordinate& neighbouring_block)
        const
    {
        return slice_segment_context_->IsZScanOrderNeighbouringBlockAvailable(
            current_block, neighbouring_block);
    }

    virtual shared_ptr<PaletteTable> GetPredictorPaletteTable(
        bool is_the_first_ctu_in_slice_segment, const Coordinate& point) 
        override
    {
        return slice_segment_context_->GetPredictorPaletteTable(
            is_the_first_ctu_in_slice_segment, header_->GetCTBHeight(),
            header_->IsEntropyCodingSyncEnabled(), 
            header_->IsDependentSliceSegment(), point);
    }

    virtual void SavePredictorPaletteTable(
        const Coordinate& point, const shared_ptr<PaletteTable>& palette_table) 
        override
    {
        return slice_segment_context_->SavePredictorPaletteTable(point, 
                                                                 palette_table);
    }

    virtual const shared_ptr<CodingTreeUnit> GetCodingTreeUnit(
        uint32_t tile_scan_index) const override
    {
        auto slice_segment_syntax = 
            slice_segment_context_->GetSliceSegmentSyntax(tile_scan_index);
        if (!slice_segment_syntax)
            return shared_ptr<CodingTreeUnit>();

        return slice_segment_syntax->GetSliceSegmentData().GetCodingTreeUnit(
            tile_scan_index);
    }

private:
    ISliceSegmentContext* slice_segment_context_;
    SliceSegmentHeader* header_;
    SliceSegmentSyntax* slice_segment_;
};

SliceSegmentSyntax::SliceSegmentSyntax()
    : header_(new SliceSegmentHeader())
    , data_(new SliceSegmentData())
{

}

SliceSegmentSyntax::~SliceSegmentSyntax()
{

}

bool SliceSegmentSyntax::Parse(BitStream* bit_stream, 
                               ISliceSegmentContext* context)
{
    SliceSegmentHeaderContext header_context(context, this);
    bool success = header_->Parse(bit_stream, &header_context);
    if (!success)
        return false;

    success = context->OnSliceSegmentHeaderParsed(*header_);
    if (!success)
        return false;

    ByteAlignment byte_alignment;
    if (!byte_alignment.Parse(bit_stream))
        return false;

    SliceSegmentDataContext data_context(context, header_.get());
    success = data_->Parse(bit_stream, &data_context);

    return success;
}

const SliceSegmentHeader& SliceSegmentSyntax::GetSliceSegmentHeader() const
{
    return *header_;
}

const SliceSegmentData& SliceSegmentSyntax::GetSliceSegmentData() const
{
    return *data_;
}
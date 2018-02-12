#include "hevc_decoder/syntax/slice_segment_syntax.h"

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/syntax/slice_segment_header.h"
#include "hevc_decoder/syntax/slice_segment_header_context.h"
#include "hevc_decoder/syntax/slice_segment_context.h"
#include "hevc_decoder/syntax/slice_segment_data.h"
#include "hevc_decoder/syntax/slice_segment_data_context.h"

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
        return slice_segment_syntax_->GetNalUnitType();
    }

    virtual uint8_t GetNuhLayerID() const override
    {
        return slice_segment_syntax_->GetNuhLayerID();
    }

    virtual uint8_t GetNuhLayerIDByPOCValue(uint32_t poc_value) const override
    {
        return slice_segment_context_->GetNuhLayerIDByPOCValue(poc_value);
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

    virtual bool GetSliceSegmentAddressByTileScanIndex(
        uint32_t tile_scan_index, uint32_t* slice_segment_address) override
    {
        return slice_segment_context_->GetSliceSegmentAddressByTileScanIndex(
            tile_scan_index, slice_segment_address);
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

private:
    ISliceSegmentContext* slice_segment_context_;
    SliceSegmentHeader* header_;
};

SliceSegmentSyntax::SliceSegmentSyntax(
    NalUnitType nal_unit_type, uint8_t nuh_layer_id,
    const ParametersManager* parameters_manager)
    : nal_unit_type_(nal_unit_type)
    , nuh_layer_id_(nuh_layer_id)
    , header_(new SliceSegmentHeader(parameters_manager))
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

    bit_stream->ByteAlign();

    SliceSegmentDataContext data_context(context, header_.get());
    success = data_->Parse(bit_stream, &data_context);
    

    return success;
}

NalUnitType SliceSegmentSyntax::GetNalUnitType() const
{
    return nal_unit_type_;
}

uint8_t SliceSegmentSyntax::GetNuhLayerID() const
{
    return nuh_layer_id_;
}

const SliceSegmentHeader& SliceSegmentSyntax::GetSliceSegmentHeader() const
{
    return *header_;
}

const SliceSegmentData& SliceSegmentSyntax::GetSliceSegmentData() const
{
    return *data_;
}
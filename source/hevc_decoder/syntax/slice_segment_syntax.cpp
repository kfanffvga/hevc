#include "hevc_decoder/syntax/slice_segment_syntax.h"

#include "hevc_decoder/base/basic_types.h"
#include "hevc_decoder/syntax/slice_segment_header.h"
#include "hevc_decoder/syntax/slice_segment_header_context.h"
#include "hevc_decoder/syntax/slice_segment_context.h"

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

SliceSegmentSyntax::SliceSegmentSyntax(
    NalUnitType nal_unit_type, uint8_t nuh_layer_id,
    const ParametersManager* parameters_manager)
    : parameters_manager_(parameters_manager)
    , nal_unit_type_(nal_unit_type)
    , nuh_layer_id_(nuh_layer_id)
{

}

SliceSegmentSyntax::~SliceSegmentSyntax()
{

}

uint32_t SliceSegmentSyntax::GetQuantizationParameter() const
{
    return 1;
}

bool SliceSegmentSyntax::IsEntropyCodingSyncEnabled() const
{
    return false;
}

bool SliceSegmentSyntax::IsDependentSliceSegment() const
{
    return false;
}

uint32_t SliceSegmentSyntax::GetSliceSegmentAddress() const
{
    return 0;
}

uint32_t SliceSegmentSyntax::GetCABACStorageIndex() const
{
    return 0;
}

bool SliceSegmentSyntax::Parse(BitStream* bit_stream, 
                               ISliceSegmentContext* context)
{
    SliceSegmentHeader header(parameters_manager_);
    SliceSegmentHeaderContext header_context(context, this);
    header.Parse(bit_stream, &header_context);
    return true;
}

NalUnitType SliceSegmentSyntax::GetNalUnitType() const
{
    return nal_unit_type_;
}

uint8_t SliceSegmentSyntax::GetNuhLayerID() const
{
    return nuh_layer_id_;
}

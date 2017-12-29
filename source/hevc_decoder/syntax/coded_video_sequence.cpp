#include "hevc_decoder/syntax/coded_video_sequence.h"

#include <cassert>

#include "hevc_decoder/decode_processor_manager.h"
#include "hevc_decoder/syntax/frame_syntax.h"
#include "hevc_decoder/syntax/slice_segment_syntax.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/slice_segment_context.h"

using std::pair;
using std::make_pair;
using std::find_if;

class SliceSegmentContext : public ISliceSegmentContext
{
public:
    SliceSegmentContext(FrameSyntax* frame_syntax)
        : frame_syntax_(frame_syntax)
    {
        assert(frame_syntax);
    }

    virtual ~SliceSegmentContext()
    {

    }

    virtual void SetPictureOrderCountByLSB(uint32_t lsb, uint32_t max_lsb)
        override
    {
        frame_syntax_->SetPictureOrderCountByLSB(lsb, max_lsb);
    }

    virtual PictureOrderCount GetPictureOrderCount() const override
    {
        return frame_syntax_->GetPictureOrderCount();
    }

private:
    FrameSyntax* frame_syntax_;

};

CodedVideoSequence::CodedVideoSequence(
    DecodeProcessorManager* decode_processor_manager,
    ParametersManager* parameters_manager)
    : decode_processor_manager_(decode_processor_manager)
    , parameters_manager_(parameters_manager)
    , frame_syntax_()
{

}

CodedVideoSequence::~CodedVideoSequence()
{

}

uint32_t CodedVideoSequence::GetLayerID(uint32_t poc_value) const
{
    auto searchor =
        [poc_value](const pair<PictureOrderCount, uint32_t>& poc) -> bool
    {
        return poc.first.value == poc_value;
    };

    auto r = find_if(pocs_info_.begin(), pocs_info_.end(), searchor);
    return r != pocs_info_.end() ? r->second : 0;
}

bool CodedVideoSequence::GetPreviewPictureOrderCount(PictureOrderCount* poc) 
    const
{
    if (!poc || pocs_info_.empty())
        return false;

    *poc = pocs_info_[pocs_info_.size() - 1].first;
    return true;
}

void CodedVideoSequence::Flush()
{
    decode_processor_manager_->Flush();
    pocs_info_.clear();
}

bool CodedVideoSequence::PushNALOfSliceSegment(NalUnit* nal, bool is_idr_frame)
{
    if (!nal)
        return false;

    BitStream* bit_stream = nal->GetBitSteam();
    bool is_new_frame = bit_stream->ReadBool();
    if (is_new_frame)
    {
        bool success = decode_processor_manager_->Decode(frame_syntax_.get());
        if (!success)
            return false;

        if (is_idr_frame)
            Flush();

        frame_syntax_.reset(new FrameSyntax(this));
    }
    bit_stream->Seek(bit_stream->GetBytePosition(),
                     bit_stream->GetBitPosition() - 1);

    unique_ptr<SliceSegmentSyntax> slice_segment_syntax(
        new SliceSegmentSyntax(nal->GetNalUnitType(), nal->GetNuhLayerID(), 
                               parameters_manager_));
    SliceSegmentContext slice_segment_context(frame_syntax_.get());
    bool success = slice_segment_syntax->Parse(nal->GetBitSteam(),
                                               &slice_segment_context);
    if (!success)
        return false;

    success = frame_syntax_->AddSliceSegment(move(slice_segment_syntax));
    if (!success)
        return false;

    if (is_new_frame)
    {
        pocs_info_.push_back(
            make_pair(frame_syntax_->GetPictureOrderCount(), 
                      nal->GetNuhLayerID()));
    }
    return true;
}

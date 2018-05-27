#include "hevc_decoder/syntax/coded_video_sequence.h"

#include <cassert>

#include "hevc_decoder/decode_processor_manager.h"
#include "hevc_decoder/syntax/frame_syntax.h"
#include "hevc_decoder/syntax/slice_segment_syntax.h"
#include "hevc_decoder/base/stream/bit_stream.h"
#include "hevc_decoder/base/tile_info.h"
#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/slice_segment_header.h"

using std::pair;
using std::make_pair;
using std::shared_ptr;
using std::find_if;

CodedVideoSequence::CodedVideoSequence(
    DecodeProcessorManager* decode_processor_manager,
    ParametersManager* parameters_manager, 
    FramePartitionManager* frame_partition_manager,
    CABACContextStorage* cabac_context_storage)
    : decode_processor_manager_(decode_processor_manager)
    , parameters_manager_(parameters_manager)
    , frame_partition_manager_(frame_partition_manager)
    , cabac_context_storage_(cabac_context_storage)
    , frame_syntax_()
    , pocs_info_()
{

}

CodedVideoSequence::~CodedVideoSequence()
{

}

const ParametersManager& CodedVideoSequence::GetParametersManager() const
{
    return *parameters_manager_;
}

uint8_t CodedVideoSequence::GetLayerID(uint32_t poc_value) const
{
    auto searchor =
        [poc_value](const pair<PictureOrderCount, uint8_t>& poc) -> bool
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

FramePartitionManager* CodedVideoSequence::GetFramePartitionManager() const
{
    return frame_partition_manager_;
}

CABACContextStorage* CodedVideoSequence::GetCABACContextStorage() const
{
    return cabac_context_storage_;
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
    if (!frame_syntax_ || !frame_syntax_->ParseSliceSegment(nal, this))
        return false;

    if (is_new_frame)
    {
        pocs_info_.push_back(
            make_pair(frame_syntax_->GetPictureOrderCount(), 
                      nal->GetNuhLayerID()));
    }
    return true;
}

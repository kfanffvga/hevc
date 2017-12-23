#include "hevc_decoder/syntax/syntax_dispatcher.h"

#include <cassert>
#include <algorithm>

#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/video_parameter_set.h"
#include "hevc_decoder/syntax/picture_parameter_set.h"
#include "hevc_decoder/syntax/parameters_manager.h"
#include "hevc_decoder/syntax/sequence_parameter_set.h"
#include "hevc_decoder/syntax/frame_syntax.h"
#include "hevc_decoder/syntax/slice_segment_syntax.h"
#include "hevc_decoder/decode_processor_manager.h"
#include "hevc_decoder/base/stream/bit_stream.h"

using std::unique_ptr;
using std::move;
using std::make_pair;
using std::pair;
using std::find_if;

SyntaxDispatcher::SyntaxDispatcher(ParametersManager* parameters_manager, 
                                   DecodeProcessorManager* processor_manager)
    : parameters_manager_(parameters_manager)
    , decode_processor_manager_(processor_manager)
    , frame_syntax_(new FrameSyntax(this))
    , pocs_info_()
{

}

SyntaxDispatcher::~SyntaxDispatcher()
{

}

bool SyntaxDispatcher::CreateSyntaxAndDispatch(unique_ptr<NalUnit> nal_unit)
{
    if (!nal_unit)
        return false;

    switch (nal_unit->GetNalUnitType())
    {
        case NalUnitType::VPS_NUT:
        {
            unique_ptr<VideoParameterSet> vps(new VideoParameterSet());
            bool success = vps->Parse(nal_unit->GetBitSteam());
            if (success)
                success = parameters_manager_->AddVideoParameterSet(move(vps));

            return success;
        }

        case NalUnitType::PPS_NUT:
        {
            unique_ptr<PictureParameterSet> pps(new PictureParameterSet());
            bool success = pps->Parse(nal_unit->GetBitSteam());
            if (success)
                success = parameters_manager_->AddPictureParameterSet(move(pps));

            return success;
        }

        case NalUnitType::SPS_NUT:
        {
            unique_ptr<SequenceParameterSet> sps(new SequenceParameterSet());
            bool success = sps->Parse(nal_unit->GetBitSteam());
            if (success)
            {
                success =
                    parameters_manager_->AddSequenceParameterSet(move(sps));
            }
            return success;
        }
        case NalUnitType::TRAIL_N:
        case NalUnitType::TRAIL_R:
        case NalUnitType::TSA_N:
        case NalUnitType::TSA_R:
        case NalUnitType::STSA_N:
        case NalUnitType::STSA_R:
        case NalUnitType::RADL_N:
        case NalUnitType::RADL_R:
        case NalUnitType::RASL_N:
        case NalUnitType::RASL_R:
        case NalUnitType::BLA_W_LP:
        case NalUnitType::BLA_W_RADL:
        case NalUnitType::BLA_N_LP:
        case NalUnitType::CRA_NUT:
            return CreateSliceSegmentSyntaxAndDispatch(nal_unit.get(), false);

        case NalUnitType::IDR_W_RADL:
        case NalUnitType::IDR_N_LP:
        {
            return CreateSliceSegmentSyntaxAndDispatch(nal_unit.get(), true);
        }

        default:
            break;
    }

    return true;
}

bool SyntaxDispatcher::CreateSliceSegmentSyntaxAndDispatch(NalUnit* nal_unit,
                                                           bool is_idr_frame)
{
    assert(nal_unit);
    BitStream* bit_stream = nal_unit->GetBitSteam();
    bool is_new_frame = bit_stream->ReadBool();
    if (is_new_frame)
    {
        bool success = decode_processor_manager_->Decode(frame_syntax_.get());
        if (!success)
            return false;

        if (is_idr_frame)
        {
            decode_processor_manager_->Flush();
            pocs_info_.clear();
        }

        frame_syntax_.reset(new FrameSyntax(this));
    }
    bit_stream->Seek(bit_stream->GetBytePosition(), 
                     bit_stream->GetBitPosition() - 1);
    unique_ptr<SliceSegmentSyntax> slice_segment_syntax(
        new SliceSegmentSyntax(nal_unit->GetNalUnitType(),
                               nal_unit->GetNuhLayerID(), parameters_manager_,
                               frame_syntax_.get(), this));
    bool success = slice_segment_syntax->Parse(nal_unit->GetBitSteam());
    if (!success)
        return false;

    success = frame_syntax_->AddSliceSegment(move(slice_segment_syntax));
    if (!success)
        return false;

    if (is_new_frame)
    {
        pocs_info_.push_back(
            make_pair(frame_syntax_->GetPictureOrderCount(), 
                      nal_unit->GetNuhLayerID()));
    }
    return true;
}

uint32_t SyntaxDispatcher::GetLayerID(uint32_t poc_value) const
{
    auto searchor = 
        [poc_value](const pair<PictureOrderCount, uint32_t>& poc) -> bool
    {
        return poc.first.value == poc_value;
    };

    auto r = find_if(pocs_info_.begin(), pocs_info_.end(), searchor);
    return r != pocs_info_.end() ? r->second : 0;
}

bool SyntaxDispatcher::GetPreviewPictureOrderCount(PictureOrderCount* poc) const
{
    if (!poc || pocs_info_.empty())
        return false;

    *poc = pocs_info_[pocs_info_.size() - 1].first;
    return true;
}

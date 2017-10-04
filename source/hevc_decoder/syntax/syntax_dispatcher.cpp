#include "hevc_decoder/syntax/syntax_dispatcher.h"

#include <cassert>

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

SyntaxDispatcher::SyntaxDispatcher(ParametersManager* parameters_manager, 
                                   DecodeProcessorManager* processor_manager)
    : parameters_manager_(parameters_manager)
    , decode_processor_manager_(processor_manager)
    , frame_syntax_(new FrameSyntax())
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
    if (nal_unit->GetBitSteam()->ReadBool())
    {
        decode_processor_manager_->Decode(frame_syntax_.get());
        if (is_idr_frame)
            decode_processor_manager_->Flush();

        frame_syntax_.reset(new FrameSyntax());
    }
    nal_unit->GetBitSteam()->Seek(0, 0);
    unique_ptr<SliceSegmentSyntax> slice_segment_syntax(
        new SliceSegmentSyntax(frame_syntax_.get()));
    bool success = slice_segment_syntax->Parse(nal_unit->GetBitSteam());
    if (!success)
        return false;

    return frame_syntax_->AddSliceSegment(move(slice_segment_syntax));
}

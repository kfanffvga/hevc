#include "hevc_decoder/syntax/syntax_dispatcher.h"

#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/video_parameter_set.h"
#include "hevc_decoder/syntax/picture_parameter_set.h"
#include "hevc_decoder/syntax/parameters_manager.h"
#include "hevc_decoder/syntax/sequence_parameter_set.h"
#include "hevc_decoder/syntax/frame_syntax.h"
#include "hevc_decoder/predictors/predictor_manager.h"

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
            return CreateSliceSegmentSyntaxAndDispatch(nal_unit.get());

        case NalUnitType::IDR_W_RADL:
        case NalUnitType::IDR_N_LP:
        {
            //TODO: flush all
            return CreateSliceSegmentSyntaxAndDispatch(nal_unit.get());
        }

        default:
            break;
    }

    return true;
}

bool SyntaxDispatcher::CreateSliceSegmentSyntaxAndDispatch(NalUnit* nal_unit)
{

    return false;
}

#include "hevc_decoder/syntax/syntax_dispatcher.h"

#include "hevc_decoder/syntax/nal_unit.h"
#include "hevc_decoder/syntax/video_parameter_set.h"
#include "hevc_decoder/syntax/picture_parameter_set.h"
#include "hevc_decoder/syntax/parameters_manager.h"

using std::unique_ptr;
using std::move;

SyntaxDispatcher::SyntaxDispatcher(ParametersManager* parameters_manager)
    : parameters_manager_(parameters_manager)
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
        default:
            break;
    }

    return true;
}